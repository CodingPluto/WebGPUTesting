#include "App.hpp"

#include <chrono>
#include <cmath>
#include <cstdint>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#include <GLFW/glfw3.h>
#else
#include <GLFW/glfw3.h>
#endif

#include "DebugTools.hpp"
#include "ImGuiManager.hpp"
#include "RenderDataPacker.hpp"
#include "formatted_webgpu.h"
#include "GPULogger.hpp"


void App::UpdateInitalization(){
  DebugSleep(0.1);
  coordinator_.StartupTick(gpu_);
  if (coordinator_.IsInitalized()){
    gpu_.ConfigureQueue(); // Change this so GPU owns the Coordinator and therefore this will all go in there after GPU has initalised and we can just poll from here
    // job for you after game jam (:
    gpu_.CreateShaderModules();
    gpu_.ConfigureSurface();
    gpu_.CreateRenderPipeline();
    gpu_.CreateComputePipeline();
    imgui_manager_.Initialize(window_manager_.GetNativeWindowHandle(), gpu_.GetDevice(), gpu_.GetSurfaceFormat());
    spdlog::info("Application Initalization Complete");
  }
}
void App::Initalize(uint16_t width, uint16_t height, const std::string& title) {
  InitializeLogging();
  spdlog::info("Using Emscripten: {}", UsingEmscripten());
  window_manager_.Initalize(width, height, title);
  input_.Initialize(window_manager_.GetNativeWindowHandle());
  start_time_ = std::chrono::high_resolution_clock::now();
  last_frame_time_ = std::chrono::high_resolution_clock::now();
  gpu_.InitializeInstance();
  gpu_.InitializeSurface(window_manager_.GetNativeWindowHandle());
  scene_.Initalize();
}
void App::LogTime(){
  int current_second = static_cast<int>(total_time_elapsed_);
  if (current_second == last_second_ + 10){
    last_second_ = current_second;
    spdlog::info("Seconds since program opened: {}", current_second);
    spdlog::debug("DeltaTime: {}", delta_time_);
  }
  else{
    spdlog::trace("Time since program opened: {}", static_cast<float>(total_time_elapsed_));
    spdlog::trace("DeltaTime: {}", delta_time_);
  }
}
void App::CalculateDeltaTime(){
  auto current_time = std::chrono::high_resolution_clock::now();
  delta_time_ = std::chrono::duration<float>(current_time - last_frame_time_).count();
  last_frame_time_ = current_time;
  total_time_elapsed_ += delta_time_;
}
void App::Update() {
  window_manager_.Update(*this, input_);
  if (!window_manager_.IsFocused()){
    spdlog::info("Window is not focused, skipping frame update");
    #ifndef __EMSCRIPTEN__
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    #endif
    return;
  }
  CalculateDeltaTime();
  LogTime();
  imgui_manager_.BeginFrame();
  scene_.Update(GetDeltaTime(), input_, window_manager_);
  UpdateGPUObjectData(gpu_, frame_render_data_, scene_.GetRegistry());
  gpu_.Update(GetDeltaTime(), GetTotalTimeElapsed());
  imgui_manager_.EndFrame(gpu_.GetRenderPassEncoder());
  gpu_.Render();
  input_.Update();
}
void App::Shutdown() {
    imgui_manager_.Shutdown();
    window_manager_.Shutdown();
}
bool App::IsRunning() const {
  return running_; 
}
float App::GetDeltaTime() const {
  return delta_time_;
}
double App::GetTotalTimeElapsed() const {
  return total_time_elapsed_;
}
void App::InitializeLogging() {
  logger_ = spdlog::stdout_color_mt("app");
  spdlog::set_default_logger(logger_);
  spdlog::set_level(spdlog::level::debug);
  spdlog::flush_on(spdlog::level::debug);
}