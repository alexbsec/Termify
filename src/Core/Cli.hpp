#ifndef _TERMIFY_CORE_CLI_HPP
#define _TERMIFY_CORE_CLI_HPP

#include "Defines.hpp"
#include "Mixer.hpp"

namespace termify::core {

enum class Command { EXIT, PLAY, STOP, REPEAT, UNKNOWN };
inline std::unordered_map<Command, uint32_t> cmdMap = {
    {Command::EXIT, 0},
    {Command::PLAY, 1},
    {Command::STOP, 0},
    {Command::REPEAT, 0},
    {Command::UNKNOWN, 0},
};

class Cli {
public:
  explicit Cli(std::unique_ptr<Mixer> &mixer);
  ~Cli();

  void Run();
  void WaveformLoop();

private:
  vector<string> parseArgs(const string &input);
  void drawBar();
  void printLog(const string &msg);
  
  void awaitMixerResponse();

private:
  atomic_bool _repeatRequested{false};
  std::unique_ptr<Mixer> &_mixer;
  std::thread _responseThread;
};

}

#endif // _TERMIFY_CORE_CLI_HPP
