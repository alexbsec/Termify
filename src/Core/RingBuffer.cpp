#include "RingBuffer.hpp"
#include "Common.hpp"
#include <algorithm>
#include <mutex>

namespace termify::core {

void RingBuffer::Update(AtomicContext &ctx, snd_pcm_t **ppPcm) {
  auto &ringBufferCtx = ctx.ringBufferCtx;
  auto &playbackCtx = ctx.playbackCtx;
  auto &visualCtx = ctx.visualizationCtx;
  while (!ringBufferCtx->decodeFinished.load() ||
         ringBufferCtx->readIndex.load() != ringBufferCtx->writeIndex.load()) {
    if (playbackCtx->shouldStop.load())
      break;
    std::unique_lock lk(ringBufferCtx->ringMtx);
    ringBufferCtx->ringCv.wait(lk, [&]() {
      return ringBufferCtx->decodeFinished.load() ||
             ringBufferCtx->writeIndex.load() !=
                 ringBufferCtx->readIndex.load();
    });
    uint64 rb = ctx.ringBufferCtx->ringBuffer.size();
    uint64 wi = ctx.ringBufferCtx->writeIndex.load(),
           ri = ctx.ringBufferCtx->readIndex.load();
    uint64 avail = (wi + rb - ri) % rb;
    if (avail == 0 && ctx.ringBufferCtx->decodeFinished)
      break;

    uint64 toRead = std::min<uint64>(avail, BUFFER_SIZE);
    std::vector<int16> chunk(toRead);
    uint32 first = std::min(toRead, rb - ri);
    memcpy(chunk.data(), &ringBufferCtx->ringBuffer[ri],
           first * sizeof(int16_t));
    if (first < toRead)
      memcpy(chunk.data() + first, &ringBufferCtx->ringBuffer[0],
             (toRead - first) * sizeof(int16_t));
    ctx.ringBufferCtx->readIndex.store((ri + toRead) % rb);
    lk.unlock();
    ringBufferCtx->ringCv.notify_all();

    if (playbackCtx->shouldStop.load())
      break;

    // update vis
    {
      std::lock_guard<std::mutex> vl(visualCtx->visMtx);
      uint64 c = std::min(visualCtx->visBuffer.size(), chunk.size());
      memcpy(visualCtx->visBuffer.data(), chunk.data(), c * sizeof(int16_t));
    }

    // write to ALSA (retry on XRUN)
    int frames = toRead / ringBufferCtx->CHANNELS, off = 0;
    while (frames > 0) {
      int w = snd_pcm_writei(*ppPcm, chunk.data() + off * ringBufferCtx->CHANNELS,
                             frames);
      if (w == -EPIPE) {
        snd_pcm_prepare(*ppPcm);
        continue;
      }
      if (w < 0) {
        snd_pcm_prepare(*ppPcm);
        break;
      }
      frames -= w;
      off += w;
    }
  }
}

} // namespace termify::core
