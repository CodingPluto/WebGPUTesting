#include <iostream>
#include <cassert>
#include <memory>
#include <print>
#include <chrono>
#include <spdlog/logger.h>
#include <thread>

#include <webgpu/webgpu_cpp.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#include <GLFW/glfw3.h>
#endif
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "glfw3webgpu.h"
#include "formatted_webgpu.h"


struct GPUContext{
  wgpu::Instance instance = {};
  wgpu::Adapter adapter = {};
  wgpu::Device device = {};
  wgpu::Queue queue = {};
  wgpu::Surface surface = {};
};
struct App{
  GLFWwindow* window;
  GPUContext gpu;
  bool running = true;
  InitializationState initialized_state = InitializationState::Uninitalised;
  std::shared_ptr<spdlog::logger> logger;
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
};



long long GetTotalProgramTimeElapsedMilliseconds(App &app){
  std::chrono::time_point<std::chrono::high_resolution_clock> current_time = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(current_time - app.start_time).count();
}


void InitaliseLogging(App &app){
  app.logger = spdlog::stdout_color_mt("app");
  spdlog::set_default_logger(app.logger);
  spdlog::set_level(spdlog::level::debug);
  spdlog::flush_on(spdlog::level::warn);
}
[[nodiscard]] consteval wgpu::CallbackMode AdapterCallbackMode(){
  #ifdef __EMSCRIPTEN__
    return wgpu::CallbackMode::AllowSpontaneous;
  #endif
  return wgpu::CallbackMode::AllowProcessEvents;
}
[[nodiscard]] consteval inline bool UsingEmscripten() noexcept{
  #ifdef __EMSCRIPTEN__ 
    return true;
  #endif
  return false;
}
void StartAdapterRequest(App &app, const wgpu::RequestAdapterOptions *options) {
  app.gpu.instance.RequestAdapter(
    options,AdapterCallbackMode(), [&app](wgpu::RequestAdapterStatus status, wgpu::Adapter result_adapter, wgpu::StringView message) {
      if (status == wgpu::RequestAdapterStatus::Success) {
          app.gpu.adapter = result_adapter;
          app.initialized_state = InitializationState::RequestingDevice; 
      } else {
          spdlog::error("Failed to get an adapter: {}", message);
          app.initialized_state = InitializationState::Failed;
      }
    }
  );
}
void StartDeviceRequest(App &app, const wgpu::DeviceDescriptor *descriptor){
  app.gpu.adapter.RequestDevice(
    descriptor, AdapterCallbackMode(), [&app](wgpu::RequestDeviceStatus status, wgpu::Device result_device, [[maybe_unused]]wgpu::StringView message) {
      if (status == wgpu::RequestDeviceStatus::Success) {
        app.gpu.device = result_device;
        app.initialized_state = InitializationState::ReceivedAdapterAndDevice;
      } else {
        spdlog::error("Failed to get a device: {}", message);
        app.initialized_state = InitializationState::Failed;
      }
    }
  );
}
void OutputFeatures(const wgpu::SupportedFeatures &features){
  for (size_t i = 0; i < features.featureCount; ++i){
    auto feature = features.features[i];
    spdlog::info("feature: {}", feature);
  }
}
void OutputLimits(const wgpu::Limits &limits){
  spdlog::info("maxTextureDimension1D: {}", limits.maxTextureDimension1D);
  spdlog::info("maxTextureDimension2D: {}", limits.maxTextureDimension2D);
  spdlog::info("maxTextureDimension3D: {}", limits.maxTextureDimension3D);
  spdlog::info("maxTextureArrayLayers: {}", limits.maxTextureArrayLayers);
}
void InspectDevice(wgpu::Device device){
  //Features
  spdlog::info("Device features: ");
  wgpu::SupportedFeatures features;
  device.GetFeatures(&features);
  OutputFeatures(features);
  //Limits
  spdlog::info("Device limits: ");
  wgpu::Limits limits = {};
  limits.nextInChain = nullptr;
  bool success = device.GetLimits(&limits) == wgpu::Status::Success;
  if (success) {
    OutputLimits(limits);
  }
}
void InspectAdapter(const wgpu::Adapter &adapter){
  //Limits
  spdlog::info("Adapter limits:");
  wgpu::Limits limits = {};
  limits.nextInChain = nullptr;
  bool success = adapter.GetLimits(&limits) == wgpu::Status::Success;
  if (success) {
    OutputLimits(limits);
  }
  // Features
  spdlog::info("Adapter features:");
  wgpu::SupportedFeatures features;
  adapter.GetFeatures(&features);
  OutputFeatures(features);

  // Properties
  spdlog::info("Adapter properties:");
  wgpu::AdapterInfo info = {};
  adapter.GetInfo(&info);

  spdlog::info("vendorID: {}", info.vendorID);
  spdlog::info("vendor: {}", info.vendor);
  spdlog::info("architecture: {}", info.architecture);
  spdlog::info("deviceID: {}", info.deviceID);
  spdlog::info("device: {}", info.device);
  spdlog::info("description: {}", info.description);
  spdlog::info("subgroup minimum size: {}", info.subgroupMinSize);
  spdlog::info("subgroup maximum size: {}", info.subgroupMaxSize);
  spdlog::info("adapter type: {}", info.adapterType);
  spdlog::info("backend type: {}", info.backendType);
}
void ConfigureSurface([[maybe_unused]]GPUContext &ctx){
  //wgpu::RequestAdapterOptions adapter_options = {.nextInChain = nullptr, .compatibleSurface = app.gpu.surface};

  wgpu::SurfaceCapabilities capabilities = {};
  ctx.surface.GetCapabilities(ctx.adapter, &capabilities);
  //wgpu::TextureFormat surface_format = capabilities.formatCount > 0 ? capabilities.formats[0] : wgpu::TextureFormat::BGRA8Unorm;
  //spdlog::info("Selected texture format: {}", surface_format);
  

  // wgpu::SurfaceConfiguration configuration = {.nextInChain = nullptr};
  // surface_configuration.width = 640;
  // surface_configuration.height = 480;

}
void Initialize([[maybe_unused]]App &app){
  spdlog::info("Using Emscripten: {}", UsingEmscripten());
  wgpu::InstanceDescriptor desc = {
    .nextInChain = nullptr
  };
  app.gpu.instance = wgpu::CreateInstance(&desc);
  if (!app.gpu.instance){
    spdlog::critical("Could not initalise WebGPU!");
    assert(false);
  }
  else spdlog::info("WGPUInstance created");
  if (!glfwInit()){
    spdlog::critical("Failed to initalise GLFW!");
    assert(false);
  }
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  app.window = glfwCreateWindow(800, 1000, "WebGPU Window", NULL, NULL);
  if (!app.window){
    spdlog::critical("Failed to open Window");
    assert(false);
  }
  app.gpu.surface = wgpu::Surface::Acquire(glfwCreateWindowWGPUSurface(app.gpu.instance.Get(), app.window));
  if (!app.gpu.surface){
    spdlog::critical("Failed to create a surface for the window");
    assert(false);
  }
  ConfigureSurface(app.gpu);
}
void InitializeCallbacks(App &app){
  spdlog::info("Initalization stage: {}", app.initialized_state);
  if (app.initialized_state == InitializationState::Uninitalised) {
    wgpu::RequestAdapterOptions adapter_options = {
      .nextInChain = nullptr
    };
    StartAdapterRequest(app, &adapter_options);
    app.initialized_state = InitializationState::RequestingAdapter;
  }
  else if (app.initialized_state == InitializationState::RequestingDevice){
    InspectAdapter(app.gpu.adapter);
    wgpu::DeviceDescriptor device_descriptor = {};
    device_descriptor.nextInChain = nullptr;
    device_descriptor.label = WGPUStringView{.data = "TestDevice",.length = WGPU_STRLEN};
    device_descriptor.requiredFeatureCount = 0;
    device_descriptor.requiredLimits = nullptr;
    device_descriptor.defaultQueue.nextInChain = nullptr;
    device_descriptor.defaultQueue.label = WGPUStringView{.data = "DefaultQueue",.length = WGPU_STRLEN};
    device_descriptor.SetDeviceLostCallback(wgpu::CallbackMode::AllowSpontaneous, []([[maybe_unused]]wgpu::Device const& device, wgpu::DeviceLostReason reason, wgpu::StringView message){
      spdlog::warn("Device lost. reason: {}", reason);
      if (message.data && message.length > 0) spdlog::info("({})", message);
    });
    device_descriptor.SetUncapturedErrorCallback([]([[maybe_unused]]wgpu::Device const& device, wgpu::ErrorType error, wgpu::StringView message){
      spdlog::error("Device uncaptured error: {}", error);
      if (message.data && message.length > 0) spdlog::info("({})", message);
    });
    StartDeviceRequest(app,&device_descriptor);
  }
  else if (app.initialized_state == InitializationState::ReceivedAdapterAndDevice){
    InspectDevice(app.gpu.device);
    app.gpu.queue = app.gpu.device.GetQueue();
    app.initialized_state = InitializationState::Ready;
  }
}
void Shutdown([[maybe_unused]]App &app){
  glfwDestroyWindow(app.window);
  //wgpuSurfaceUnconfigure(app.gpu.surface);
  glfwTerminate();
  spdlog::info("Shutting down.");
}
void Update([[maybe_unused]]App &app){
  #ifndef __EMSCRIPTEN__ 
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    app.gpu.instance.ProcessEvents();
  #endif
  if (app.initialized_state != InitializationState::Ready) InitializeCallbacks(app);
  else spdlog::info("Time since program opened: {}", static_cast<long>(GetTotalProgramTimeElapsedMilliseconds(app)));
  glfwPollEvents();
  if (glfwWindowShouldClose(app.window)) app.running = false;

  if (app.initialized_state != InitializationState::Ready) return;
  if (GetTotalProgramTimeElapsedMilliseconds(app) >= 1500){
    app.running = false;
  }
}
#ifdef __EMSCRIPTEN__
void EmscriptenLoop(void* arg) {
  App* app = static_cast<App*>(arg);
  Update(*app);
  if (!app->running) {
      emscripten_cancel_main_loop();
      Shutdown(*app);
  }
}
#endif

int main([[maybe_unused]] int argc, [[maybe_unused]] char*argv[]){
  App app = {.window = {}, .gpu = {}, .logger = {}, .start_time = {std::chrono::high_resolution_clock::now()}};
  InitaliseLogging(app);
  Initialize(app);
  #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(EmscriptenLoop, &app, 0, true);
  #else
    while (app.running){
      Update(app);
    }
  Shutdown(app);
  #endif
}
// Matrices in Eigen are row-major, where GLM are column-major, so must
// transpose between them!
// Always pass eigen by const &
// Use smart pointers
// Use ECS
// use spdlog (fast C++ logging)
// Dear ImGUI for any UI creation.
// stb_image - load images from memory.
// Tracy profiler
// nlohmann/json for serialisation using json.
// native file dialog to open file // filewatcher to watch my files?



  // wgpu::CommandEncoderDescriptor encoder_descriptor = {.nextInChain = nullptr, .label = "My command encoder"};
  // wgpu::CommandEncoder encoder = device.CreateCommandEncoder(&encoder_descriptor);
  // encoder.InsertDebugMarker("do one thing");
  // wgpu::CommandBufferDescriptor command_buffer_descriptor = {.nextInChain = nullptr, .label = "My command buffer"};
  // wgpu::CommandBuffer command = encoder.Finish(&command_buffer_descriptor);
  // std::print("Submitting command..\n");
  // queue.Submit(1, &command);
  // std::print("Command submitted\n");
  //   queue.OnSubmittedWorkDone(wgpu::CallbackMode::AllowSpontaneous,[](wgpu::QueueWorkDoneStatus status, wgpu::StringView message){
  //   std::print("queue work finished. Status: {} ; Message: {}\n", status, message);
  // });