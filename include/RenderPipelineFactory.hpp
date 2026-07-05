#ifndef INCLUDE_RENDER_PIPELINE_FACTORY_HPP_
#define INCLUDE_RENDER_PIPELINE_FACTORY_HPP_

#include <webgpu/webgpu_cpp.h>

class RenderPipelineFactory {
 public:
  RenderPipelineFactory() = delete;

  [[nodiscard]] static wgpu::RenderPipeline Create(
      const wgpu::Device& device,
      const wgpu::ShaderModule& shader_module,
      const wgpu::PipelineLayout& layout,
      wgpu::TextureFormat surface_format);

 private:
  [[nodiscard]] static wgpu::VertexBufferLayout CreateVertexLayout();
};

#endif  // INCLUDE_RENDER_PIPELINE_FACTORY_HPP_