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
#else
#include <GLFW/glfw3.h>
#endif


void App::Initalize(uint16_t width, uint16_t height, const std::string& title) {
  InitializeLogging();
  spdlog::info("Using Emscripten: {}", UsingEmscripten());
  start_time_ = std::chrono::high_resolution_clock::now();
  last_frame_time_ = std::chrono::high_resolution_clock::now();
  if (!glfwInit()){
    spdlog::critical("Failed to initalise GLFW!");
    assert(false);
  }
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  window_ = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

  spdlog::info("Created Window with dimensions: {}x{}", width, height);
  
  if (!window_){
    spdlog::critical("Failed to open Window");
    assert(false);
  }
}
void App::LogTime(){
  spdlog::debug("DeltaTime: {}", delta_time_);
  int current_second = static_cast<int>(total_time_elapsed_);
  if (current_second == last_second_ + 10){
    last_second_ = current_second;
    spdlog::info("Seconds since program opened: {}", current_second);
  }
  else{
    spdlog::trace("Time since program opened: {}", static_cast<float>(total_time_elapsed_));
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
  glfwPollEvents();
  if (glfwWindowShouldClose(window_)) running_ = false;
  // if (UsingEmscripten() && total_time_elapsed_ >= 20){
  //   running_ = false;
  // }
}
void App::Shutdown() {
    glfwDestroyWindow(window_);
    glfwTerminate();
}
bool App::IsRunning() const {
  return running_; 
}
GLFWwindow* App::GetWindow() const {
  return window_;
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