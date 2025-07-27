#ifndef _TERMIFY_CORE_MIXER_HPP
#define _TERMIFY_CORE_MIXER_HPP

#include "Defines.hpp"
#include "Common.hpp"
#include "FFmpegDecoder.hpp"

using AtomicBool = std::atomic<bool>;

namespace termify::core {

class Mixer {
public:
  Mixer(PlaybackContext *pCtx, RingBufferContext *rCtx, VisualizationContext *vCtx);

  void Play(const string& song);
  void Stop();

  inline AtomicContext *GetCtxRef() {
    return &_ctx;
  }

private:
  std::unique_ptr<FFmpegDecoder> _decoder;
  AtomicContext _ctx; 
};

}

#endif // _TERMIFY_CORE_MIXER_HPP
