#include "Mixer.hpp"
#include "Common.hpp"
#include "FFmpegDecoder.hpp"
#include <mutex>
#include <sstream>

namespace termify::core {

Mixer::Mixer(PlaybackContext *pCtx, RingBufferContext *rCtx,
             VisualizationContext *vCtx, MixerResponseContext *mrCtx)
    : _decoder(std::make_unique<FFmpegDecoder>(_ctx)) {
  _ctx.playbackCtx = pCtx;
  _ctx.ringBufferCtx = rCtx;
  _ctx.visualizationCtx = vCtx;
  _ctx.mResponseCtx = mrCtx;
}

void Mixer::Play(const string &song) {
  if (_ctx.playbackCtx->isPlaying) {
    dispatchResponse("Already playing");
    return;
  }

  std::ostringstream oss;
  oss << "Playing " << song << "...";
  dispatchResponse(oss.str());
  _ctx.playbackCtx->isPlaying = true;
  _ctx.playbackCtx->shouldStop = false;

  _ctx.playbackCtx->playThread = std::thread([this, song]() {
    do {
      _decoder->Stream(song);
    } while (_repeatSong && !_ctx.playbackCtx->shouldStop.load());
  });

  _ctx.playbackCtx->playThread.detach();
}

void Mixer::Stop() {
  if (!_ctx.playbackCtx->isPlaying) {
    dispatchResponse("Nothing is playing");
    return;
  }

  _ctx.playbackCtx->shouldStop.store(true);
  _ctx.ringBufferCtx->ringCv.notify_all();
  if (_ctx.playbackCtx->playThread.joinable()) {
    _ctx.playbackCtx->playThread.join();
  }

  _repeatSong = false;
}

void Mixer::ToggleRepeat(bool toggle) {
  const string onOrOff = toggle ? "on" : "off";
  std::ostringstream oss;
  oss << "Repeat " << onOrOff;
  dispatchResponse(oss.str());
  _repeatSong = toggle;
}

void Mixer::dispatchResponse(const string &msg, bool isErr) {
  MixerResponse res{
      .id = _lastResId++,
      .message = msg,
      .isErr = isErr,
  };
  {
    std::lock_guard lk(_ctx.mResponseCtx->resMtx);
    _ctx.mResponseCtx->responses.push(std::move(res));
    _ctx.mResponseCtx->resNotProcessedCount.store(
        _ctx.mResponseCtx->responses.size());
    _ctx.mResponseCtx->resCv.notify_all();
  }
}

} // namespace termify::core
