#include "BufferFactory.hpp"
#include <bit>
#include <cstdint>
#include <spdlog/spdlog.h>
wgpu::Buffer BufferFactory::CreateVertex(const wgpu::Device& device, const wgpu::Queue& queue, const std::vector<float>& data) {
  wgpu::BufferDescriptor vertex_buffer_descriptor = {
    .label = "PointBuffer",
    .usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex,
    .size = AlignSize(data.size() * sizeof(float), 4),
    .mappedAtCreation = false
  };
  wgpu::Buffer vertex_buffer = device.CreateBuffer(&vertex_buffer_descriptor);
  queue.WriteBuffer(vertex_buffer, 0, data.data(), vertex_buffer_descriptor.size);
  return vertex_buffer;
}

wgpu::Buffer BufferFactory::CreateIndex(const wgpu::Device& device, const wgpu::Queue& queue, const std::vector<uint16_t>& data) {
  wgpu::BufferDescriptor index_buffer_descriptor = {
    .label = "IndexBuffer",
    .usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Index,
    .size = AlignSize(data.size() * sizeof(uint16_t), 4), // Rounds up to a multiple of 4 bytes
    .mappedAtCreation = false
  };
  wgpu::Buffer index_buffer = device.CreateBuffer(&index_buffer_descriptor);
  queue.WriteBuffer(index_buffer, 0, data.data(), index_buffer_descriptor.size);
  return index_buffer;
}

wgpu::Buffer BufferFactory::CreateUniform(const wgpu::Device& device, size_t size) {
  wgpu::BufferDescriptor uniform_buffer_descriptor = {
      .label = "UniformBuffer",
      .usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform,
      .size = size,
      .mappedAtCreation = false
    };
  spdlog::info("Creating a UniformBuffer with a size of {} bytes", size);
  return device.CreateBuffer(&uniform_buffer_descriptor);
}
constexpr size_t BufferFactory::AlignSize(size_t size, size_t alignment) noexcept{
  assert(std::has_single_bit(alignment)); // Confirming we are conforming to multiples of 2 bytes
  return (size + alignment - 1) & ~(alignment - 1);
}

