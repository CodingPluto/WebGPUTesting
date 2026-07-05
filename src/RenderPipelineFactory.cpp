#include "RenderPipelineFactory.hpp"

enum VertexAttributeType{
  kPosition = 0,
  kColor = 1
};


wgpu::RenderPipeline RenderPipelineFactory::Create(const wgpu::Device& device, const wgpu::ShaderModule& shader_module, const wgpu::PipelineLayout& layout, wgpu::TextureFormat surface_format) {

  // TODO: Create RenderPipelineDescriptor
  wgpu::RenderPipelineDescriptor pipeline_descriptor = {};
  pipeline_descriptor.vertex.bufferCount = 1;
  pipeline_descriptor.layout = layout;
  // TODO: Configure vertex, fragment, primitive, and multisample states
  wgpu::VertexBufferLayout vertex_layout = CreateVertexLayout();
  pipeline_descriptor.vertex.buffers = &vertex_layout;
  pipeline_descriptor.vertex.module = shader_module;
  pipeline_descriptor.vertex.constantCount = 0;
  pipeline_descriptor.vertex.constants = nullptr;

  //Primitive Pipeline state
  pipeline_descriptor.primitive.topology = wgpu::PrimitiveTopology::TriangleList;
  pipeline_descriptor.primitive.stripIndexFormat = wgpu::IndexFormat::Undefined;
  pipeline_descriptor.primitive.frontFace = wgpu::FrontFace::CCW;
  pipeline_descriptor.primitive.cullMode = wgpu::CullMode::None; // Set to none for developing so stuff doesn't disappear

  //Fragment Shader Stage
  wgpu::FragmentState fragment_state = {};
  fragment_state.module = shader_module;
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
  color_target.format = surface_format;
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
  return device.CreateRenderPipeline(&pipeline_descriptor);;
}

wgpu::VertexBufferLayout RenderPipelineFactory::CreateVertexLayout() {
  wgpu::VertexBufferLayout vertex_buffer_layout = {};
  static std::vector<wgpu::VertexAttribute> vertex_attributes(2);
  vertex_attributes[VertexAttributeType::kPosition].shaderLocation = 0;
  vertex_attributes[VertexAttributeType::kPosition].format = wgpu::VertexFormat::Float32x2;
  vertex_attributes[VertexAttributeType::kPosition].offset = 0;
  vertex_attributes[VertexAttributeType::kColor].shaderLocation = 1;
  vertex_attributes[VertexAttributeType::kColor].format = wgpu::VertexFormat::Float32x3;
  vertex_attributes[VertexAttributeType::kColor].offset = 2 * sizeof(float);
  vertex_buffer_layout.attributeCount = static_cast<uint32_t>(vertex_attributes.size());
  vertex_buffer_layout.attributes = vertex_attributes.data();
  const int kDataEntriesPerVertex = 5;
  vertex_buffer_layout.arrayStride = kDataEntriesPerVertex * sizeof(float);
  vertex_buffer_layout.stepMode = wgpu::VertexStepMode::Vertex;
  return vertex_buffer_layout;
}