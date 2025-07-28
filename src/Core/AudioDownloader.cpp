#include "AudioDownloader.hpp"
#include <exception>
#include <stdexcept>

namespace termify::core {

bool YtDlpManager::_initialized = false;

YtDlpManager &YtDlpManager::Self() {
  static YtDlpManager instance;
  return instance;
}

bool YtDlpManager::Init() {
  if (_initialized)
    return false;

  try {
    _executablePath = getGlobalLocation();
    _ytDlpExecutablePath = getYtDlpLocation();
  } catch (const std::exception &e) {
    return false;
  }

  _initialized = true;
  return true;
}

string YtDlpManager::DownloadAudioToFile(const string &query) {
  if (!_initialized) {
    return "";
  }
  const string tmp_template = "/tmp/tmp_audio.%(ext)s";
  // --quiet + --no-warnings suppress yt-dlp logs,
  // and shell redirection "> /dev/null 2>&1" kills any remaining
  // stdout/stderr

  string cmd = _ytDlpExecutablePath.string() +
               " --quiet --no-warnings "
               "--extract-audio --audio-format mp3 "
               "-o \"" +
               tmp_template +
               "\" "
               "\"ytsearch1:" +
               query + "\" > /dev/null 2>&1";

  if (std::system(cmd.c_str()) != 0)
    throw std::runtime_error("yt-dlp download failed");
  return "/tmp/tmp_audio.mp3";
}

std::filesystem::path YtDlpManager::getGlobalLocation() {
#if defined(_WIN32)
  char path[MAX_PATH];
  GetModuleFileNameA(nullptr, path, MAX_PATH);
  return std::filesystem::path(path).parent_path();

#elif defined(__linux__)
  char path[4096];
  ssize_t count = readlink("/proc/self/exe", path, sizeof(path));
  if (count == -1)
    throw std::runtime_error("Failed to read /proc/self/exe");
  return std::filesystem::path(string(path, count)).parent_path();

#elif defined(__APPLE__)
  char path[4096];
  uint32_t size = sizeof(path);
  if (_NSGetExecutablePath(path, &size) != 0)
    throw std::runtime_error("Buffer too small for executable path");
  return std::filesystem::path(path).parent_path();

#else
  static_assert(false, "Unsupported platform");
#endif
}

std::filesystem::path YtDlpManager::getYtDlpLocation() {
#ifdef NDEBUG
  // Release build
  return "/usr/local/share/termify/bin/yt-dlp";
#else
  // Debug/dev build
  return _executablePath / ".local/bin/yt-dlp";
#endif
}

} // namespace termify::core
