#ifndef _TERMIFY_INCLUDE_AUDIO_DOWNLOADER_HPP
#define _TERMIFY_INCLUDE_AUDIO_DOWNLOADER_HPP

#include "Defines.hpp"

namespace termify::core {

class YtDlpManager {

public:
  static YtDlpManager &Self();
  bool Init();

  string DownloadAudioToFile(const string &query);

private:
  YtDlpManager() = default;

  std::filesystem::path getGlobalLocation();
  std::filesystem::path getYtDlpLocation();

private:
  std::filesystem::path _executablePath;
  std::filesystem::path _ytDlpExecutablePath;
  static bool _initialized;
};

} // namespace termify::core

#endif // _TERMIFY_INCLUDE_AUDIO_DOWNLOADER_HPP
