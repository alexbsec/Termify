#ifndef _TERMIFY_CORE_COMMON_HPP
#define _TERMIFY_CORE_COMMON_HPP

#include "Defines.hpp"

namespace termify::core {

struct PlaybackContext;
struct RingBufferContext;
struct VisualizationContext;

const int BUFFER_SIZE = 2048;

struct AtomicContext {
  PlaybackContext *playbackCtx;
  RingBufferContext *ringBufferCtx;
  VisualizationContext *visualizationCtx;
};

struct PlaybackContext {
  atomic_bool isPlaying, shouldStop, gRunning;
  std::thread playThread;
};

struct RingBufferContext {
  const int BUFFER_FRAMES = 44100 * 5;
  const int CHANNELS = 1;
  vector<int16> ringBuffer;
  atomic_uint32 writeIndex, readIndex;
  std::mutex ringMtx;
  condition_variable ringCv;
  atomic_bool decodeFinished;
};

struct VisualizationContext {
  const int BAR_ROW = 2;
  std::array<int16, BUFFER_SIZE> visBuffer{}; 
  std::mutex visMtx, coutMtx;
};

} // namespace termify::core

#endif // _TERMIFY_CORE_COMMON_HPP
