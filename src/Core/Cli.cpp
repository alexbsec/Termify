#include "Cli.hpp"
#include "Common.hpp"

namespace termify::core {

Command StringToCommand(const std::string &s) {
  if (s == "exit")
    return Command::EXIT;
  if (s == "play")
    return Command::PLAY;
  if (s == "stop")
    return Command::STOP;
  if (s == "repeat")
    return Command::REPEAT;
  return Command::UNKNOWN;
}

Cli::Cli(std::unique_ptr<Mixer> &mixer) : _mixer(mixer) {}

void Cli::Run() {
  auto ctx = _mixer->GetCtxRef();
  std::string line;
  std::getline(std::cin, line);
  auto args = parseArgs(line);
  if (args.empty())
    return;

  const string cmdStr = args[0];

  switch (StringToCommand(cmdStr)) {
  case Command::EXIT:
    ctx->playbackCtx->gRunning = false;
    break;
  case Command::PLAY: {
    if (args.size() != 2) {
      std::cout << "Usage: play \"song name\"\n";
      return;
    }
    const string song = args[1];
    _mixer->Play(song);
    break;
  }
  case Command::STOP:
    _mixer->Stop();
    break;
  case Command::REPEAT:
    _repeatRequested = !_repeatRequested;
    _mixer->ToggleRepeat(_repeatRequested);
    break;
  default:
    std::cout << "Unknown command\n";
  }
}

vector<string> Cli::parseArgs(const string &input) {
  vector<string> res;
  std::istringstream ss(input);
  string tok;
  while (ss >> std::quoted(tok)) {
    res.push_back(tok);
    tok.clear();
  }
  return res;
}

} // namespace termify::core
