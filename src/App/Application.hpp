#ifndef _TERMIFY_APP_APPLICATION_HPP
#define _TERMIFY_APP_APPLICATION_HPP

#include "../Core/Mixer.hpp"
#include "../Core/Cli.hpp"

namespace termify::app {
class Application {
public:
  Application();
  void Start();

private:
  void waveformLoop();
  void drawBar();

private:
  core::PlaybackContext _playbackCtx;
  core::RingBufferContext _ringBufferCtx;
  core::VisualizationContext _visualizationCtx;
  core::MixerResponseContext _mResponseCtx;
  std::unique_ptr<core::Mixer> _mixer;
  std::unique_ptr<core::Cli> _cli;
};

} // namespace termify::app

#endif
