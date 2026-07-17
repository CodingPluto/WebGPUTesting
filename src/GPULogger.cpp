#include "GPULogger.hpp"

#include <webgpu/webgpu_cpp.h>
#include <spdlog/spdlog.h>

#include "formatted_webgpu.h"

void GPULogger::OutputFeatures(const wgpu::SupportedFeatures &features){
  for (size_t i = 0; i < features.featureCount; ++i){
    auto feature = features.features[i];
    spdlog::debug("feature: {}", feature);
  }
}
void GPULogger::OutputLimits(const wgpu::Limits &limits){
  spdlog::debug("maxTextureDimension1D: {}", limits.maxTextureDimension1D);
  spdlog::debug("maxTextureDimension2D: {}", limits.maxTextureDimension2D);
  spdlog::debug("maxTextureDimension3D: {}", limits.maxTextureDimension3D);
  spdlog::debug("maxTextureArrayLayers: {}", limits.maxTextureArrayLayers);
  spdlog::debug("maxVertexAttributes: {}", limits.maxVertexAttributes);
  spdlog::debug("maxVertexBuffers: {}", limits.maxVertexBuffers);
  spdlog::debug("maxVertexBuffersArrayStride: {}", limits.maxVertexBufferArrayStride);
  spdlog::debug("maxBindGroups: {}", limits.maxBindGroups);
  spdlog::debug("maxUniformBuffersPerShaderStage: {}", limits.maxUniformBuffersPerShaderStage);
  spdlog::debug("maxUniformBufferBindingSize: {}", limits.maxUniformBufferBindingSize);
}
void GPULogger::InspectDevice(wgpu::Device device){
  //Features
  spdlog::debug("Device features: ");
  wgpu::SupportedFeatures features;
  device.GetFeatures(&features);
  OutputFeatures(features);
  //Limits
  spdlog::debug("Device limits: ");
  wgpu::Limits limits = {};
  bool success = device.GetLimits(&limits) == wgpu::Status::Success;
  if (success) {
    OutputLimits(limits);
  }
}
void GPULogger::InspectAdapter(const wgpu::Adapter &adapter){
  //Limits
  spdlog::debug("Adapter limits:");
  wgpu::Limits limits = {};
  bool success = adapter.GetLimits(&limits) == wgpu::Status::Success;
  if (success) {
    OutputLimits(limits);
  }
  // Features
  spdlog::debug("Adapter features:");
  wgpu::SupportedFeatures features;
  adapter.GetFeatures(&features);
  OutputFeatures(features);
  // Properties
  spdlog::debug("Adapter properties:");
  wgpu::AdapterInfo info = {};
  adapter.GetInfo(&info);
  spdlog::debug("vendorID: {}", info.vendorID);
  spdlog::debug("vendor: {}", info.vendor);
  spdlog::debug("architecture: {}", info.architecture);
  spdlog::debug("deviceID: {}", info.deviceID);
  spdlog::debug("device: {}", info.device);
  spdlog::debug("description: {}", info.description);
  spdlog::debug("subgroup minimum size: {}", info.subgroupMinSize);
  spdlog::debug("subgroup maximum size: {}", info.subgroupMaxSize);
  spdlog::debug("adapter type: {}", info.adapterType);
  spdlog::debug("backend type: {}", info.backendType);
}