#ifndef INCLUDE_APP_HPP_
#define INCLUDE_APP_HPP_

#include "GPUContext.hpp"
#include "ImGuiManager.hpp"
#include "Scene.hpp"
#include "WindowManager.hpp"
#include "formatted_webgpu.h"
#include "StartupCoordinator.hpp"
#include "FrameRenderData.hpp"
#include "Input.hpp"

#include <chrono>
#include <memory>

#include <spdlog/logger.h>

const float kDefaultDeltaTime = 0.016;
class App {
 public:
  App() = default;
  ~App() = default;
  void Initalize(uint16_t width, uint16_t height, const std::string &title);
  void UpdateInitalization();
  void Update();

  bool IsInitalized(){ return coordinator_.IsInitalized(); }
  
  void Shutdown();
  [[nodiscard]] bool IsRunning() const;
  [[nodiscard]] float GetDeltaTime() const;
  [[nodiscard]] double GetTotalTimeElapsed() const;
  void StopApp(){ running_ = false;}

 private:
  WindowManager window_manager_ = {};
  StartupCoordinator coordinator_ = {};
  GPUContext gpu_ = {};
  Scene scene_ = {};
  ImGuiManager imgui_manager_ = {};
  FrameRenderData frame_render_data_ = {};
  Input input_ = {};
  void CalculateDeltaTime();
  void InitializeLogging();
  void LogTime();
  bool running_ = true;
  std::shared_ptr<spdlog::logger> logger_ = nullptr;
  
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time_ = {};
  std::chrono::time_point<std::chrono::high_resolution_clock> last_frame_time_ = {};
  
  double total_time_elapsed_ = 0.0;
  float delta_time_ = kDefaultDeltaTime;
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