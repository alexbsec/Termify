#include "Application.hpp"
#include "../Core/AudioDownloader.hpp"
#include <stdexcept>

namespace termify::app {

Application::Application()
    : _mixer(std::make_unique<core::Mixer>(&_playbackCtx, &_ringBufferCtx,
                                           &_visualizationCtx, &_mResponseCtx)),
      _cli(std::make_unique<core::Cli>(_mixer)) {
  _playbackCtx.gRunning = true;
  _ringBufferCtx.ringBuffer.resize(_ringBufferCtx.CHANNELS *
                                   _ringBufferCtx.BUFFER_FRAMES);
}

void Application::Start() {
  if (!core::YtDlpManager::Self().Init()) {
    throw std::runtime_error("Failed to initialize yt-dlp manager");
  }
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
      drawStatusBar();
      drawWaveformBar();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
}


void Application::drawStatusBar() {
  auto &vis = _visualizationCtx;
  // BAR_ROW is where your waveform lives
  const int TITLE_ROW  = vis.BAR_ROW - 2;
  const int STATUS_ROW = vis.BAR_ROW - 1;

  const std::string track  = _mixer->TrackName();
  const std::string repeat = _mixer->IsRepeatOn() ? "ON" : "OFF";
  const bool        paused = _playbackCtx.isPaused.load();

  constexpr const char* ANSI_BLUE   = "\033[34m";
  constexpr const char* ANSI_GREEN  = "\033[32m";
  constexpr const char* ANSI_YELLOW = "\033[33m";
  constexpr const char* ANSI_RESET  = "\033[0m";

  // 1) Draw title
  std::cout
    << "\033[s"                                      // save cursor
    << "\033[" << TITLE_ROW  << ";1H\033[2K"         // move+clear title row
    << ANSI_BLUE << "Termify v0.1.0" << ANSI_RESET   // colored title
    << "\033[u";                                     // restore

  // 2) Draw status
  std::cout
    << "\033[s"                                      // save again
    << "\033[" << STATUS_ROW << ";1H\033[2K"         // move+clear status row
       << "Now playing: " << ANSI_GREEN << track << ANSI_RESET
       << "    Repeat: "  << ANSI_BLUE  << repeat << ANSI_RESET
       << "    Paused: ";
  if (paused)
    std::cout << ANSI_YELLOW << "YES" << ANSI_RESET;
  else
    std::cout << ANSI_GREEN  << "NO"  << ANSI_RESET;
  std::cout
    << "\033[u"                                      // and restore
    << std::flush;
}


void Application::drawWaveformBar() {
  auto &vis = _visualizationCtx;
  int maxv = 0;
  for (auto s : vis.visBuffer)
    maxv = std::max(maxv, std::abs(s));
  int level = std::min(40, maxv / 1000);

  // save, go to BAR_ROW, clear, draw bar, restore
  std::cout << "\033[s"
            << "\033[" << vis.BAR_ROW << ";1H\033[2K["
            << std::string(level, '#') << std::string(40 - level, ' ') << "]"
            << "\033[u" << std::flush;
}

} // namespace termify::app
