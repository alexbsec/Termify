#ifndef _TERMIFY_INCLUDE_FFMPEG_DECODER_HPP
#define _TERMIFY_INCLUDE_FFMPEG_DECODER_HPP

#include "Common.hpp"
#include "Defines.hpp"

namespace termify::core {

class FFmpegDecoder {
public:
  explicit FFmpegDecoder(AtomicContext &ctx);

  void Stream(const string &query);

private:
  string downloadAudio(const string &query);
  int init(const string &source, AVCodecContext **ppCodecCtx);
  void resampleStereo(AVCodecContext *codecCtx);
  void openAlsa(snd_pcm_t **ppPcm);
  void decodeResample(AVCodecContext *codecCtx, AVPacket *pkt, AVFrame *frame,
                      int streamIndex, uint64 bufCapacity, uint8_t *outBuf);

  void worker(snd_pcm_t **ppPcm);

private:
  AtomicContext &_ctx;
  AVFormatContext *_fmtCtx;
  SwrContext *_swr;
  std::thread _consumer;

  bool _deleteAfter = false;
};

} // namespace termify::core

#endif // _TERMIFY_INCLUDE_FFMPEG_DECODER_HPP
