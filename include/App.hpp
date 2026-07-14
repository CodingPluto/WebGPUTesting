#ifndef INCLUDE_APP_HPP_
#define INCLUDE_APP_HPP_

#include "GPUContext.hpp"
#include "ImGuiManager.hpp"
#include "Scene.hpp"
#include "formatted_webgpu.h"
#include <chrono>
#include <memory>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <GLFW/glfw3.h>
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
  bool IsInitalized() {return initialized_state_ == InitializationState::Ready;}
  inline void SetInitalizedState(InitializationState state){initialized_state_ = state;}
  void UpdateInitalization();
  void Update();
  
  void Shutdown();
  [[nodiscard]] bool IsRunning() const;
  [[nodiscard]] GLFWwindow* GetWindow() const {return window_;};
  [[nodiscard]] float GetDeltaTime() const;
  [[nodiscard]] double GetTotalTimeElapsed() const;

 private:
  GPUContext gpu = {};
  Scene scene = {};
  ImGuiManager imgui_manager = {};
  InitializationState initialized_state_ = InitializationState::Uninitalised;
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