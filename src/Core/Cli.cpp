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
  return Command::UNKNOWN;
}

Cli::Cli(std::unique_ptr<Mixer> &mixer) 
  : _mixer(mixer) {}

void Cli::Run() {
  auto ctx = _mixer->GetCtxRef();
  std::string line;
  std::getline(std::cin, line);
  auto args = parseArgs(line);
  if (args.empty())
    return;

  switch (StringToCommand(args[0])) {
  case Command::EXIT:
    ctx->playbackCtx->gRunning = false;
    break;
  case Command::PLAY:
    if (args.size() != 2) {
      std::cout << "Usage: play \"song name\"\n";
      return;
    }     
    _mixer->Play(args[1]);
    break;
  case Command::STOP:
    _mixer->Stop();
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

}
