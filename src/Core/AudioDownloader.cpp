#include "AudioDownloader.hpp"

namespace termify::core {

string DownloadAudio(const string &query) {
  const string tmp_template = "/tmp/tmp_audio.%(ext)s";
  // --quiet + --no-warnings suppress yt-dlp logs,
  // and shell redirection "> /dev/null 2>&1" kills any remaining stdout/stderr
  string cmd =
    "yt-dlp --quiet --no-warnings "
    "--extract-audio --audio-format mp3 "
    "-o \"" + tmp_template + "\" "
    "\"ytsearch1:" + query + "\" "
    "> /dev/null 2>&1";
  if (std::system(cmd.c_str()) != 0)
    throw std::runtime_error("yt-dlp download failed");
  return "/tmp/tmp_audio.mp3";
}

}
