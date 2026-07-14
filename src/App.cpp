#include "App.hpp"
#include "DebugTools.hpp"
#include "ImGuiManager.hpp"
#include "RenderDataPacker.hpp"
#include "formatted_webgpu.h"

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

//static_assert(true, "App.cpp is compiled");

void App::UpdateInitalization(){
  spdlog::info("Initalization stage: {}", initialized_state_);
  DebugSleep(0.1);
  switch (initialized_state_){
    case InitializationState::Uninitalised: {
      wgpu::RequestAdapterOptions adapter_options = {};
      gpu.StartAdapterRequest(&adapter_options, *this);
      initialized_state_ = InitializationState::RequestedAdapter;
      return;
    }
    case InitializationState::RequestedAdapter:
      gpu.ProcessEvents();
      return;
    case InitializationState::RequestingDevice:{
      gpu.InspectAdapter(gpu.GetAdapter());
      wgpu::DeviceDescriptor device_descriptor = {};
      device_descriptor.nextInChain = nullptr;
      device_descriptor.label = WGPUStringView{.data = "TestDevice",.length = WGPU_STRLEN};
      device_descriptor.requiredFeatureCount = 0;
      device_descriptor.requiredLimits = nullptr;
      device_descriptor.defaultQueue.nextInChain = nullptr;
      device_descriptor.defaultQueue.label = WGPUStringView{.data = "DefaultQueue",.length = WGPU_STRLEN};
      wgpu::Limits limits = {};
      limits.maxVertexAttributes = 8;
      limits.maxVertexBuffers = 4;
      limits.maxBufferSize = 6 * 10 * sizeof(float);
      limits.maxVertexBufferArrayStride = 10 * sizeof(float);
      limits.maxInterStageShaderVariables = 3;
      limits.maxBindGroups = 1;
      limits.maxUniformBuffersPerShaderStage = 1;
      limits.maxUniformBufferBindingSize = 2000;
      limits.maxStorageBufferBindingSize = 1024 * 1024;
      limits.maxStorageBuffersPerShaderStage = 1;
      device_descriptor.requiredLimits = &limits;
      device_descriptor.SetDeviceLostCallback(wgpu::CallbackMode::AllowSpontaneous, []([[maybe_unused]]wgpu::Device const& device, wgpu::DeviceLostReason reason, wgpu::StringView message){
        spdlog::warn("Device lost. reason: {}", reason);
        if (message.data && message.length > 0) spdlog::info("({})", message);
      });
      device_descriptor.SetUncapturedErrorCallback([]([[maybe_unused]]wgpu::Device const& device, wgpu::ErrorType error, wgpu::StringView message){
        spdlog::error("Device uncaptured error: {}", error);
        if (message.data && message.length > 0) spdlog::info("({})", message);
      });
      gpu.StartDeviceRequest(&device_descriptor, *this);
      initialized_state_ = InitializationState::RequestedDevice;
      return;
    }
    case InitializationState::RequestedDevice:
      gpu.ProcessEvents();
      return;
    case InitializationState::ReceivedAdapterAndDevice:
      gpu.InspectDevice(gpu.GetDevice());
      gpu.ConfigureQueue();
      gpu.CreateShaderModules();
      gpu.ConfigureSurface();
      gpu.CreateRenderPipeline();
      gpu.CreateComputePipeline();
      imgui_manager.Initialize(window_, gpu.GetDevice(), gpu.GetSurfaceFormat());
      initialized_state_ = InitializationState::Ready;
      return;
    default:
      spdlog::error("Invalid InitializationState enum value");
      return;
  }
}
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
  gpu.InitializeInstance();
  gpu.InitializeSurface(window_);
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
  glfwPollEvents();
  if (glfwWindowShouldClose(window_)) running_ = false;
  // if (UsingEmscripten() && total_time_elapsed_ >= 20){
  //   running_ = false;
  // }
  imgui_manager.BeginFrame();
  scene.Update(GetDeltaTime());
  UpdateGPUObjectData(gpu, scene.GetRegistry());
  gpu.Update(GetDeltaTime(), GetTotalTimeElapsed());
  imgui_manager.EndFrame(gpu.GetRenderPassEncoder());
  gpu.Render();
}
void App::Shutdown() {
    imgui_manager.Shutdown();
    glfwDestroyWindow(window_);
    glfwTerminate();
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