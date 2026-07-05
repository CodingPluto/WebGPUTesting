#include <cstdint>
#include <glm/fwd.hpp>
#include <iostream>
#include <cassert>
#include <iterator>
#include <memory>
#include <chrono>
#include <spdlog/logger.h>
#include <string>
#include <thread>
#include <fstream>
#include <filesystem>

#include <webgpu/webgpu.h>
#include <webgpu/webgpu_cpp.h>
#include <glm/glm.hpp>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#include <GLFW/glfw3.h>
#endif
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "glfw3webgpu.h"
#include "formatted_webgpu.h"

const float kDefaultDeltaTime = 0.016;

struct Uniforms {
  glm::f32 time; // at byte offset 0
  glm::f32 delta_time; // at byte offset 4
  std::array<glm::f32, 2> padding = {};
};
enum VertexAttributeType{
  kPosition = 0,
  kColor = 1
};
enum BindingType{
  kUniforms = 0,
  kComputeInput = 1,
  kComputeOutput = 2
};

struct GPUContext{
  wgpu::Instance instance = {};
  wgpu::Adapter adapter = {};
  wgpu::Device device = {};
  wgpu::Queue queue = {};
  wgpu::Surface surface = {};
  wgpu::TextureFormat surface_format = wgpu::TextureFormat::Undefined;
  wgpu::ShaderModule shader_module = {};
  wgpu::RenderPipeline pipeline = {};
  std::string shader_source = {};
  std::filesystem::file_time_type shader_last_edited = {};
  wgpu::Buffer point_buffer = {};
  wgpu::Buffer index_buffer = {};
  wgpu::Buffer uniform_buffer = {};
  uint32_t index_count = 0;
  wgpu::PipelineLayout pipeline_layout = {};
  wgpu::BindGroupLayout bind_group_layout = {};
  wgpu::BindGroup bind_group = {};
  wgpu::Buffer buffer_2 = {};
  wgpu::ComputePipeline compute_pipeline = {};

  //wgpu::ShaderModule computer_shader_module = {};
};
struct App{
  GLFWwindow* window;
  GPUContext gpu = {};
  bool running = true;
  InitializationState initialized_state = InitializationState::Uninitalised;
  std::shared_ptr<spdlog::logger> logger = {};
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time = {};
  std::chrono::time_point<std::chrono::high_resolution_clock> last_frame_time = {};
  double total_time_elapsed = 0;
  float delta_time = kDefaultDeltaTime;
};

bool LoadGeometry(
    const std::filesystem::path& path,
    std::vector<float>& pointData,
    std::vector<uint16_t>& indexData
) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }

    pointData.clear();
    indexData.clear();

    enum class Section {
        None,
        Points,
        Indices,
    };
    Section currentSection = Section::None;

    float value;
    uint16_t index;
    std::string line;
    while (!file.eof()) {
        getline(file, line);
        
        // overcome the `CRLF` problem
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        
        if (line == "[points]") {
            currentSection = Section::Points;
        }
        else if (line == "[indices]") {
            currentSection = Section::Indices;
        }
        else if (line[0] == '#' || line.empty()) {
            // Do nothing, this is a comment
        }
        else if (currentSection == Section::Points) {
            std::istringstream iss(line);
            // Get x, y, r, g, b
            for (int i = 0; i < 5; ++i) {
                iss >> value;
                pointData.push_back(value);
            }
        }
        else if (currentSection == Section::Indices) {
            std::istringstream iss(line);
            // Get corners #0 #1 and #2
            for (int i = 0; i < 3; ++i) {
                iss >> index;
                indexData.push_back(index);
            }
        }
    }
    return true;
}
void InitaliseLogging(App &app){
  app.logger = spdlog::stdout_color_mt("app");
  spdlog::set_default_logger(app.logger);
  spdlog::set_level(spdlog::level::trace);
  spdlog::flush_on(spdlog::level::info);
}
[[nodiscard]] consteval wgpu::CallbackMode GetPlatformCallbackMode(){
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
    options,GetPlatformCallbackMode(), [&app](wgpu::RequestAdapterStatus status, wgpu::Adapter result_adapter, wgpu::StringView message) {
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
    descriptor, GetPlatformCallbackMode(), [&app](wgpu::RequestDeviceStatus status, wgpu::Device result_device, [[maybe_unused]]wgpu::StringView message) {
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
  spdlog::trace("maxVertexAttributes: {}", limits.maxVertexAttributes);
  spdlog::trace("maxVertexBuffers: {}", limits.maxVertexBuffers);
  spdlog::trace("maxVertexBuffersArrayStride: {}", limits.maxVertexBufferArrayStride);
  spdlog::trace("maxBindGroups: {}", limits.maxBindGroups);
  spdlog::trace("maxUniformBuffersPerShaderStage: {}", limits.maxUniformBuffersPerShaderStage);
  spdlog::trace("maxUniformBufferBindingSize: {}", limits.maxUniformBufferBindingSize);
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
  app.window = glfwCreateWindow(500, 500, "WebGPU Window", NULL, NULL);
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
void CreateShaderModules(GPUContext &ctx);
void CreateRenderPipeline(GPUContext &ctx);
void CreateComputePipeline(GPUContext &ctx);
void PlayingWithBuffers(GPUContext &ctx);
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
    wgpu::Limits limits = {};
    limits.maxVertexAttributes = 8;
    limits.maxVertexBuffers = 4;
    limits.maxBufferSize = 6 * 10 * sizeof(float);
    limits.maxVertexBufferArrayStride = 10 * sizeof(float);
    limits.maxInterStageShaderVariables = 3;
    limits.maxBindGroups = 1;
    limits.maxUniformBuffersPerShaderStage = 1;
    limits.maxUniformBufferBindingSize = 16 * 4;
    device_descriptor.requiredLimits = &limits;

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
    CreateShaderModules(app.gpu);
    CreateRenderPipeline(app.gpu);
    CreateComputePipeline(app.gpu);
    PlayingWithBuffers(app.gpu);
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
void HotReloadShaders(GPUContext &ctx){
  CreateShaderModules(ctx);
  CreateRenderPipeline(ctx);
  CreateComputePipeline(ctx);
}
std::filesystem::file_time_type GetFileLastEdited(const std::string &path){
  std::error_code ec = {};
  auto current = std::filesystem::last_write_time(path,ec);
  return current;
}
void Update([[maybe_unused]]App &app){
  #ifndef __EMSCRIPTEN__
    //std::this_thread::sleep_for(std::chrono::milliseconds(16));
    app.gpu.instance.ProcessEvents();
  #endif
  auto current_time = std::chrono::high_resolution_clock::now();
  double delta_time = std::chrono::duration<float>(current_time - app.last_frame_time).count();
  app.last_frame_time = current_time;
  app.total_time_elapsed += delta_time;
  spdlog::trace("DeltaTime: {}", delta_time);
  if (app.initialized_state != InitializationState::Ready) InitializeCallbacks(app);
  else spdlog::info("Time since program opened: {}", static_cast<double>(app.total_time_elapsed));
  glfwPollEvents();
  if (glfwWindowShouldClose(app.window)) app.running = false;
  if (app.initialized_state != InitializationState::Ready) return;
  if (app.gpu.shader_last_edited != GetFileLastEdited("assets/shader.wgsl")){
    HotReloadShaders(app.gpu);
  }
  if (UsingEmscripten() && app.total_time_elapsed >= 1500){
    app.running = false;
  }
  Uniforms uniforms = {
    .time = static_cast<float>(app.total_time_elapsed),
    .delta_time = (glm::f32)delta_time
  };
  app.gpu.queue.WriteBuffer(app.gpu.uniform_buffer, 0, &uniforms, sizeof(kUniforms));

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
    .clearValue = wgpu::Color{0.05, 0.05, 0.05, 1.0}
  }; // resolve target for multi-sampling
  render_pass_descriptor.colorAttachments = &color_attachment;
  wgpu::RenderPassEncoder render_pass = encoder.BeginRenderPass(&render_pass_descriptor);

  render_pass.SetPipeline(app.gpu.pipeline);
  render_pass.SetVertexBuffer(0, app.gpu.point_buffer, 0, app.gpu.point_buffer.GetSize());
  render_pass.SetIndexBuffer(app.gpu.index_buffer, wgpu::IndexFormat::Uint16, 0, app.gpu.index_buffer.GetSize());
  render_pass.SetBindGroup(0, app.gpu.bind_group, 0, nullptr);
  render_pass.DrawIndexed(app.gpu.index_count, 1, 0,0, 0);
  render_pass.End();

  wgpu::ComputePassDescriptor compute_pass_descriptor = {.timestampWrites = nullptr};
  wgpu::ComputePassEncoder compute_pass = encoder.BeginComputePass(&compute_pass_descriptor);
  compute_pass.SetPipeline(app.gpu.compute_pipeline);
  // Set Bind Group here
  compute_pass.DispatchWorkgroups(1,1,1);
  compute_pass.End();
  
  
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
  App app = {.window = {}, .gpu = {}, .logger = {}, .start_time = std::chrono::high_resolution_clock::now(), .last_frame_time = std::chrono::high_resolution_clock::now()};
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

std::string ReadFileToString(const std::string &path){
  std::ifstream file(path, std::ios::binary);
  if (!file){
    spdlog::warn("Failed to open and read file: {}", path);
    spdlog::warn("Returning empty string.");
    return std::string();
  }
  return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

wgpu::ShaderModule InitaliseShader(GPUContext &ctx, const std::string &file_path){
  ctx.shader_source = ReadFileToString(file_path);
  ctx.shader_last_edited = GetFileLastEdited(file_path);
  spdlog::trace("Shader source: {}", ctx.shader_source);
  wgpu::ShaderModuleDescriptor shader_descriptor = {};
  wgpu::ShaderSourceWGSL shader_code_descriptor = {};
  shader_code_descriptor.nextInChain = nullptr; // why does this exist?
  shader_code_descriptor.sType = wgpu::SType::ShaderSourceWGSL;
  shader_descriptor.nextInChain = &shader_code_descriptor;
  shader_code_descriptor.code = ctx.shader_source.c_str();

  return ctx.device.CreateShaderModule(&shader_descriptor);
}

void CreateShaderModules(GPUContext &ctx){
  ctx.shader_module = InitaliseShader(ctx, "assets/shader.wgsl");
  //ctx.computer_shader_module = InitaliseShader(ctx, "assets/compute_shader.wgsl");
}



//   wgpu::ShaderModule computer_shader_module

// }


void CreateRenderPipeline(GPUContext &ctx){
  wgpu::RenderPipelineDescriptor pipeline_descriptor = {};


  std::vector<wgpu::BindGroupLayoutEntry> bindings(1);
  for (auto &b : bindings){
    b = {};
  }
  bindings[BindingType::kUniforms].binding = 0;
  bindings[BindingType::kUniforms].visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;
  bindings[BindingType::kUniforms].buffer.type = wgpu::BufferBindingType::Uniform;
  bindings[BindingType::kUniforms].buffer.minBindingSize = sizeof(Uniforms);
  // bindings[BindingType::kComputeInput].binding = 1;
  // bindings[BindingType::kComputeInput].buffer.type = wgpu::BufferBindingType::ReadOnlyStorage;
  // bindings[BindingType::kComputeInput].visibility = wgpu::ShaderStage::Compute;
  // bindings[BindingType::kUniforms].buffer.minBindingSize = 0;
  // bindings[BindingType::kComputeOutput].binding = 2;
  // bindings[BindingType::kComputeOutput].buffer.type = wgpu::BufferBindingType::Storage;
  // bindings[BindingType::kComputeOutput].visibility = wgpu::ShaderStage::Compute;
  // bindings[BindingType::kUniforms].buffer.minBindingSize = 0;

  wgpu::BindGroupLayoutDescriptor bind_group_layout_descriptor = {};
  bind_group_layout_descriptor.entryCount = (uint32_t)bindings.size();
  bind_group_layout_descriptor.entries = bindings.data();

  ctx.bind_group_layout = ctx.device.CreateBindGroupLayout(&bind_group_layout_descriptor);
  
  wgpu::PipelineLayoutDescriptor pipeline_layout_descriptor = {};
  pipeline_layout_descriptor.bindGroupLayoutCount = 1;
  pipeline_layout_descriptor.bindGroupLayouts = &ctx.bind_group_layout;
  ctx.pipeline_layout = ctx.device.CreatePipelineLayout(&pipeline_layout_descriptor);

  pipeline_descriptor.layout = ctx.pipeline_layout;

  //Vertex Pipeline State
  // std::vector<float> point_data = {
  //   -0.8, -0.8, 1.0, 1.0, 1.0,
  //   0.8, -0.8, 1.0, 1.0, 1.0,
  //   0.8, 0.8, 1.0, 1.0, 1.0,
  //   -0.8, 0.8, 1.0, 1.0, 1.0
  // };

  // std::vector<uint16_t> index_data = {
  //   0 , 1, 2, // Triangle 0 connects points 0, 1, 2
  //   0, 2, 3 // Triangle 1 connects points 0, 2, and 3
  // };

  std::vector<float> point_data = {};
  std::vector<uint16_t> index_data = {};
  bool geometry_loaded = LoadGeometry("assets/webgpu.txt", point_data, index_data);
  if (!geometry_loaded){
    spdlog::error("Failed to load geometry from assets/webgpu.txt");
  }
  
  index_data.resize((index_data.size() + 1) & ~1); // Round up to a multiple of 2 bytes.
  ctx.index_count = static_cast<uint32_t>(index_data.size());
  const int kDataEntriesPerVertex = 5;
  wgpu::BufferDescriptor point_buffer_descriptor = {
    .label = "PointBuffer",
    .usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex,
    .size = ((point_data.size() * sizeof(float)) + 3) & ~3,
    .mappedAtCreation = false
  };
  ctx.point_buffer = ctx.device.CreateBuffer(&point_buffer_descriptor);
  ctx.queue.WriteBuffer(ctx.point_buffer, 0, point_data.data(), point_buffer_descriptor.size);
  wgpu::BufferDescriptor index_buffer_descriptor = {
    .label = "IndexBuffer",
    .usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Index,
    .size = ((index_data.size() * sizeof(uint16_t)) + 3) & ~3, // Rounds up to a multiple of 4 bytes
    .mappedAtCreation = false
  };
  ctx.index_buffer = ctx.device.CreateBuffer(&index_buffer_descriptor);
  ctx.queue.WriteBuffer(ctx.index_buffer, 0, index_data.data(), index_buffer_descriptor.size);
  pipeline_descriptor.vertex.bufferCount = 1;
  wgpu::VertexBufferLayout vertex_buffer_layout = {};

  std::vector<wgpu::VertexAttribute> vertex_attributes(2);
  vertex_attributes[VertexAttributeType::kPosition].shaderLocation = 0;
  vertex_attributes[VertexAttributeType::kPosition].format = wgpu::VertexFormat::Float32x2;
  vertex_attributes[VertexAttributeType::kPosition].offset = 0;
  vertex_attributes[VertexAttributeType::kColor].shaderLocation = 1;
  vertex_attributes[VertexAttributeType::kColor].format = wgpu::VertexFormat::Float32x3;
  vertex_attributes[VertexAttributeType::kColor].offset = 2 * sizeof(float);

  vertex_buffer_layout.attributeCount = static_cast<uint32_t>(vertex_attributes.size());
  vertex_buffer_layout.attributes = vertex_attributes.data();
  vertex_buffer_layout.arrayStride = kDataEntriesPerVertex * sizeof(float);
  vertex_buffer_layout.stepMode = wgpu::VertexStepMode::Vertex;
  pipeline_descriptor.vertex.buffers = &vertex_buffer_layout;
  pipeline_descriptor.vertex.module = ctx.shader_module;
  pipeline_descriptor.vertex.constantCount = 0;
  pipeline_descriptor.vertex.constants = nullptr;

  wgpu::BufferDescriptor uniform_buffer_descriptor = {
    .label = "UniformBuffer",
    .usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform,
    .size = sizeof(Uniforms), // 3 extra floats for alignment constraints?
    .mappedAtCreation = false
  };
  ctx.uniform_buffer = ctx.device.CreateBuffer(&uniform_buffer_descriptor);
  Uniforms uniforms = {.time = 0.0, .delta_time = kDefaultDeltaTime};
  ctx.queue.WriteBuffer(ctx.uniform_buffer, 0, &uniforms, sizeof(kUniforms));


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


  wgpu::BindGroupEntry binding = {};
  binding.binding = 0;
  binding.buffer = ctx.uniform_buffer;
  binding.offset = 0;
  binding.size = 4 * sizeof(float);
  wgpu::BindGroupDescriptor bind_group_descriptor = {};
  bind_group_descriptor.layout = ctx.bind_group_layout;
  bind_group_descriptor.entryCount = 1;
  bind_group_descriptor.entries = &binding;
  ctx.bind_group = ctx.device.CreateBindGroup(&bind_group_descriptor);
}




void PlayingWithBuffers(GPUContext &ctx){
  wgpu::BufferDescriptor buffer_descriptor = {
    .label = "Some GPU-side data buffer",
    .usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::CopySrc,
    .size = 16,
    .mappedAtCreation = false
  };
  wgpu::Buffer buffer_1 = ctx.device.CreateBuffer(&buffer_descriptor);
  buffer_descriptor.label = "Output Buffer";
  buffer_descriptor.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::MapRead;
  ctx.buffer_2 = ctx.device.CreateBuffer(&buffer_descriptor);
  std::vector<uint8_t> numbers(16);
  for (uint8_t i = 0; i < 16; ++i) numbers[i] = i;
  ctx.queue.WriteBuffer(buffer_1, 0, numbers.data(), numbers.size());
  wgpu::CommandEncoder encoder = ctx.device.CreateCommandEncoder(); // might need Default
  encoder.CopyBufferToBuffer(buffer_1, 0, ctx.buffer_2, 0, 16);
  wgpu::CommandBuffer command_copy = encoder.Finish();
  ctx.queue.Submit(1, &command_copy);
  ctx.queue.OnSubmittedWorkDone(GetPlatformCallbackMode(),[&ctx](wgpu::QueueWorkDoneStatus status, wgpu::StringView message){
    spdlog::info("Copied data from buffer from 1 to 2?: status: [{}], message '{}'" , status, message);
    ctx.buffer_2.MapAsync(wgpu::MapMode::Read, 0, 16, GetPlatformCallbackMode(), [&ctx]([[maybe_unused]]wgpu::MapAsyncStatus status, [[maybe_unused]]wgpu::StringView message){
      spdlog::info("Buffer 2 mapped with status: {}", status);
      uint8_t* buffer_data = (uint8_t*)ctx.buffer_2.GetConstMappedRange(0,16);
      std::string str = "Buffer: [";
      for (int i = 0; i < 16; ++i){
        str += std::to_string(buffer_data[i]) + ", ";
      }
      str += "]";
      spdlog::info(str);
      ctx.buffer_2.Unmap();
    });
  });
}

void CreateComputePipeline(GPUContext &ctx){
  // Compute pass here

  



  wgpu::ComputePipelineDescriptor compute_pipeline_descriptor = {};
  compute_pipeline_descriptor.compute.entryPoint = "cs_main";
  compute_pipeline_descriptor.compute.module = ctx.shader_module;
  ctx.compute_pipeline = ctx.device.CreateComputePipeline(&compute_pipeline_descriptor);

}




// Learning about the GPU
// a buffer is a chunk of memory allocated in VRAM.
// WriteBuffer copies the CPU side of the memory during transfer to its own location, that then it is put from there onto the GPU.
// Can be disabled with mapping.



