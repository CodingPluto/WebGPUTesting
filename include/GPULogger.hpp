#ifndef INCLUDE_GPU_LOGGER_HPP
#define INCLUDE_GPU_LOGGER_HPP

#include <webgpu/webgpu_cpp.h>

class GPULogger{
public:
  GPULogger() = delete;
  static void OutputFeatures(const wgpu::SupportedFeatures &features);
  static void OutputLimits(const wgpu::Limits &limits);
  static void InspectDevice(wgpu::Device device);
  static void InspectAdapter(const wgpu::Adapter &adapter);
};

#endif // INCLUDE_GPU_LOGGER_HPP