#ifndef _TERMIFY_CORE_CLI_HPP
#define _TERMIFY_CORE_CLI_HPP

#include "Defines.hpp"
#include "Mixer.hpp"

namespace termify::core {

enum class Command { EXIT, PLAY, STOP, UNKNOWN };
inline std::unordered_map<Command, uint32_t> cmdMap = {
    {Command::EXIT, 0},
    {Command::PLAY, 1},
    {Command::STOP, 0},
    {Command::UNKNOWN, 0},
};

class Cli {
public:
  explicit Cli(std::unique_ptr<Mixer> &mixer);

  void Run();
  void WaveformLoop();

private:
  vector<string> parseArgs(const string &input);
  void drawBar();
  void printLog(const string &msg);

private:
  std::unique_ptr<Mixer> &_mixer;
};

}

#endif // _TERMIFY_CORE_CLI_HPP
