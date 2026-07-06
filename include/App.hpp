#ifndef INCLUDE_APP_HPP_
#define INCLUDE_APP_HPP_

#include <chrono>
#include <memory>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#include <GLFW/glfw3.h>
#endif
#include <spdlog/logger.h>

const float kDefaultDeltaTime = 0.016;
class App {
 public:
  App() = default;
  ~App() = default;
  void Initalize(uint16_t width, uint16_t height, const std::string &title);
  void Update();
  void Shutdown();
  [[nodiscard]] bool IsRunning() const;
  [[nodiscard]] GLFWwindow* GetWindow() const {return window_;};
  [[nodiscard]] float GetDeltaTime() const;
  [[nodiscard]] double GetTotalTimeElapsed() const;

 private:
  void CalculateDeltaTime();
  void InitializeLogging();
  void LogTime();
  GLFWwindow* window_ = nullptr;
  bool running_ = true;
  std::shared_ptr<spdlog::logger> logger_ = nullptr;
  
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time_ = {};
  std::chrono::time_point<std::chrono::high_resolution_clock> last_frame_time_ = {};
  
  double total_time_elapsed_ = 0.0;
  float delta_time_ = 0.016f; // kDefaultDeltaTime
  int last_second_ = -1;
};

[[nodiscard]] consteval bool UsingEmscripten() noexcept {
#ifdef __EMSCRIPTEN__ 
  return true;
#else
  return false;
#endif
}

#endif //INCLUDE_APP_HPP_