#include "Mixer.hpp"
#include "Common.hpp"
#include "FFmpegDecoder.hpp"

namespace termify::core {

Mixer::Mixer(PlaybackContext *pCtx, RingBufferContext *rCtx,
             VisualizationContext *vCtx)
    : _decoder(std::make_unique<FFmpegDecoder>(_ctx)) {
  _ctx.playbackCtx = pCtx;
  _ctx.ringBufferCtx = rCtx;
  _ctx.visualizationCtx = vCtx;
}

void Mixer::Play(const string &song) {
  if (_ctx.playbackCtx->isPlaying) {
    std::cout << "Already playing\n";
    return;
  }

  std::cout << "Playing " << song << "...\n";
  _ctx.playbackCtx->isPlaying = true;
  _ctx.playbackCtx->shouldStop = false;

  _ctx.playbackCtx->playThread = std::thread([this, song]() {
    do {
      std::cout << "Streaming song...\n";
      _decoder->Stream(song);
      if (_repeatSong && !_ctx.playbackCtx->shouldStop.load())
        std::cout << "Repeating song...\n";
    } while (_repeatSong && !_ctx.playbackCtx->shouldStop.load());
  });

  _ctx.playbackCtx->playThread.detach();
}

void Mixer::Stop() {
  if (!_ctx.playbackCtx->isPlaying) {
    std::cout << "Nothing playing\n";
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
  std::cout << "Repeat " <<  onOrOff << std::endl;
  _repeatSong = toggle; 
}

} // namespace termify::core
