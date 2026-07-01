#include <iostream>
#include <cassert>
#include <memory>
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
  wgpu::TextureFormat surface_format = wgpu::TextureFormat::Undefined;
  wgpu::ShaderModule shader_module = {};
  wgpu::RenderPipeline pipeline = {};
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
    spdlog::trace("feature: {}", feature);
  }
}
void OutputLimits(const wgpu::Limits &limits){
  spdlog::trace("maxTextureDimension1D: {}", limits.maxTextureDimension1D);
  spdlog::trace("maxTextureDimension2D: {}", limits.maxTextureDimension2D);
  spdlog::trace("maxTextureDimension3D: {}", limits.maxTextureDimension3D);
  spdlog::trace("maxTextureArrayLayers: {}", limits.maxTextureArrayLayers);
}
void InspectDevice(wgpu::Device device){
  //Features
  spdlog::trace("Device features: ");
  wgpu::SupportedFeatures features;
  device.GetFeatures(&features);
  OutputFeatures(features);
  //Limits
  spdlog::trace("Device limits: ");
  wgpu::Limits limits = {};
  bool success = device.GetLimits(&limits) == wgpu::Status::Success;
  if (success) {
    OutputLimits(limits);
  }
}
void InspectAdapter(const wgpu::Adapter &adapter){
  //Limits
  spdlog::trace("Adapter limits:");
  wgpu::Limits limits = {};
  bool success = adapter.GetLimits(&limits) == wgpu::Status::Success;
  if (success) {
    OutputLimits(limits);
  }
  // Features
  spdlog::trace("Adapter features:");
  wgpu::SupportedFeatures features;
  adapter.GetFeatures(&features);
  OutputFeatures(features);

  // Properties
  spdlog::trace("Adapter properties:");
  wgpu::AdapterInfo info = {};
  adapter.GetInfo(&info);

  spdlog::trace("vendorID: {}", info.vendorID);
  spdlog::trace("vendor: {}", info.vendor);
  spdlog::trace("architecture: {}", info.architecture);
  spdlog::trace("deviceID: {}", info.deviceID);
  spdlog::trace("device: {}", info.device);
  spdlog::trace("description: {}", info.description);
  spdlog::trace("subgroup minimum size: {}", info.subgroupMinSize);
  spdlog::trace("subgroup maximum size: {}", info.subgroupMaxSize);
  spdlog::trace("adapter type: {}", info.adapterType);
  spdlog::trace("backend type: {}", info.backendType);
}
void ConfigureSurface([[maybe_unused]]GPUContext &ctx){
  //wgpu::RequestAdapterOptions adapter_options = {.nextInChain = nullptr, .compatibleSurface = app.gpu.surface};
  spdlog::debug("Configuring Surface");
  wgpu::SurfaceConfiguration configuration = {};
  configuration.width = 640;
  configuration.height = 400;

  wgpu::SurfaceCapabilities capabilities = {};
  
  ctx.surface.GetCapabilities(ctx.adapter, &capabilities);
  ctx.surface_format = capabilities.formatCount > 0 ? capabilities.formats[0] : wgpu::TextureFormat::BGRA8Unorm;
  configuration.format = ctx.surface_format;
  configuration.viewFormatCount = 0;
  configuration.viewFormats = nullptr;
  configuration.usage = wgpu::TextureUsage::RenderAttachment;
  configuration.device = ctx.device;
  configuration.presentMode = wgpu::PresentMode::Fifo;
  configuration.alphaMode = wgpu::CompositeAlphaMode::Auto;
  
  ctx.surface.Configure(&configuration);
  spdlog::debug("Configured surface");
}
void Initialize([[maybe_unused]]App &app){
  spdlog::info("Using Emscripten: {}", UsingEmscripten());
  wgpu::InstanceDescriptor desc = {};
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
}

void CreateShaderModule(GPUContext &ctx);
void CreateRenderPipeline(GPUContext &ctx);
void InitializeCallbacks(App &app){
  spdlog::info("Initalization stage: {}", app.initialized_state);
  if (app.initialized_state == InitializationState::Uninitalised) {
    wgpu::RequestAdapterOptions adapter_options = {};
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
    ConfigureSurface(app.gpu);
    CreateShaderModule(app.gpu);
    CreateRenderPipeline(app.gpu);
    app.initialized_state = InitializationState::Ready;
  }
}
void Shutdown([[maybe_unused]]App &app){
  glfwDestroyWindow(app.window);
  //wgpuSurfaceUnconfigure(app.gpu.surface);
  glfwTerminate();
  spdlog::info("Shutting down.");
}
std::pair<wgpu::SurfaceTexture, wgpu::TextureView> GetNextSurfaceViewData(GPUContext &ctx){
  wgpu::SurfaceTexture surface_texture = {};
  ctx.surface.GetCurrentTexture(&surface_texture);
  if (surface_texture.status != wgpu::SurfaceGetCurrentTextureStatus::SuccessOptimal){
    spdlog::critical("Couldnt't get current texture for the surface");
    assert(false);
    return {surface_texture, nullptr};
  }

  wgpu::TextureViewDescriptor view_descriptor = {
    .nextInChain = nullptr,
    .label = "Surface texture view",
    .format = ctx.surface_format,
    .dimension = wgpu::TextureViewDimension::e2D,
    .baseMipLevel = 0,
    .mipLevelCount = 1,
    .baseArrayLayer = 0,
    .arrayLayerCount = 1,
    .aspect = wgpu::TextureAspect::All
  };
  wgpu::TextureView target_view = surface_texture.texture.CreateView(&view_descriptor);

  return {surface_texture, target_view};
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
  if (GetTotalProgramTimeElapsedMilliseconds(app) >= 5000){
    app.running = false;
  }
  if (app.initialized_state != InitializationState::Ready) return;
  if (GetTotalProgramTimeElapsedMilliseconds(app) >= 15000000){
    app.running = false;
  }

  auto [surface_view, target_view] = GetNextSurfaceViewData(app.gpu);

  wgpu::CommandEncoderDescriptor encoder_descriptor = {.nextInChain = nullptr, .label = "My command encoder"};
  wgpu::CommandEncoder encoder = app.gpu.device.CreateCommandEncoder(&encoder_descriptor);
  wgpu::RenderPassDescriptor render_pass_descriptor = {
    .colorAttachmentCount = 1,
    .depthStencilAttachment = nullptr,
    .timestampWrites = nullptr
  };
  wgpu::RenderPassColorAttachment color_attachment = {
    .view = target_view,
    .depthSlice = wgpu::kDepthSliceUndefined,
    .resolveTarget = nullptr,
    .loadOp = wgpu::LoadOp::Clear,
    .storeOp = wgpu::StoreOp::Store,
    .clearValue = wgpu::Color{0.3, 1, 0.2, 1.0}
  }; // resolve target for multi-sampling
  render_pass_descriptor.colorAttachments = &color_attachment;
  wgpu::RenderPassEncoder render_pass = encoder.BeginRenderPass(&render_pass_descriptor);


  render_pass.SetPipeline(app.gpu.pipeline);
  render_pass.Draw(3, 1, 0, 0);

  render_pass.End();
  wgpu::CommandBufferDescriptor command_buffer_descriptor = {.nextInChain = nullptr, .label = "My command buffer"};
  wgpu::CommandBuffer command = encoder.Finish(&command_buffer_descriptor);
  spdlog::info("Submitting command..");
  app.gpu.queue.Submit(1, &command);
  spdlog::info("Command submitted");
    app.gpu.queue.OnSubmittedWorkDone(wgpu::CallbackMode::AllowSpontaneous,[](wgpu::QueueWorkDoneStatus status, wgpu::StringView message){
    spdlog::info("queue work finished. Status: {} ; Message: {}", status, message);
  });
  #ifndef __EMSCRIPTEN__
    app.gpu.surface.Present();
  #endif
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

  const char *shader_source = R"(
    @vertex
    fn vs_main(@builtin(vertex_index) in_vertex_index: u32) -> @builtin(position) vec4f {
        var p = vec2f(0.0, 0.0);
        if (in_vertex_index == 0u) {
            p = vec2f(-0.5, -0.5);
        } else if (in_vertex_index == 1u) {
            p = vec2f(0.5, -0.5);
        } else {
            p = vec2f(0.0, 0.5);
        }
        return vec4f(p, 0.0, 1.0);
    }

    @fragment
    fn fs_main() -> @location(0) vec4f {
        return vec4f(0.0, 0.4, 1.0, 1.0);
    }
  )";


  void CreateShaderModule(GPUContext &ctx){
    wgpu::ShaderModuleDescriptor shader_descriptor = {};

    wgpu::ShaderSourceWGSL shader_code_descriptor = {};
    shader_code_descriptor.nextInChain = nullptr;
    shader_code_descriptor.sType = wgpu::SType::ShaderSourceWGSL;
    shader_descriptor.nextInChain = &shader_code_descriptor;
    shader_code_descriptor.code = shader_source;

    ctx.shader_module = ctx.device.CreateShaderModule(&shader_descriptor);
  }

  void CreateRenderPipeline(GPUContext &ctx){
    wgpu::RenderPipelineDescriptor pipeline_descriptor = {};


    //Vertex Pipeline State
    pipeline_descriptor.vertex.bufferCount = 0;
    pipeline_descriptor.vertex.buffers = nullptr;
    pipeline_descriptor.vertex.module = ctx.shader_module;
    pipeline_descriptor.vertex.constantCount = 0;
    pipeline_descriptor.vertex.constants = nullptr;


    //Primitive Pipeline state
    pipeline_descriptor.primitive.topology = wgpu::PrimitiveTopology::TriangleList;
    pipeline_descriptor.primitive.stripIndexFormat = wgpu::IndexFormat::Undefined;
    pipeline_descriptor.primitive.frontFace = wgpu::FrontFace::CCW;
    pipeline_descriptor.primitive.cullMode = wgpu::CullMode::None; // Set to none for developing so stuff doesn't disappear

    //Fragment Shader Stage
    wgpu::FragmentState fragment_state = {};
    fragment_state.module = ctx.shader_module;
    fragment_state.entryPoint = "fs_main";
    fragment_state.constantCount = 0;
    fragment_state.constants = nullptr;

    wgpu::BlendState blend_state = {};
    blend_state.color.srcFactor = wgpu::BlendFactor::SrcAlpha;
    blend_state.color.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
    blend_state.color.operation = wgpu::BlendOperation::Add;
    blend_state.alpha.srcFactor = wgpu::BlendFactor::Zero;
    blend_state.alpha.dstFactor = wgpu::BlendFactor::One;
    blend_state.alpha.operation = wgpu::BlendOperation::Add;

    wgpu::ColorTargetState color_target = {};
    color_target.format = ctx.surface_format;
    color_target.blend = &blend_state;
    color_target.writeMask = wgpu::ColorWriteMask::All;
    fragment_state.targetCount = 1;
    fragment_state.targets = &color_target;

    pipeline_descriptor.fragment = &fragment_state;
    pipeline_descriptor.depthStencil = nullptr; // used for depth buffers

    //Multisampling Pipeline state
    pipeline_descriptor.multisample.count = 1;
    pipeline_descriptor.multisample.mask = ~0u;
    pipeline_descriptor.multisample.alphaToCoverageEnabled = false;

    ctx.pipeline = ctx.device.CreateRenderPipeline(&pipeline_descriptor);
  }

// void PlayingWithBuffers(){

// }

  // Learning about the GPU
  // a buffer is a chunk of memory allocated in VRAM.