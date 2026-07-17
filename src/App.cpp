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
  coordinator.StartupTick(gpu, imgui_manager, window_manager.GetNativeWindowHandle());
}
void App::Initalize(uint16_t width, uint16_t height, const std::string& title) {
  InitializeLogging();
  spdlog::info("Using Emscripten: {}", UsingEmscripten());
  window_manager.Initalize(width, height, title);
  start_time_ = std::chrono::high_resolution_clock::now();
  last_frame_time_ = std::chrono::high_resolution_clock::now();
  gpu.InitializeInstance();
  gpu.InitializeSurface(window_manager.GetNativeWindowHandle());
  scene.Initalize();
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
  CalculateDeltaTime();
  LogTime();
  window_manager.Update(*this);
  imgui_manager.BeginFrame();
  scene.Update(GetDeltaTime());
  UpdateGPUObjectData(gpu, scene.GetRegistry());
  gpu.Update(GetDeltaTime(), GetTotalTimeElapsed());
  imgui_manager.EndFrame(gpu.GetRenderPassEncoder());
  gpu.Render();
}
void App::Shutdown() {
    imgui_manager.Shutdown();
    window_manager.Shutdown();
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