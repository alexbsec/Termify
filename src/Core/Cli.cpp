#include "Cli.hpp"
#include "Common.hpp"

namespace termify::core {

Command StringToCommand(const std::string &s) {
  if (s == "exit" || s == ".q")
    return Command::EXIT;
  if (s == "play" || s == ".p")
    return Command::PLAY;
  if (s == "stop" || s == ".s")
    return Command::STOP;
  if (s == "loop" || s == ".l")
    return Command::REPEAT;
  if (s == "pause" || s == ".P") 
    return Command::PAUSE;
  if (s == "resume" || s == ".r") {
    return Command::RESUME;
  }
  return Command::UNKNOWN;
}

Cli::Cli(std::unique_ptr<Mixer> &mixer) : _mixer(mixer) {
  _responseThread = std::thread([this, &mixer]() {
    while (mixer->GetCtxRef()->playbackCtx->gRunning) {
      awaitMixerResponse();
    }
  });
  _responseThread.detach();
}

Cli::~Cli() {
  if (_responseThread.joinable()) {
    _responseThread.join();
  }
}

void Cli::Run() {
  auto ctx = _mixer->GetCtxRef();
  std::string line;
  std::getline(std::cin, line);
  auto args = parseArgs(line);
  if (args.empty())
    return;

  const string cmdStr = args[0];

  switch (StringToCommand(cmdStr)) {
  case Command::EXIT: {
    const bool forceStop = true;
    _mixer->Stop(forceStop);
    ctx->playbackCtx->gRunning = false;
    break;
  }
  case Command::PLAY: {
    if (args.size() != 2) {
      std::cout << "Usage: .p \"song name\"\n";
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
  case Command::PAUSE:
    _mixer->Pause();
    break;
  case Command::RESUME:
    _mixer->Resume();
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

void Cli::awaitMixerResponse() {
  // here we log the response
  // this should be async
  auto ctx = _mixer->GetCtxRef();
  while (ctx->mResponseCtx->resNotProcessedCount.load() != 0) {
    std::lock_guard lk(ctx->mResponseCtx->resMtx);
    if (ctx->mResponseCtx->responses.empty()) continue;
    auto mixerResponse = ctx->mResponseCtx->responses.front();
    ctx->mResponseCtx->responses.pop();

    if (mixerResponse.isErr) {
      // handle
    }
    std::cout << mixerResponse.message << std::endl;
    ctx->mResponseCtx->resNotProcessedCount.store(
        ctx->mResponseCtx->responses.size());
  }
}

} // namespace termify::core
