#include "FFmpegDecoder.hpp"
#include "AudioDownloader.hpp"
#include "RingBuffer.hpp"
#include <libavcodec/avcodec.h>

namespace termify::core {

FFmpegDecoder::FFmpegDecoder(AtomicContext &ctx) : _ctx(ctx) {}

void FFmpegDecoder::Stream(const string &query) {
  _ctx.ringBufferCtx->decodeFinished.store(false);
  _ctx.ringBufferCtx->writeIndex.store(0);
  _ctx.ringBufferCtx->readIndex.store(0);
  _ctx.playbackCtx->shouldStop.store(false);
  _ctx.playbackCtx->isPlaying.store(true);
  _ctx.playbackCtx->isPaused.store(false);

  // 1) Download if it's a search term
  const string source = downloadAudio(query);

  // 2) FFmpeg init
  AVCodecContext *codecCtx = nullptr;
  int streamIndex = init(source, &codecCtx);

  // 3) Resampler to S16 stereo 44100
  resampleStereo(codecCtx);

  // 4) ALSA open
  snd_pcm_t *pcm = nullptr;
  openAlsa(&pcm);

  // 5) Prepare packet/frame+buffer
  AVPacket *pkt = av_packet_alloc();
  AVFrame *frame = av_frame_alloc();
  uint64 bufCapacity = 192000;
  auto outBuf = new uint8_t[bufCapacity];

  // 6) Consumer thread → ALSA
  std::thread consumer([&]() { worker(&pcm); });

  // 7) Producer → decode+resample→ring
  decodeResample(codecCtx, pkt, frame, streamIndex, bufCapacity, outBuf);
  // wait on consumer
  consumer.join();

  // cleanup
  delete[] outBuf;
  av_frame_free(&frame);
  av_packet_free(&pkt);
  swr_free(&_swr);
  avcodec_free_context(&codecCtx);
  avformat_close_input(&_fmtCtx);

  // delete temp file if we downloaded
  if (_deleteAfter) {
    std::filesystem::remove(source);
  }

  _ctx.playbackCtx->shouldStop = false;
  _ctx.playbackCtx->isPlaying = false;
  std::cout << "Done.\n";
}

string FFmpegDecoder::downloadAudio(const string &query) {
  string source = query;
  if (!std::filesystem::exists(source)) {
    source = YtDlpManager::Self().DownloadAudioToFile(query);
    _deleteAfter = true;
  }

  return source;
}

int FFmpegDecoder::init(const string &source, AVCodecContext **ppCodecCtx) {
  avformat_network_init();
  av_log_set_level(AV_LOG_QUIET);
  _fmtCtx = nullptr;
  if (avformat_open_input(&_fmtCtx, source.c_str(), nullptr, nullptr) < 0)
    throw std::runtime_error("Failed to open input");
  if (avformat_find_stream_info(_fmtCtx, nullptr) < 0)
    throw std::runtime_error("Failed to read stream info");

  int streamIndex =
      av_find_best_stream(_fmtCtx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
  if (streamIndex < 0)
    throw std::runtime_error("No audio stream");

  AVCodecParameters *codecpar = _fmtCtx->streams[streamIndex]->codecpar;
  const AVCodec *codec = avcodec_find_decoder(codecpar->codec_id);
  *ppCodecCtx = avcodec_alloc_context3(codec);
  avcodec_parameters_to_context(*ppCodecCtx, codecpar);
  avcodec_open2(*ppCodecCtx, codec, nullptr);
  return streamIndex;

  return streamIndex;
}

void FFmpegDecoder::resampleStereo(AVCodecContext *codecCtx) {
  _swr = swr_alloc();
  AVChannelLayout stereo;
  av_channel_layout_default(&stereo, _ctx.ringBufferCtx->CHANNELS);
  av_opt_set_chlayout(_swr, "in_chlayout", &codecCtx->ch_layout, 0);
  av_opt_set_sample_fmt(_swr, "in_sample_fmt", codecCtx->sample_fmt, 0);
  av_opt_set_int(_swr, "in_sample_rate", codecCtx->sample_rate, 0);
  av_opt_set_chlayout(_swr, "out_chlayout", &stereo, 0);
  av_opt_set_sample_fmt(_swr, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
  av_opt_set_int(_swr, "out_sample_rate", 44100, 0);
  swr_init(_swr);
}

void FFmpegDecoder::openAlsa(snd_pcm_t **ppPcm) {
  if (snd_pcm_open(ppPcm, "pipewire", SND_PCM_STREAM_PLAYBACK, 0) < 0)
    throw std::runtime_error("ALSA open failed");
  snd_pcm_set_params(*ppPcm, SND_PCM_FORMAT_S16_LE, SND_PCM_ACCESS_RW_INTERLEAVED,
                     _ctx.ringBufferCtx->CHANNELS, 44100, 1, 500000);
}

void FFmpegDecoder::decodeResample(AVCodecContext *codecCtx, AVPacket *pkt,
                                   AVFrame *frame, int streamIndex,
                                   uint64 bufCapacity, uint8_t *outBuf) {
  while (av_read_frame(_fmtCtx, pkt) >= 0 &&
         !_ctx.playbackCtx->shouldStop.load()) {
    if (pkt->stream_index == streamIndex) {
      avcodec_send_packet(codecCtx, pkt);
      while (avcodec_receive_frame(codecCtx, frame) == 0 &&
             !_ctx.playbackCtx->shouldStop.load()) {
        int outSamples = swr_convert(
            _swr, &outBuf,
            bufCapacity / sizeof(int16_t) / _ctx.ringBufferCtx->CHANNELS,
            (const uint8_t **)frame->data, frame->nb_samples);
        size_t samples = outSamples * _ctx.ringBufferCtx->CHANNELS, done = 0;
        while (done < samples && !_ctx.playbackCtx->shouldStop.load()) {
          std::unique_lock lk(_ctx.ringBufferCtx->ringMtx);
          size_t ri = _ctx.ringBufferCtx->readIndex,
                 wi = _ctx.ringBufferCtx->writeIndex,
                 rb = _ctx.ringBufferCtx->ringBuffer.size();
          size_t freeSp = (ri + rb - wi - 1) % rb;
          _ctx.ringBufferCtx->ringCv.wait(lk, [&]() {
            size_t ri2 = _ctx.ringBufferCtx->readIndex,
                   wi2 = _ctx.ringBufferCtx->writeIndex;
            return _ctx.playbackCtx->shouldStop.load() ||
                   ((ri2 + rb - wi2 - 1) % rb) > 0;
          });

          ri = _ctx.ringBufferCtx->readIndex;
          wi = _ctx.ringBufferCtx->writeIndex;
          freeSp = (ri + rb - wi - 1) % rb;
          size_t cont = std::min({freeSp, samples - done, rb - wi});
          memcpy(&_ctx.ringBufferCtx->ringBuffer[wi], ((int16 *)outBuf) + done,
                 cont * sizeof(int16));
          _ctx.ringBufferCtx->writeIndex.store((wi + cont) % rb);
          done += cont;
          lk.unlock();
          _ctx.ringBufferCtx->ringCv.notify_all();
        }
      }
    }
    av_packet_unref(pkt);
  }

  // signal end
  _ctx.ringBufferCtx->decodeFinished.store(true);
  _ctx.ringBufferCtx->ringCv.notify_all();
}

void FFmpegDecoder::worker(snd_pcm_t **ppPcm) { RingBuffer::Update(_ctx, ppPcm); }

} // namespace termify::core
