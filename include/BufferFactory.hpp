#ifndef INCLUDE_BUFFER_FACTORY_HPP_
#define INCLUDE_BUFFER_FACTORY_HPP_
#include <vector>
#include <webgpu/webgpu_cpp.h>
class BufferFactory {
 public:
  BufferFactory() = delete;
  [[nodiscard]] static wgpu::Buffer CreateVertex(const wgpu::Device& device,const wgpu::Queue& queue, const std::vector<float>& data);
  [[nodiscard]] static wgpu::Buffer CreateIndex(const wgpu::Device& device, const wgpu::Queue& queue, const std::vector<uint16_t>& data);
  [[nodiscard]] static wgpu::Buffer CreateUniform(const wgpu::Device& device, size_t size);
 private:
  [[nodiscard]] static constexpr size_t AlignSize(size_t size, size_t alignment) noexcept;
};
#endif //INCLUDE_BUFFER_FACTORY_HPP_