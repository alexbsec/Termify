#ifndef _TERMIFY_CORE_COMMON_HPP
#define _TERMIFY_CORE_COMMON_HPP

#include "Defines.hpp"
#include <queue>

namespace termify::core {

struct PlaybackContext;
struct RingBufferContext;
struct VisualizationContext;
struct MixerResponse;
struct MixerResponseContext;

const int BUFFER_SIZE = 2048;

struct AtomicContext {
  PlaybackContext *playbackCtx;
  RingBufferContext *ringBufferCtx;
  VisualizationContext *visualizationCtx;
  MixerResponseContext *mResponseCtx;
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

struct MixerResponseContext {
  atomic_uint32 resNotProcessedCount{0};
  std::queue<MixerResponse> responses;
  std::mutex resMtx;
  condition_variable resCv;
};

struct MixerResponse {
  int64 id;
  string message;
  bool isErr;
};

} // namespace termify::core

#endif // _TERMIFY_CORE_COMMON_HPP
