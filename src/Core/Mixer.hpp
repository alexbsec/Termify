#ifndef _TERMIFY_CORE_MIXER_HPP
#define _TERMIFY_CORE_MIXER_HPP

#include "Common.hpp"
#include "Defines.hpp"
#include "FFmpegDecoder.hpp"

using AtomicBool = std::atomic<bool>;

namespace termify::core {

class Mixer {
public:
  Mixer(PlaybackContext *pCtx, RingBufferContext *rCtx,
        VisualizationContext *vCtx, MixerResponseContext *mrCtx);

  void Play(const string &song);
  void ToggleRepeat(bool toggle);
  void Pause();
  void Resume();
  void Stop(bool force = false);

  inline const string &TrackName() const { return _trackName; }
  inline bool IsRepeatOn() const { return _repeatSong; }
  inline AtomicContext *GetCtxRef() { return &_ctx; }

private:
  void dispatchResponse(const string &msg, bool isErr = false);

private:
  std::unique_ptr<FFmpegDecoder> _decoder;
  AtomicContext _ctx;

  bool _repeatSong = false;
  int64 _lastResId = 1;
  string _trackName;
};

} // namespace termify::core

#endif // _TERMIFY_CORE_MIXER_HPP
