#include "Application.hpp"

namespace termify::app {

Application::Application()
    : _mixer(std::make_unique<core::Mixer>(&_playbackCtx, &_ringBufferCtx,
                                           &_visualizationCtx)),
      _cli(std::make_unique<core::Cli>(_mixer)) {
  _playbackCtx.gRunning = true;
  _ringBufferCtx.ringBuffer.resize(_ringBufferCtx.CHANNELS * _ringBufferCtx.BUFFER_FRAMES);
}

void Application::Start() {
  std::cout << "\033[2J\033[H\n\n" << std::flush;
  std::thread viz([this]() { waveformLoop(); });
  while (_playbackCtx.gRunning) {
    _cli->Run();
  }
  viz.join();
}

void Application::waveformLoop() {
  while (_playbackCtx.gRunning) {
    {
      std::lock_guard lk(_visualizationCtx.visMtx);
      drawBar();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
}

void Application::drawBar() {
  int maxv = 0;
  for (auto &s : _visualizationCtx.visBuffer)
    maxv = std::max<int>(maxv, std::abs(s));
  int level = std::min(40, maxv / 1000);
  std::cout << "\033[s\033[" << _visualizationCtx.BAR_ROW << ";1H\033[2K[";
  for (int i = 0; i < 40; i++)
    std::cout << (i < level ? '#' : ' ');
  std::cout << "]\033[u" << std::flush;
}

} // namespace termify::app
