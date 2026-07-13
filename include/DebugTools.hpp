#ifndef INCLUDE_DEBUG_TOOLS_HPP
#define INCLUDE_DEBUG_TOOLS_HPP


#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#include <chrono>
#include <thread>
#endif

inline void DebugSleep(const float seconds) {
  const uint32_t millisecond_duration = static_cast<uint32_t>(seconds * 1000);
  #ifdef __EMSCRIPTEN__
    emscripten_sleep(millisecond_duration);
  #else
    std::this_thread::sleep_for(std::chrono::milliseconds(millisecond_duration));
  #endif
}

#endif // INCLUDE_DEBUG_TOOLS_HPP