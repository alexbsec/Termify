#ifndef _TERMIFY_CORE_RINGBUFFER_HPP
#define _TERMIFY_CORE_RINGBUFFER_HPP

#include "Common.hpp"

namespace termify::core {

class RingBuffer {
public:
  static void Update(AtomicContext &ctx, snd_pcm_t **ppPcm);
};


}

#endif // _TERMIFY_CORE_RINGBUFFER_HPP
