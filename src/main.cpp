#include <iostream>
#include <cassert>
#include <vector>
#include <iomanip>
#include <format>
#include <print>


#include <webgpu/webgpu_cpp.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

template <>
struct std::formatter<wgpu::StringView> : std::formatter<std::string_view> {
  auto format(const wgpu::StringView &wgpu_str, std::format_context &ctx) const {
    std::string_view view = {};
    if (wgpu_str.data){
      view = (wgpu_str.length == WGPU_STRLEN) ? std::string_view{wgpu_str.data} : std::string_view{wgpu_str.data, wgpu_str.length};
    }
    return std::formatter<std::string_view>::format(view, ctx);
  }
};
template <>
struct std::formatter<wgpu::DeviceLostReason> : std::formatter<std::string_view> {
  auto format(wgpu::DeviceLostReason v, std::format_context& ctx) const {
    std::string_view s;
    switch (v) {
      case wgpu::DeviceLostReason::Unknown: s = "Unknown"; break;
      case wgpu::DeviceLostReason::Destroyed: s = "Destroyed"; break;
      case wgpu::DeviceLostReason::CallbackCancelled: s = "CallbackCancelled"; break;
      case wgpu::DeviceLostReason::FailedCreation: s = "FailedCreation"; break;
      default: s = "UnknownDeviceLostReason"; break;
    }
    return std::formatter<std::string_view>::format(s, ctx);
  }
};
template <>
struct std::formatter<wgpu::FeatureName> : std::formatter<std::string_view> {
  auto format(wgpu::FeatureName v, std::format_context& ctx) const {
    std::string_view s;
    switch (v) {
      case wgpu::FeatureName::CoreFeaturesAndLimits: s = "CoreFeaturesAndLimits"; break;
      case wgpu::FeatureName::DepthClipControl: s = "DepthClipControl"; break;
      case wgpu::FeatureName::Depth32FloatStencil8: s = "Depth32FloatStencil8"; break;
      case wgpu::FeatureName::TextureCompressionBC: s = "TextureCompressionBC"; break;
      case wgpu::FeatureName::TextureCompressionBCSliced3D: s = "TextureCompressionBCSliced3D"; break;
      case wgpu::FeatureName::TextureCompressionETC2: s = "TextureCompressionETC2"; break;
      case wgpu::FeatureName::TextureCompressionASTC: s = "TextureCompressionASTC"; break;
      case wgpu::FeatureName::TextureCompressionASTCSliced3D: s = "TextureCompressionASTCSliced3D"; break;
      case wgpu::FeatureName::TimestampQuery: s = "TimestampQuery"; break;
      case wgpu::FeatureName::IndirectFirstInstance: s = "IndirectFirstInstance"; break;
      case wgpu::FeatureName::ShaderF16: s = "ShaderF16"; break;
      case wgpu::FeatureName::RG11B10UfloatRenderable: s = "RG11B10UfloatRenderable"; break;
      case wgpu::FeatureName::BGRA8UnormStorage: s = "BGRA8UnormStorage"; break;
      case wgpu::FeatureName::Float32Filterable: s = "Float32Filterable"; break;
      case wgpu::FeatureName::Float32Blendable: s = "Float32Blendable"; break;
      case wgpu::FeatureName::ClipDistances: s = "ClipDistances"; break;
      case wgpu::FeatureName::DualSourceBlending: s = "DualSourceBlending"; break;
      case wgpu::FeatureName::Subgroups: s = "Subgroups"; break;
      case wgpu::FeatureName::TextureFormatsTier1: s = "TextureFormatsTier1"; break;
      case wgpu::FeatureName::TextureFormatsTier2: s = "TextureFormatsTier2"; break;
      case wgpu::FeatureName::PrimitiveIndex: s = "PrimitiveIndex"; break;
      case wgpu::FeatureName::TextureComponentSwizzle: s = "TextureComponentSwizzle"; break;
      case wgpu::FeatureName::DawnInternalUsages: s = "DawnInternalUsages"; break;
      case wgpu::FeatureName::DawnMultiPlanarFormats: s = "DawnMultiPlanarFormats"; break;
      case wgpu::FeatureName::DawnNative: s = "DawnNative"; break;
      case wgpu::FeatureName::ChromiumExperimentalTimestampQueryInsidePasses: s = "ChromiumExperimentalTimestampQueryInsidePasses"; break;
      case wgpu::FeatureName::ImplicitDeviceSynchronization: s = "ImplicitDeviceSynchronization"; break;
      case wgpu::FeatureName::TransientAttachments: s = "TransientAttachments"; break;
      case wgpu::FeatureName::MSAARenderToSingleSampled: s = "MSAARenderToSingleSampled"; break;
      case wgpu::FeatureName::D3D11MultithreadProtected: s = "D3D11MultithreadProtected"; break;
      case wgpu::FeatureName::ANGLETextureSharing: s = "ANGLETextureSharing"; break;
      case wgpu::FeatureName::PixelLocalStorageCoherent: s = "PixelLocalStorageCoherent"; break;
      case wgpu::FeatureName::PixelLocalStorageNonCoherent: s = "PixelLocalStorageNonCoherent"; break;
      case wgpu::FeatureName::Unorm16TextureFormats: s = "Unorm16TextureFormats"; break;
      case wgpu::FeatureName::MultiPlanarFormatExtendedUsages: s = "MultiPlanarFormatExtendedUsages"; break;
      case wgpu::FeatureName::MultiPlanarFormatP010: s = "MultiPlanarFormatP010"; break;
      case wgpu::FeatureName::HostMappedPointer: s = "HostMappedPointer"; break;
      case wgpu::FeatureName::MultiPlanarRenderTargets: s = "MultiPlanarRenderTargets"; break;
      case wgpu::FeatureName::MultiPlanarFormatNv12a: s = "MultiPlanarFormatNv12a"; break;
      case wgpu::FeatureName::FramebufferFetch: s = "FramebufferFetch"; break;
      case wgpu::FeatureName::BufferMapExtendedUsages: s = "BufferMapExtendedUsages"; break;
      case wgpu::FeatureName::AdapterPropertiesMemoryHeaps: s = "AdapterPropertiesMemoryHeaps"; break;
      case wgpu::FeatureName::AdapterPropertiesD3D: s = "AdapterPropertiesD3D"; break;
      case wgpu::FeatureName::AdapterPropertiesVk: s = "AdapterPropertiesVk"; break;
      case wgpu::FeatureName::DawnFormatCapabilities: s = "DawnFormatCapabilities"; break;
      case wgpu::FeatureName::DawnDrmFormatCapabilities: s = "DawnDrmFormatCapabilities"; break;
      case wgpu::FeatureName::MultiPlanarFormatNv16: s = "MultiPlanarFormatNv16"; break;
      case wgpu::FeatureName::MultiPlanarFormatNv24: s = "MultiPlanarFormatNv24"; break;
      case wgpu::FeatureName::MultiPlanarFormatP210: s = "MultiPlanarFormatP210"; break;
      case wgpu::FeatureName::MultiPlanarFormatP410: s = "MultiPlanarFormatP410"; break;
      case wgpu::FeatureName::SharedTextureMemoryVkDedicatedAllocation: s = "SharedTextureMemoryVkDedicatedAllocation"; break;
      case wgpu::FeatureName::SharedTextureMemoryAHardwareBuffer: s = "SharedTextureMemoryAHardwareBuffer"; break;
      case wgpu::FeatureName::SharedTextureMemoryDmaBuf: s = "SharedTextureMemoryDmaBuf"; break;
      case wgpu::FeatureName::SharedTextureMemoryOpaqueFD: s = "SharedTextureMemoryOpaqueFD"; break;
      case wgpu::FeatureName::SharedTextureMemoryZirconHandle: s = "SharedTextureMemoryZirconHandle"; break;
      case wgpu::FeatureName::SharedTextureMemoryDXGISharedHandle: s = "SharedTextureMemoryDXGISharedHandle"; break;
      case wgpu::FeatureName::SharedTextureMemoryD3D11Texture2D: s = "SharedTextureMemoryD3D11Texture2D"; break;
      case wgpu::FeatureName::SharedTextureMemoryIOSurface: s = "SharedTextureMemoryIOSurface"; break;
      case wgpu::FeatureName::SharedTextureMemoryEGLImage: s = "SharedTextureMemoryEGLImage"; break;
      case wgpu::FeatureName::SharedFenceVkSemaphoreOpaqueFD: s = "SharedFenceVkSemaphoreOpaqueFD"; break;
      case wgpu::FeatureName::SharedFenceSyncFD: s = "SharedFenceSyncFD"; break;
      case wgpu::FeatureName::SharedFenceVkSemaphoreZirconHandle: s = "SharedFenceVkSemaphoreZirconHandle"; break;
      case wgpu::FeatureName::SharedFenceDXGISharedHandle: s = "SharedFenceDXGISharedHandle"; break;
      case wgpu::FeatureName::SharedFenceMTLSharedEvent: s = "SharedFenceMTLSharedEvent"; break;
      case wgpu::FeatureName::SharedBufferMemoryD3D12Resource: s = "SharedBufferMemoryD3D12Resource"; break;
      case wgpu::FeatureName::StaticSamplers: s = "StaticSamplers"; break;
      case wgpu::FeatureName::YCbCrVulkanSamplers: s = "YCbCrVulkanSamplers"; break;
      case wgpu::FeatureName::ShaderModuleCompilationOptions: s = "ShaderModuleCompilationOptions"; break;
      case wgpu::FeatureName::DawnLoadResolveTexture: s = "DawnLoadResolveTexture"; break;
      case wgpu::FeatureName::DawnPartialLoadResolveTexture: s = "DawnPartialLoadResolveTexture"; break;
      case wgpu::FeatureName::MultiDrawIndirect: s = "MultiDrawIndirect"; break;
      case wgpu::FeatureName::DawnTexelCopyBufferRowAlignment: s = "DawnTexelCopyBufferRowAlignment"; break;
      case wgpu::FeatureName::FlexibleTextureViews: s = "FlexibleTextureViews"; break;
      case wgpu::FeatureName::ChromiumExperimentalSubgroupMatrix: s = "ChromiumExperimentalSubgroupMatrix"; break;
      case wgpu::FeatureName::SharedFenceEGLSync: s = "SharedFenceEGLSync"; break;
      case wgpu::FeatureName::DawnDeviceAllocatorControl: s = "DawnDeviceAllocatorControl"; break;
      case wgpu::FeatureName::AdapterPropertiesWGPU: s = "AdapterPropertiesWGPU"; break;
      case wgpu::FeatureName::SharedBufferMemoryD3D12SharedMemoryFileMappingHandle: s = "SharedBufferMemoryD3D12SharedMemoryFileMappingHandle"; break;
      case wgpu::FeatureName::SharedTextureMemoryD3D12Resource: s = "SharedTextureMemoryD3D12Resource"; break;
      case wgpu::FeatureName::ChromiumExperimentalSamplingResourceTable: s = "ChromiumExperimentalSamplingResourceTable"; break;
      case wgpu::FeatureName::SubgroupSizeControl: s = "SubgroupSizeControl"; break;
      case wgpu::FeatureName::AtomicVec2uMinMax: s = "AtomicVec2uMinMax"; break;
      case wgpu::FeatureName::Unorm16FormatsForExternalTexture: s = "Unorm16FormatsForExternalTexture"; break;
      case wgpu::FeatureName::OpaqueYCbCrAndroidForExternalTexture: s = "OpaqueYCbCrAndroidForExternalTexture"; break;
      case wgpu::FeatureName::Unorm16Filterable: s = "Unorm16Filterable"; break;
      case wgpu::FeatureName::RenderPassRenderArea: s = "RenderPassRenderArea"; break;
      case wgpu::FeatureName::AdapterPropertiesDrm: s = "AdapterPropertiesDrm"; break;
      default: s = "UnknownFeature"; break;
    }
    return std::formatter<std::string_view>::format(s, ctx);
  }
};
template <>
struct std::formatter<wgpu::AdapterType> : std::formatter<std::string_view> {
  auto format(wgpu::AdapterType v, std::format_context& ctx) const {
    std::string_view s;
    switch (v) {
      case wgpu::AdapterType::DiscreteGPU: s = "DiscreteGPU"; break;
      case wgpu::AdapterType::IntegratedGPU: s = "IntegratedGPU"; break;
      case wgpu::AdapterType::CPU: s = "CPU"; break;
      case wgpu::AdapterType::Unknown: s = "Unknown"; break;
      default:
        assert(false);
        s = "UnknownAdapterType";
        break;
    }
    return std::formatter<std::string_view>::format(s, ctx);
  }
};
template <>
struct std::formatter<wgpu::BackendType> : std::formatter<std::string_view> {
  auto format(wgpu::BackendType v, std::format_context& ctx) const {
    std::string_view s;
    switch (v) {
      case wgpu::BackendType::Undefined: s = "Undefined"; break;
      case wgpu::BackendType::Null:      s = "Null"; break;
      case wgpu::BackendType::WebGPU:    s = "WebGPU"; break;
      case wgpu::BackendType::D3D11:     s = "D3D11"; break;
      case wgpu::BackendType::D3D12:     s = "D3D12"; break;
      case wgpu::BackendType::Metal:     s = "Metal"; break;
      case wgpu::BackendType::Vulkan:    s = "Vulkan"; break;
      case wgpu::BackendType::OpenGL:    s = "OpenGL"; break;
      case wgpu::BackendType::OpenGLES:  s = "OpenGLES"; break;
      default:
        assert(false);
        s = "UnknownBackendType";
        break;
    }
    return std::formatter<std::string_view>::format(s, ctx);
  }
};
[[nodiscard]] consteval inline bool UsingEmscripten() noexcept{
  #ifdef __EMSCRIPTEN__ 
    return true;
  #endif
  return false;
}


wgpu::Adapter RequestAdapterSync(wgpu::Instance instance, const wgpu::RequestAdapterOptions *options){
  wgpu::Adapter adapter = nullptr;
  bool request_ended = false;
  wgpu::Future future = instance.RequestAdapter( // Async function!
      options, 
      wgpu::CallbackMode::AllowSpontaneous, 
      [&](wgpu::RequestAdapterStatus status, wgpu::Adapter result_adapter, [[maybe_unused]]wgpu::StringView message) {
          if (status == wgpu::RequestAdapterStatus::Success) {
              adapter = result_adapter;
          }
          request_ended = true;
      }
  );
  #ifdef __EMSCRIPTEN__
    while (!request_ended) {
        emscripten_sleep(100);
    }
  #else
    instance.WaitAny(future, 0);
  #endif
  assert(request_ended);
  return adapter;
}
wgpu::Device RequestDeviceSync(wgpu::Instance instance, wgpu::Adapter adapter, const wgpu::DeviceDescriptor *descriptor){
  wgpu::Device device = nullptr;
  bool request_ended = false;
  wgpu::Future future = adapter.RequestDevice( // Async function!
      descriptor, 
      wgpu::CallbackMode::AllowSpontaneous, 
      [&](wgpu::RequestDeviceStatus status, wgpu::Device result_device, [[maybe_unused]]wgpu::StringView message) {
          if (status == wgpu::RequestDeviceStatus::Success) {
              device = result_device;
          }
          request_ended = true;
      }
  );
  #ifdef __EMSCRIPTEN__
    while (!request_ended) {
        emscripten_sleep(100);
    }
  #else
    instance.WaitAny(future, 0);
  #endif
  assert(request_ended);
  return device;
}
void OutputFeatures(const wgpu::SupportedFeatures &features){
  for (size_t i = 0; i < features.featureCount; ++i){
    auto feature = features.features[i];
    std::print("feature: {}\n", feature);
  }
}
void OutputLimits(const wgpu::Limits &limits){
  std::print("maxTextureDimension1D: {}\n", limits.maxTextureDimension1D);
  std::print("maxTextureDimension2D: {}\n", limits.maxTextureDimension2D);
  std::print("maxTextureDimension3D: {}\n", limits.maxTextureDimension3D);
  std::print("maxTextureArrayLayers: {}\n", limits.maxTextureArrayLayers);
}


void inspectDevice(wgpu::Device device){
  //Features
  std::print("Device features: \n");
  wgpu::SupportedFeatures features;
  device.GetFeatures(&features);
  OutputFeatures(features);
  //Limits
  std::print("Device limits:\n");
  wgpu::Limits limits = {};
  limits.nextInChain = nullptr;
  bool success = device.GetLimits(&limits) == wgpu::Status::Success;
  if (success) {
    OutputLimits(limits);
  }
}

void inspectAdapter(const wgpu::Adapter &adapter){
  //Limits
  std::print("Adapter limits:\n");
  wgpu::Limits limits = {};
  limits.nextInChain = nullptr;
  bool success = adapter.GetLimits(&limits) == wgpu::Status::Success;
  if (success) {
    OutputLimits(limits);
  }
  //Features
  std::print("Adapter features:\n");
  wgpu::SupportedFeatures features;
  adapter.GetFeatures(&features);
  OutputFeatures(features);
  //Properties
  std::print("Adapter properties:\n");
  wgpu::AdapterInfo info = {};
  adapter.GetInfo(&info);
  std::print("vendorID: {}\n", info.vendorID);
  std::print("vendor: {}\n", info.vendor);
  std::print("architecture: {}\n", info.architecture);
  std::print("deviceID: {}\n", info.deviceID);
  std::print("device: {}\n", info.device);
  std::print("description: {}\n", info.description);
  std::print("subgroup minimum size: {}\n", info.subgroupMinSize);
  std::print("subgroup maximum size: {}\n", info.subgroupMaxSize);
  std::print("adapter type: {}\n", info.adapterType);
  std::print("backend type: {}\n", info.backendType);
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char*argv[]){
  std::cout << "Using Emscripten: " << std::boolalpha <<  UsingEmscripten() << std::noboolalpha << std::endl;
  wgpu::InstanceDescriptor desc = {
    .nextInChain = nullptr
  };
  wgpu::Instance instance = wgpu::CreateInstance(&desc);
  if (!instance){
    std::cerr << "Could not initalise WebGPU!" << std::endl;
    return 1;
  }
  else std::print("WGPUInstance created\n");
  wgpu::RequestAdapterOptions adapter_options = {
    .nextInChain = nullptr
  };
  wgpu::Adapter adapter = RequestAdapterSync(instance, &adapter_options);
  inspectAdapter(adapter);
  if (adapter) std::print("Adapter created successfully\n");
  wgpu::DeviceDescriptor device_descriptor = {};
  wgpu::Device device = RequestDeviceSync(instance, adapter, &device_descriptor);
  if (device) std::print("Device created successfully\n");
  device_descriptor.nextInChain = nullptr;
  device_descriptor.label = WGPUStringView{.data = "TestDevice",.length = WGPU_STRLEN};
  device_descriptor.requiredFeatureCount = 0;
  device_descriptor.requiredLimits = nullptr;
  device_descriptor.defaultQueue.nextInChain = nullptr;
  device_descriptor.defaultQueue.label = WGPUStringView{.data = "DefaultQueue",.length = WGPU_STRLEN};
  device_descriptor.SetDeviceLostCallback(wgpu::CallbackMode::AllowSpontaneous, []([[maybe_unused]]wgpu::Device const& device, [[maybe_unused]]wgpu::DeviceLostReason reason, [[maybe_unused]]wgpu::StringView message){
    std::print("Device lost");
  });
    //reason: {}", reason);
    //if (message.data) std::print("({})", message);
    //std::cout << std::endl;
  
  
  inspectDevice(device);
  device.Destroy();
  std::print("device destroyed\n");
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
