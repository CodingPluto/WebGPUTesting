#include "GPUContext.hpp"

#include <chrono>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <webgpu/webgpu_cpp.h>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

#include "App.hpp"
#include "RenderPipelineFactory.hpp"
#include "ShaderLoader.hpp"
#include "formatted_webgpu.h"
#include "glfw3webgpu.h"
#include "BufferFactory.hpp"

enum BindingType{
  kUniforms = 0,
  kComputeInput = 1,
  kComputeOutput = 2
};

struct Uniforms {
  glm::f32 time = 0; // at byte offset 0
  glm::f32 delta_time = 0; // at byte offset 
  std::array<glm::f32, 2> padding = {};
  glm::mat4 projection_matrix = {};
}; // Apparently takes 80 bytes!  Starts at an offset of 16.
void GPUContext::StartAdapterRequest(const wgpu::RequestAdapterOptions *options) {
  instance_.RequestAdapter(
    options,GetPlatformCallbackMode(), [this](wgpu::RequestAdapterStatus status, wgpu::Adapter result_adapter, wgpu::StringView message) {
      if (status == wgpu::RequestAdapterStatus::Success) {
          adapter_ = result_adapter;
          initialized_state_ = InitializationState::RequestingDevice; 
      } else {
          spdlog::error("Failed to get an adapter: {}", message);
          initialized_state_ = InitializationState::Failed;
      }
    }
  );
}
void GPUContext::StartDeviceRequest(const wgpu::DeviceDescriptor *descriptor){
  adapter_.RequestDevice(
    descriptor, GetPlatformCallbackMode(), [this](wgpu::RequestDeviceStatus status, wgpu::Device result_device, [[maybe_unused]]wgpu::StringView message) {
      if (status == wgpu::RequestDeviceStatus::Success) {
        device_ = result_device;
        initialized_state_ = InitializationState::ReceivedAdapterAndDevice;
      } else {
        spdlog::error("Failed to get a device: {}", message);
        initialized_state_ = InitializationState::Failed;
      }
    }
  );
}
void GPUContext::OutputFeatures(const wgpu::SupportedFeatures &features){
  for (size_t i = 0; i < features.featureCount; ++i){
    auto feature = features.features[i];
    spdlog::trace("feature: {}", feature);
  }
}
void GPUContext::OutputLimits(const wgpu::Limits &limits){
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
void GPUContext::InspectDevice(wgpu::Device device){
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
void GPUContext::InspectAdapter(const wgpu::Adapter &adapter){
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
void GPUContext::InitializeSurface(GLFWwindow *window) {
  spdlog::trace("Initializing surface for window. Window pointer: {}", static_cast<void*>(window));
  surface_ = wgpu::Surface::Acquire(glfwCreateWindowWGPUSurface(instance_.Get(), window));
  if (!surface_){
    spdlog::critical("Failed to create a surface for the window");
    assert(false);
  }
  int width, height;
  glfwGetWindowSize(window, &width, &height);
  surface_configuration_.width = width;
  surface_configuration_.height = height;
}
void GPUContext::InitializeInstance() {
  wgpu::InstanceDescriptor instance_descriptor = {};
  instance_ = wgpu::CreateInstance(&instance_descriptor);
  if (!instance_){
    spdlog::critical("Could not initalise WebGPU!");
    assert(false);
  }
  else spdlog::info("WGPUInstance created");
}
void GPUContext::InitializeCallbacks(){
  spdlog::info("Initalization stage: {}", initialized_state_);
  if (initialized_state_ == InitializationState::Uninitalised) {
    wgpu::RequestAdapterOptions adapter_options = {};
    StartAdapterRequest(&adapter_options);
    initialized_state_ = InitializationState::RequestingAdapter;
  }
  else if (initialized_state_ == InitializationState::RequestingDevice){
    InspectAdapter(adapter_);
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
    device_descriptor.requiredLimits = &limits;
    device_descriptor.SetDeviceLostCallback(wgpu::CallbackMode::AllowSpontaneous, []([[maybe_unused]]wgpu::Device const& device, wgpu::DeviceLostReason reason, wgpu::StringView message){
      spdlog::warn("Device lost. reason: {}", reason);
      if (message.data && message.length > 0) spdlog::info("({})", message);
    });
    device_descriptor.SetUncapturedErrorCallback([]([[maybe_unused]]wgpu::Device const& device, wgpu::ErrorType error, wgpu::StringView message){
      spdlog::error("Device uncaptured error: {}", error);
      if (message.data && message.length > 0) spdlog::info("({})", message);
    });
    StartDeviceRequest(&device_descriptor);
  }
  else if (initialized_state_ == InitializationState::ReceivedAdapterAndDevice){
    InspectDevice(device_);
    queue_ = device_.GetQueue();
    CreateShaderModules();
    ConfigureSurface();
    CreateRenderPipeline();
    CreateComputePipeline();
    initialized_state_ = InitializationState::Ready;
  }
}
std::pair<wgpu::SurfaceTexture, wgpu::TextureView> GPUContext::GetNextSurfaceViewData(){
  wgpu::SurfaceTexture surface_texture = {};
  surface_.GetCurrentTexture(&surface_texture);
  if (surface_texture.status != wgpu::SurfaceGetCurrentTextureStatus::SuccessOptimal){
    spdlog::critical("Couldnt't get current texture for the surface");
    assert(false);
    return {surface_texture, nullptr};
  }
  wgpu::TextureViewDescriptor view_descriptor = {
    .nextInChain = nullptr,
    .label = "Surface texture view",
    .format = surface_format_,
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
enum ScrollingState{
  kMovingUp = 0,
  kMovingDown = 1,
  kMovingLeft = 2,
  kMovingRight = 3
};


static ScrollingState scrolling_state_vertical = kMovingUp;
static ScrollingState scrolling_state_horiziontal = kMovingRight;
void GPUContext::Update(float delta_time, double total_time_elapsed_){
  #ifndef __EMSCRIPTEN__
    if (delta_time < 0.03){
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
      spdlog::warn("VSYNC not enabled, preventing overclocking!");
    }
    instance_.ProcessEvents();
  #endif
  if (initialized_state_ != InitializationState::Ready){
    InitializeCallbacks();
    return;
  }
  if (shader_last_edited_ != ShaderLoader::GetLastEdited("assets/shader.wgsl")){
    HotReloadShaders();
  }

  const float scroll_speed = 180 * delta_time;
  if (camera_y > surface_configuration_.height - 68){
    scrolling_state_vertical = kMovingUp;
  }
  if (camera_y < 5){
    scrolling_state_vertical = kMovingDown;
  }
  if (camera_x < 45){
    scrolling_state_horiziontal = kMovingRight;
  }
  if (camera_x > surface_configuration_.width - 73){
    scrolling_state_horiziontal = kMovingLeft;
  }
  switch(scrolling_state_vertical){
    case kMovingUp:
      camera_y -= scroll_speed;
      break;
    case kMovingDown:
      camera_y += scroll_speed;
      break;
    default:
      break;
  }
  switch(scrolling_state_horiziontal){
    case kMovingLeft:
      camera_x -= scroll_speed;
      break;
    case kMovingRight:
      camera_x += scroll_speed;
      break;
    default:
      break;
  }


  UpdateViewProjectionMatrices();
  Uniforms uniforms = {
    .time = static_cast<float>(total_time_elapsed_),
    .delta_time = (glm::f32)delta_time,
    .projection_matrix = projection_matrix_
  };
  queue_.WriteBuffer(uniform_buffer_, 0, &uniforms, sizeof(Uniforms));
  auto [surface_view, target_view] = GetNextSurfaceViewData();
  wgpu::CommandEncoderDescriptor encoder_descriptor = {.nextInChain = nullptr, .label = "My command encoder"};
  wgpu::CommandEncoder encoder = device_.CreateCommandEncoder(&encoder_descriptor);
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
  render_pass.SetPipeline(pipeline_);
  render_pass.SetVertexBuffer(0, vertex_buffer_, 0, vertex_buffer_.GetSize());
  render_pass.SetIndexBuffer(index_buffer_, wgpu::IndexFormat::Uint16, 0, index_buffer_.GetSize());
  render_pass.SetBindGroup(0, bind_group_, 0, nullptr);
  render_pass.DrawIndexed(index_count_, 1, 0,0, 0);
  render_pass.End();
  wgpu::ComputePassDescriptor compute_pass_descriptor = {.timestampWrites = nullptr};
  wgpu::ComputePassEncoder compute_pass = encoder.BeginComputePass(&compute_pass_descriptor);
  compute_pass.SetPipeline(compute_pipeline_);
  // Set Bind Group here
  compute_pass.DispatchWorkgroups(1,1,1);
  compute_pass.End();
  wgpu::CommandBufferDescriptor command_buffer_descriptor = {.nextInChain = nullptr, .label = "My command buffer"};
  wgpu::CommandBuffer command = encoder.Finish(&command_buffer_descriptor);
  spdlog::trace("Submitting command..");
  queue_.Submit(1, &command);
  spdlog::trace("Command submitted");
    queue_.OnSubmittedWorkDone(wgpu::CallbackMode::AllowSpontaneous,[](wgpu::QueueWorkDoneStatus status, wgpu::StringView message){
    spdlog::trace("queue work finished. Status: {} ; Message: {}", status, message);
  });
  #ifndef __EMSCRIPTEN__
    surface_.Present();
  #endif
}
void GPUContext::CreatePipelineLayout() {
  std::vector<wgpu::BindGroupLayoutEntry> bindings(1);
  for (auto &b : bindings){
    b = {};
  }
  bindings[BindingType::kUniforms].binding = 0;
  bindings[BindingType::kUniforms].visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;
  bindings[BindingType::kUniforms].buffer.type = wgpu::BufferBindingType::Uniform;
  bindings[BindingType::kUniforms].buffer.minBindingSize = sizeof(Uniforms);
  wgpu::BindGroupLayoutDescriptor bind_group_layout_descriptor = {};
  bind_group_layout_descriptor.entryCount = (uint32_t)bindings.size();
  bind_group_layout_descriptor.entries = bindings.data();
  bind_group_layout_ = device_.CreateBindGroupLayout(&bind_group_layout_descriptor);
  wgpu::PipelineLayoutDescriptor pipeline_layout_descriptor = {};
  pipeline_layout_descriptor.bindGroupLayoutCount = 1;
  pipeline_layout_descriptor.bindGroupLayouts = &bind_group_layout_;
  pipeline_layout_ = device_.CreatePipelineLayout(&pipeline_layout_descriptor);
}
void GPUContext::ConfigureSurface(){
  spdlog::debug("Configuring Surface");
  wgpu::SurfaceCapabilities capabilities = {};
  surface_.GetCapabilities(adapter_, &capabilities);
  surface_format_ = capabilities.formatCount > 0 ? capabilities.formats[0] : wgpu::TextureFormat::BGRA8Unorm;
  surface_configuration_.format = surface_format_;
  surface_configuration_.viewFormatCount = 0;
  surface_configuration_.viewFormats = nullptr;
  surface_configuration_.usage = wgpu::TextureUsage::RenderAttachment;
  surface_configuration_.device = device_;
  surface_configuration_.presentMode = wgpu::PresentMode::Fifo;
  surface_configuration_.alphaMode = wgpu::CompositeAlphaMode::Auto;
  surface_.Configure(&surface_configuration_);
  spdlog::debug("Configured surface");
}
void GPUContext::CreateRenderPipeline(){
  CreatePipelineLayout();
  pipeline_ = RenderPipelineFactory::Create(device_, shader_module_, pipeline_layout_, surface_format_);
  std::vector<float> vertex_data = {};
  std::vector<uint16_t> index_data = {};
  bool geometry_loaded = LoadGeometry("assets/webgpu.txt", vertex_data, index_data);
  if (!geometry_loaded){
    spdlog::error("Failed to load geometry from assets/webgpu.txt");
  }
  index_data.resize((index_data.size() + 1) & ~1); // Round up to a multiple of 2 bytes.
  index_count_ = static_cast<uint32_t>(index_data.size());
  vertex_buffer_ = BufferFactory::CreateVertex(device_, queue_, vertex_data);
  index_buffer_ = BufferFactory::CreateIndex(device_, queue_, index_data);
  uniform_buffer_ = BufferFactory::CreateUniform(device_, sizeof(Uniforms));
  UpdateViewProjectionMatrices();
  Uniforms uniforms = {.time = 0.0, .delta_time = kDefaultDeltaTime, .projection_matrix = projection_matrix_};
  queue_.WriteBuffer(uniform_buffer_, 0, &uniforms, sizeof(Uniforms));
  wgpu::BindGroupEntry binding = {};
  binding.binding = 0;
  binding.buffer = uniform_buffer_;
  binding.offset = 0;
  binding.size = sizeof(Uniforms);
  wgpu::BindGroupDescriptor bind_group_descriptor = {};
  bind_group_descriptor.layout = bind_group_layout_;
  bind_group_descriptor.entryCount = 1;
  bind_group_descriptor.entries = &binding;
  bind_group_ = device_.CreateBindGroup(&bind_group_descriptor);
}
void GPUContext::CreateResources() {
  // TODO: Initialize point_buffer, index_buffer, and uniform_buffer
}
void GPUContext::HotReloadShaders() {
  CreateShaderModules();
  CreateRenderPipeline();
  CreateComputePipeline();
}
void GPUContext::CreateComputePipeline(){
  wgpu::ComputePipelineDescriptor compute_pipeline_descriptor = {};
  compute_pipeline_descriptor.compute.entryPoint = "cs_main";
  compute_pipeline_descriptor.compute.module = shader_module_;
  compute_pipeline_ = device_.CreateComputePipeline(&compute_pipeline_descriptor);
}
void GPUContext::CreateShaderModules(){
  shader_module_ = ShaderLoader::Load(device_, "assets/shader.wgsl");
  shader_last_edited_ = ShaderLoader::GetLastEdited("assets/shader.wgsl");
  //shader_module_ = InitaliseShader("assets/shader.wgsl");
  //ctx.computer_shader_module = InitaliseShader(ctx, "assets/compute_shader.wgsl");
}

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


void GPUContext::UpdateViewProjectionMatrices(){
  projection_matrix_ = glm::ortho(0.0f, static_cast<float>(surface_configuration_.width), static_cast<float>(surface_configuration_.height), 0.0f);
  view_matrix_ = glm::translate(glm::mat4(1.0f), glm::vec3(camera_x, camera_y, 0.0f));
  projection_matrix_ = projection_matrix_ * view_matrix_;
}
