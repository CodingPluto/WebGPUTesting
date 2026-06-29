#include <iostream>
#include <cassert>
#include <print>
#include <chrono>


#include <thread>
#include <webgpu/webgpu_cpp.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <webgpu/webgpu.h>
#else
#include <GLFW/glfw3.h>
#endif

#include "glfw3webgpu.h"
#include "formatted_webgpu.h"


struct GPUContext{
  wgpu::Instance instance = {};
  wgpu::Adapter adapter = {};
  wgpu::Device device = {};
  wgpu::Queue queue = {};
};
struct App{
  GLFWwindow* window;
  GPUContext gpu;
  bool running = true;
  InitializationState initalised_state = InitializationState::Uninitalised;
};
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
wgpu::Adapter RequestAdapterSync(wgpu::Instance instance, const wgpu::RequestAdapterOptions *options){
  wgpu::Adapter adapter = nullptr;
  bool request_ended = false;
  [[maybe_unused]]wgpu::Future future = instance.RequestAdapter( // Async function!
      options, 
      wgpu::CallbackMode::AllowSpontaneous, 
      [&](wgpu::RequestAdapterStatus status, wgpu::Adapter result_adapter, [[maybe_unused]]wgpu::StringView message) {
          if (status == wgpu::RequestAdapterStatus::Success) {
              adapter = result_adapter;
              request_ended = true;
          }
          else{
            std::print("Failed to get an adapter for reason {}\n", status);
            std::print("--    Error message: {}\n", message);
          }
      }
  );
  instance.WaitAny(future, 0);
  assert(request_ended);
  return adapter;
}
wgpu::Device RequestDeviceSync([[maybe_unused]]wgpu::Instance instance, wgpu::Adapter adapter, const wgpu::DeviceDescriptor *descriptor){
  wgpu::Device device = nullptr;
  bool request_ended = false;
  [[maybe_unused]]wgpu::Future future = adapter.RequestDevice( // Async function!
      descriptor, 
      wgpu::CallbackMode::AllowSpontaneous, 
      [&](wgpu::RequestDeviceStatus status, wgpu::Device result_device, [[maybe_unused]]wgpu::StringView message) {
          if (status == wgpu::RequestDeviceStatus::Success) {
              device = result_device;
          }
          request_ended = true;
      }
  );
  instance.WaitAny(future, 0);
  assert(request_ended);
  return device;
}
void StartAdapterRequest(wgpu::Instance instance, const wgpu::RequestAdapterOptions *options, App &app) {
  instance.RequestAdapter(
    options,AdapterCallbackMode(), [&](wgpu::RequestAdapterStatus status, wgpu::Adapter result_adapter, wgpu::StringView message) {
      if (status == wgpu::RequestAdapterStatus::Success) {
          app.gpu.adapter = result_adapter;
          // Safely advance the state machine inside your frame loop!
          app.initalised_state = InitializationState::RequestingDevice; 
      } else {
          std::print("Failed to get an adapter: {}\n", message);
          app.initalised_state = InitializationState::Failed;
      }
    }
  );
}
void StartDeviceRequest(App &app, const wgpu::DeviceDescriptor *descriptor){
  app.gpu.adapter.RequestDevice(
    descriptor, AdapterCallbackMode(), [&](wgpu::RequestDeviceStatus status, wgpu::Device result_device, [[maybe_unused]]wgpu::StringView message) {
      if (status == wgpu::RequestDeviceStatus::Success) {
        app.gpu.device = result_device;
        app.initalised_state = InitializationState::Ready;
      } else {
        std::print("Failed to get a device: {}\n", message);
        app.initalised_state = InitializationState::Failed;
      }
    }
  );
}
void OutputFeatures(const wgpu::SupportedFeatures &features){
  for (size_t i = 0; i < features.featureCount; ++i){
    auto feature = features.features[i];
    std::print("feature: {}\n", feature);
  }
}
void OutputLimits(const wgpu::Limits &limits){
  std::print("maxTextureDimension1D: {}\n", limits.maxTextureDimension1D);
  std::print("maxTextureDimension2D: {}\n", limits.maxTextureDimension2D);
  std::print("maxTextureDimension3D: {}\n", limits.maxTextureDimension3D);
  std::print("maxTextureArrayLayers: {}\n", limits.maxTextureArrayLayers);
}
void InspectDevice(wgpu::Device device){
  //Featurea
  std::print("Device features: \n");
  wgpu::SupportedFeatures features;
  device.GetFeatures(&features);
  OutputFeatures(features);
  //Limits
  std::print("Device limits:\n");
  wgpu::Limits limits = {};
  limits.nextInChain = nullptr;
  bool success = device.GetLimits(&limits) == wgpu::Status::Success;
  if (success) {
    OutputLimits(limits);
  }
}
void InspectAdapter(const wgpu::Adapter &adapter){
  //Limits
  std::print("Adapter limits:\n");
  wgpu::Limits limits = {};
  limits.nextInChain = nullptr;
  std::cout << "got to here" << std::endl;
  bool success = adapter.GetLimits(&limits) == wgpu::Status::Success;
  std::cout << "Got adapter limits: " << success << std::endl;
  if (success) {
    OutputLimits(limits);
  }
  //Features
  std::print("Adapter features:\n");
  wgpu::SupportedFeatures features;
  adapter.GetFeatures(&features);
  OutputFeatures(features);
  //Properties
  std::print("Adapter properties:\n");
  wgpu::AdapterInfo info = {};
  adapter.GetInfo(&info);
  std::print("vendorID: {}\n", info.vendorID);
  std::print("vendor: {}\n", info.vendor);
  std::print("architecture: {}\n", info.architecture);
  std::print("deviceID: {}\n", info.deviceID);
  std::print("device: {}\n", info.device);
  std::print("description: {}\n", info.description);
  std::print("subgroup minimum size: {}\n", info.subgroupMinSize);
  std::print("subgroup maximum size: {}\n", info.subgroupMaxSize);
  std::print("adapter type: {}\n", info.adapterType);
  std::print("backend type: {}\n", info.backendType);
}


void InitaliseAdapterAndDeviceNative(GPUContext &gpu){
  wgpu::RequestAdapterOptions adapter_options = {
    .nextInChain = nullptr
  };
  gpu.adapter = RequestAdapterSync(gpu.instance, &adapter_options);
  if (gpu.adapter) std::print("Adapter created successfully\n");
  else std::print("Failed to initalise adapter successfully!\n");
  InspectAdapter(gpu.adapter);
  
  wgpu::DeviceDescriptor device_descriptor = {};
  device_descriptor.nextInChain = nullptr;
  device_descriptor.label = WGPUStringView{.data = "TestDevice",.length = WGPU_STRLEN};
  device_descriptor.requiredFeatureCount = 0;
  device_descriptor.requiredLimits = nullptr;
  device_descriptor.defaultQueue.nextInChain = nullptr;
  device_descriptor.defaultQueue.label = WGPUStringView{.data = "DefaultQueue",.length = WGPU_STRLEN};
  device_descriptor.SetDeviceLostCallback(wgpu::CallbackMode::AllowSpontaneous, []([[maybe_unused]]wgpu::Device const& device, wgpu::DeviceLostReason reason, wgpu::StringView message){
    std::print("Device lost. reason: {}", reason);
    if (message.data) std::print("({})", message);
    std::cout << std::endl;
  }); 
  device_descriptor.SetUncapturedErrorCallback([]([[maybe_unused]]wgpu::Device const& device, wgpu::ErrorType error, wgpu::StringView message){
    std::print("Device uncaptured error: {}", error);
    if (message.data) std::print("({})", message);
    std::cout << std::endl;
  });
  gpu.device = RequestDeviceSync(gpu.instance, gpu.adapter, &device_descriptor);
  if (gpu.device) std::print("Device created successfully\n");
  InspectDevice(gpu.device);
  gpu.queue = gpu.device.GetQueue();
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

}

void Initalise([[maybe_unused]]App &app){
  std::cout << "Using Emscripten: " << std::boolalpha <<  UsingEmscripten() << std::noboolalpha << std::endl;
  wgpu::InstanceDescriptor desc = {
    .nextInChain = nullptr
  };
  app.gpu.instance = wgpu::CreateInstance(&desc);
  if (!app.gpu.instance){
    std::cerr << "Could not initalise WebGPU!" << std::endl;
    assert(false);
  }
  else std::print("WGPUInstance created\n");

  // #ifndef __EMSCRIPTEN__
  //   InitaliseAdapterAndDeviceNative(app.gpu);
  //   app.initalised_state = InitializationState::Ready;
  // #endif

  if (!glfwInit()){
    std::cerr << "Failed to initalise GLFW!" << std::endl;
    assert(false);
  }
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  app.window = glfwCreateWindow(800, 1000, "WebGPU Window", NULL, NULL);
  if (!app.window){
    std::cerr << "Failed to open Window" << std::endl;
    assert(false);
  }

  WGPUSurface rawSurface = glfwCreateWindowWGPUSurface(app.gpu.instance.Get(), app.window);
  wgpu::Surface surface = wgpu::Surface::Acquire(rawSurface);
}

void NewInitalise(App &app){
  std::print("Initalization stage: {}\n", app.initalised_state);
  if (app.initalised_state == InitializationState::Uninitalised) {
    wgpu::RequestAdapterOptions adapter_options = {
      .nextInChain = nullptr
    };
    StartAdapterRequest(app.gpu.instance, &adapter_options, app);
    app.initalised_state = InitializationState::RequestingAdapter;
  }
  else if (app.initalised_state == InitializationState::RequestingDevice){
    wgpu::DeviceDescriptor device_descriptor = {};
    device_descriptor.nextInChain = nullptr;
    device_descriptor.label = WGPUStringView{.data = "TestDevice",.length = WGPU_STRLEN};
    device_descriptor.requiredFeatureCount = 0;
    device_descriptor.requiredLimits = nullptr;
    device_descriptor.defaultQueue.nextInChain = nullptr;
    device_descriptor.defaultQueue.label = WGPUStringView{.data = "DefaultQueue",.length = WGPU_STRLEN};
    device_descriptor.SetDeviceLostCallback(wgpu::CallbackMode::AllowSpontaneous, []([[maybe_unused]]wgpu::Device const& device, wgpu::DeviceLostReason reason, wgpu::StringView message){
      std::print("Device lost. reason: {}", reason);
      if (message.data) std::print("({})", message);
      std::cout << std::endl;
    });
    device_descriptor.SetUncapturedErrorCallback([]([[maybe_unused]]wgpu::Device const& device, wgpu::ErrorType error, wgpu::StringView message){
    std::print("Device uncaptured error: {}", error);
    if (message.data) std::print("({})", message);
    std::cout << std::endl;
    });
    StartDeviceRequest(app,&device_descriptor);
  }
  app.gpu.instance.ProcessEvents();
  #ifndef __EMSCRIPTEN__
    app.gpu.instance.ProcessEvents();
  #endif
}

void Update([[maybe_unused]]App &app){
  #ifndef __EMSCRIPTEN__ 
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
  #endif
  if (app.initalised_state != InitializationState::Ready) NewInitalise(app);
  else std::print("Into main update\n");




  glfwPollEvents();
  if (glfwWindowShouldClose(app.window)) app.running = false;
}
void Shutdown([[maybe_unused]]App &app){
  glfwDestroyWindow(app.window);
  glfwTerminate();
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
  App app = {.window = {}, .gpu = {}};
  Initalise(app);

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
