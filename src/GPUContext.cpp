#include "GPUContext.hpp"

#include <chrono>
#include <cstdint>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <imgui.h>
#include <webgpu/webgpu_cpp.h>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_wgpu.h>

#include "App.hpp"
#include "RenderPipelineFactory.hpp"
#include "ShaderLoader.hpp"
#include "formatted_webgpu.h"
#include "glfw3webgpu.h"
#include "BufferFactory.hpp"
#include "RenderDataPacker.hpp"
#include "StartupCoordinator.hpp"

#ifndef NDEBUG
  #include "DebugTools.hpp"
#endif

enum BindingType {
  kUniforms = 0,
  kObjectData
};
struct Uniforms {
  glm::f32 time = 0; // at byte offset 0
  glm::f32 delta_time = 0; // at byte offset 
  std::array<glm::f32, 2> padding = {};
  glm::mat4 projection_matrix = {};
}; // Apparently takes 80 bytes!  Starts at an offset of 16.

void GPUContext::UpdateObjectDataScratchPad(std::span<const GPUObjectData> objects){
  object_data_scratchpad_span_ = objects;
}
void GPUContext::StartAdapterRequest(const wgpu::RequestAdapterOptions *options, StartupCoordinator &startup_coordinator) {
  instance_.RequestAdapter(
    options,GetPlatformCallbackMode(), [this, &startup_coordinator](wgpu::RequestAdapterStatus status, wgpu::Adapter result_adapter, wgpu::StringView message) {
      if (status == wgpu::RequestAdapterStatus::Success) {
          spdlog::debug("Recieved adapter.");
          adapter_ = result_adapter;
          startup_coordinator.SetInitalizedState(InitializationState::RequestingDevice); 
      } else {
          spdlog::error("Failed to get an adapter: {}", message);
          startup_coordinator.SetInitalizedState(InitializationState::Failed);
      }
    }
  );
}
void GPUContext::StartDeviceRequest(const wgpu::DeviceDescriptor *descriptor, StartupCoordinator &startup_coordinator){
  adapter_.RequestDevice(
    descriptor, GetPlatformCallbackMode(), [this, &startup_coordinator](wgpu::RequestDeviceStatus status, wgpu::Device result_device, [[maybe_unused]]wgpu::StringView message) {
      if (status == wgpu::RequestDeviceStatus::Success) {
        spdlog::debug("Recieved device.");
        device_ = result_device;
        startup_coordinator.SetInitalizedState(InitializationState::ReceivedAdapterAndDevice);
      } else {
        spdlog::error("Failed to get a device: {}", message);
        startup_coordinator.SetInitalizedState(InitializationState::Failed);
      }
    }
  );
}
void GPUContext::SetupAdapterRequest(StartupCoordinator &startup_coordinator){
  StartAdapterRequest(&adapter_options_, startup_coordinator);
}
void GPUContext::SetupDeviceRequest(StartupCoordinator &startup_coordinator){
  device_descriptor_.nextInChain = nullptr;
  device_descriptor_.label = WGPUStringView{.data = "TestDevice",.length = WGPU_STRLEN};
  device_descriptor_.requiredFeatureCount = 0;
  device_descriptor_.requiredLimits = nullptr;
  device_descriptor_.defaultQueue.nextInChain = nullptr;
  device_descriptor_.defaultQueue.label = WGPUStringView{.data = "DefaultQueue",.length = WGPU_STRLEN};
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
  device_descriptor_.requiredLimits = &limits;
  device_descriptor_.SetDeviceLostCallback(wgpu::CallbackMode::AllowSpontaneous, []([[maybe_unused]]wgpu::Device const& device, wgpu::DeviceLostReason reason, wgpu::StringView message){
    spdlog::warn("Device lost. reason: {}", reason);
    if (message.data && message.length > 0) spdlog::info("({})", message);
  });
  device_descriptor_.SetUncapturedErrorCallback([]([[maybe_unused]]wgpu::Device const& device, wgpu::ErrorType error, wgpu::StringView message){
    spdlog::error("Device uncaptured error: {}", error);
    if (message.data && message.length > 0) spdlog::info("({})", message);
  });
  StartDeviceRequest(&device_descriptor_, startup_coordinator);
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
void GPUContext::ConfigureQueue(){
  queue_ = device_.GetQueue();
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
void GPUContext::ProcessEvents(){
  instance_.ProcessEvents();
}
void GPUContext::Update(float delta_time, double total_time_elapsed_){
  #ifndef NDEBUG
    DebugSleep(0.05);
  #endif
  #ifndef __EMSCRIPTEN__
    if (delta_time < 0.003){
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
      spdlog::warn("VSYNC not enabled, preventing overclocking!");
    }

  #endif
  if (shader_last_edited_ != ShaderLoader::GetLastEdited("assets/shader.wgsl")){
    HotReloadShaders();
  }

  UpdateViewProjectionMatrices();
  Uniforms uniforms = {
    .time = static_cast<float>(total_time_elapsed_),
    .delta_time = (glm::f32)delta_time,
    .projection_matrix = projection_matrix_
  };
  queue_.WriteBuffer(storage_buffer_, 0, object_data_scratchpad_span_.data(), object_data_scratchpad_span_.size() * sizeof(GPUObjectData));
  queue_.WriteBuffer(uniform_buffer_, 0, &uniforms, sizeof(Uniforms));
  auto [surface_view, target_view] = GetNextSurfaceViewData();
  wgpu::CommandEncoderDescriptor encoder_descriptor = {.nextInChain = nullptr, .label = "My command encoder"};
  encoder_ = device_.CreateCommandEncoder(&encoder_descriptor);
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
  render_pass_ = encoder_.BeginRenderPass(&render_pass_descriptor);
  render_pass_.SetPipeline(pipeline_);
  render_pass_.SetVertexBuffer(0, vertex_buffer_, 0, vertex_buffer_.GetSize());
  render_pass_.SetIndexBuffer(index_buffer_, wgpu::IndexFormat::Uint16, 0, index_buffer_.GetSize());
  render_pass_.SetBindGroup(0, bind_group_, 0, nullptr);
  render_pass_.DrawIndexed(index_count_, object_data_scratchpad_span_.size(), 0,0, 0);
  // might need to change instanceCount to a different variable if we allocate differently in the future.
  

  // wgpu::ComputePassDescriptor compute_pass_descriptor = {.timestampWrites = nullptr};
  // wgpu::ComputePassEncoder compute_pass = encoder.BeginComputePass(&compute_pass_descriptor);
  // compute_pass.SetPipeline(compute_pipeline_);
  // // Set Bind Group here
  // compute_pass.DispatchWorkgroups(1,1,1);
  // compute_pass.End();

}
void GPUContext::Render(){
  render_pass_.End();
  wgpu::CommandBufferDescriptor command_buffer_descriptor = {.nextInChain = nullptr, .label = "My command buffer"};
  wgpu::CommandBuffer command = encoder_.Finish(&command_buffer_descriptor);
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
  std::vector<wgpu::BindGroupLayoutEntry> bindings(2);
  for (auto &b : bindings){
    b = {};
  }
  bindings[BindingType::kUniforms].binding = 0;
  bindings[BindingType::kUniforms].visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;
  bindings[BindingType::kUniforms].buffer.type = wgpu::BufferBindingType::Uniform;
  bindings[BindingType::kUniforms].buffer.minBindingSize = sizeof(Uniforms);
  bindings[BindingType::kObjectData].binding = 1;
  bindings[BindingType::kObjectData].visibility = wgpu::ShaderStage::Vertex;
  bindings[BindingType::kObjectData].buffer.type = wgpu::BufferBindingType::ReadOnlyStorage;
  bindings[BindingType::kObjectData].buffer.minBindingSize = sizeof(GPUObjectData);
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
  wgpu::BufferDescriptor storage_descriptor = {
    .usage = wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopyDst,
    .size = kMaxObjects * sizeof(GPUObjectData)
  };
  storage_buffer_ = device_.CreateBuffer(&storage_descriptor);
  std::vector<wgpu::BindGroupEntry> entries(2);
  entries[BindingType::kUniforms].binding = 0;
  entries[BindingType::kUniforms].buffer = uniform_buffer_;
  entries[BindingType::kUniforms].size = sizeof(Uniforms);
  entries[BindingType::kObjectData].binding = 1;
  entries[BindingType::kObjectData].buffer = storage_buffer_;
  entries[BindingType::kObjectData].size = kMaxObjects * sizeof(GPUObjectData);
  spdlog::debug("Size of Object Data: {}", sizeof(GPUObjectData));
  wgpu::BindGroupDescriptor bind_group_descriptor = {};
  bind_group_descriptor.layout = bind_group_layout_;
  bind_group_descriptor.entryCount = (uint32_t)entries.size();
  bind_group_descriptor.entries = entries.data();
  bind_group_ = device_.CreateBindGroup(&bind_group_descriptor);
}
void GPUContext::CreateResources() {}
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
void GPUContext::UpdateViewProjectionMatrices(){
  projection_matrix_ = glm::ortho(0.0f, static_cast<float>(surface_configuration_.width), static_cast<float>(surface_configuration_.height), 0.0f);
  view_matrix_ = glm::translate(glm::mat4(1.0f), glm::vec3(camera_x, camera_y, 0.0f));
  projection_matrix_ = projection_matrix_ * view_matrix_;
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