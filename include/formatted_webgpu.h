#pragma once
#include <spdlog/fmt/fmt.h> 
#include <fmt/format.h>
#include <string_view>
#include <cassert>


enum class InitializationState{
  Uninitalised,
  RequestingAdapter,
  RequestingDevice,
  ReceivedAdapterAndDevice,
  Ready,
  Failed
};

namespace wgpu {
  constexpr std::string_view format_as(TextureFormat f) {
    switch (f) {
      case TextureFormat::Undefined: return "Undefined";
      case TextureFormat::R8Unorm: return "R8Unorm";
      case TextureFormat::R8Snorm: return "R8Snorm";
      case TextureFormat::R8Uint: return "R8Uint";
      case TextureFormat::R8Sint: return "R8Sint";
      case TextureFormat::R16Unorm: return "R16Unorm";
      case TextureFormat::R16Snorm: return "R16Snorm";
      case TextureFormat::R16Uint: return "R16Uint";
      case TextureFormat::R16Sint: return "R16Sint";
      case TextureFormat::R16Float: return "R16Float";
      case TextureFormat::RG8Unorm: return "RG8Unorm";
      case TextureFormat::RG8Snorm: return "RG8Snorm";
      case TextureFormat::RG8Uint: return "RG8Uint";
      case TextureFormat::RG8Sint: return "RG8Sint";
      case TextureFormat::R32Float: return "R32Float";
      case TextureFormat::R32Uint: return "R32Uint";
      case TextureFormat::R32Sint: return "R32Sint";
      case TextureFormat::RG16Unorm: return "RG16Unorm";
      case TextureFormat::RG16Snorm: return "RG16Snorm";
      case TextureFormat::RG16Uint: return "RG16Uint";
      case TextureFormat::RG16Sint: return "RG16Sint";
      case TextureFormat::RG16Float: return "RG16Float";
      case TextureFormat::RGBA8Unorm: return "RGBA8Unorm";
      case TextureFormat::RGBA8UnormSrgb: return "RGBA8UnormSrgb";
      case TextureFormat::RGBA8Snorm: return "RGBA8Snorm";
      case TextureFormat::RGBA8Uint: return "RGBA8Uint";
      case TextureFormat::RGBA8Sint: return "RGBA8Sint";
      case TextureFormat::BGRA8Unorm: return "BGRA8Unorm";
      case TextureFormat::BGRA8UnormSrgb: return "BGRA8UnormSrgb";
      case TextureFormat::RGB10A2Uint: return "RGB10A2Uint";
      case TextureFormat::RGB10A2Unorm: return "RGB10A2Unorm";
      case TextureFormat::RG11B10Ufloat: return "RG11B10Ufloat";
      case TextureFormat::RGB9E5Ufloat: return "RGB9E5Ufloat";
      case TextureFormat::RG32Float: return "RG32Float";
      case TextureFormat::RG32Uint: return "RG32Uint";
      case TextureFormat::RG32Sint: return "RG32Sint";
      case TextureFormat::RGBA16Unorm: return "RGBA16Unorm";
      case TextureFormat::RGBA16Snorm: return "RGBA16Snorm";
      case TextureFormat::RGBA16Uint: return "RGBA16Uint";
      case TextureFormat::RGBA16Sint: return "RGBA16Sint";
      case TextureFormat::RGBA16Float: return "RGBA16Float";
      case TextureFormat::RGBA32Float: return "RGBA32Float";
      case TextureFormat::RGBA32Uint: return "RGBA32Uint";
      case TextureFormat::RGBA32Sint: return "RGBA32Sint";
      case TextureFormat::Stencil8: return "Stencil8";
      case TextureFormat::Depth16Unorm: return "Depth16Unorm";
      case TextureFormat::Depth24Plus: return "Depth24Plus";
      case TextureFormat::Depth24PlusStencil8: return "Depth24PlusStencil8";
      case TextureFormat::Depth32Float: return "Depth32Float";
      case TextureFormat::Depth32FloatStencil8: return "Depth32FloatStencil8";
      case TextureFormat::BC1RGBAUnorm: return "BC1RGBAUnorm";
      case TextureFormat::BC1RGBAUnormSrgb: return "BC1RGBAUnormSrgb";
      case TextureFormat::BC2RGBAUnorm: return "BC2RGBAUnorm";
      case TextureFormat::BC2RGBAUnormSrgb: return "BC2RGBAUnormSrgb";
      case TextureFormat::BC3RGBAUnorm: return "BC3RGBAUnorm";
      case TextureFormat::BC3RGBAUnormSrgb: return "BC3RGBAUnormSrgb";
      case TextureFormat::BC4RUnorm: return "BC4RUnorm";
      case TextureFormat::BC4RSnorm: return "BC4RSnorm";
      case TextureFormat::BC5RGUnorm: return "BC5RGUnorm";
      case TextureFormat::BC5RGSnorm: return "BC5RGSnorm";
      case TextureFormat::BC6HRGBUfloat: return "BC6HRGBUfloat";
      case TextureFormat::BC6HRGBFloat: return "BC6HRGBFloat";
      case TextureFormat::BC7RGBAUnorm: return "BC7RGBAUnorm";
      case TextureFormat::BC7RGBAUnormSrgb: return "BC7RGBAUnormSrgb";
      case TextureFormat::ETC2RGB8Unorm: return "ETC2RGB8Unorm";
      case TextureFormat::ETC2RGB8UnormSrgb: return "ETC2RGB8UnormSrgb";
      case TextureFormat::ETC2RGB8A1Unorm: return "ETC2RGB8A1Unorm";
      case TextureFormat::ETC2RGB8A1UnormSrgb: return "ETC2RGB8A1UnormSrgb";
      case TextureFormat::ETC2RGBA8Unorm: return "ETC2RGBA8Unorm";
      case TextureFormat::ETC2RGBA8UnormSrgb: return "ETC2RGBA8UnormSrgb";
      case TextureFormat::EACR11Unorm: return "EACR11Unorm";
      case TextureFormat::EACR11Snorm: return "EACR11Snorm";
      case TextureFormat::EACRG11Unorm: return "EACRG11Unorm";
      case TextureFormat::EACRG11Snorm: return "EACRG11Snorm";
      case TextureFormat::ASTC4x4Unorm: return "ASTC4x4Unorm";
      case TextureFormat::ASTC4x4UnormSrgb: return "ASTC4x4UnormSrgb";
      case TextureFormat::ASTC5x4Unorm: return "ASTC5x4Unorm";
      case TextureFormat::ASTC5x4UnormSrgb: return "ASTC5x4UnormSrgb";
      case TextureFormat::ASTC5x5Unorm: return "ASTC5x5Unorm";
      case TextureFormat::ASTC5x5UnormSrgb: return "ASTC5x5UnormSrgb";
      case TextureFormat::ASTC6x5Unorm: return "ASTC6x5Unorm";
      case TextureFormat::ASTC6x5UnormSrgb: return "ASTC6x5UnormSrgb";
      case TextureFormat::ASTC6x6Unorm: return "ASTC6x6Unorm";
      case TextureFormat::ASTC6x6UnormSrgb: return "ASTC6x6UnormSrgb";
      case TextureFormat::ASTC8x5Unorm: return "ASTC8x5Unorm";
      case TextureFormat::ASTC8x5UnormSrgb: return "ASTC8x5UnormSrgb";
      case TextureFormat::ASTC8x6Unorm: return "ASTC8x6Unorm";
      case TextureFormat::ASTC8x6UnormSrgb: return "ASTC8x6UnormSrgb";
      case TextureFormat::ASTC8x8Unorm: return "ASTC8x8Unorm";
      case TextureFormat::ASTC8x8UnormSrgb: return "ASTC8x8UnormSrgb";
      case TextureFormat::ASTC10x5Unorm: return "ASTC10x5Unorm";
      case TextureFormat::ASTC10x5UnormSrgb: return "ASTC10x5UnormSrgb";
      case TextureFormat::ASTC10x6Unorm: return "ASTC10x6Unorm";
      case TextureFormat::ASTC10x6UnormSrgb: return "ASTC10x6UnormSrgb";
      case TextureFormat::ASTC10x8Unorm: return "ASTC10x8Unorm";
      case TextureFormat::ASTC10x8UnormSrgb: return "ASTC10x8UnormSrgb";
      case TextureFormat::ASTC10x10Unorm: return "ASTC10x10Unorm";
      case TextureFormat::ASTC10x10UnormSrgb: return "ASTC10x10UnormSrgb";
      case TextureFormat::ASTC12x10Unorm: return "ASTC12x10Unorm";
      case TextureFormat::ASTC12x10UnormSrgb: return "ASTC12x10UnormSrgb";
      case TextureFormat::ASTC12x12Unorm: return "ASTC12x12Unorm";
      case TextureFormat::ASTC12x12UnormSrgb: return "ASTC12x12UnormSrgb";
      #ifndef __EMSCRIPTEN__
      case TextureFormat::R8BG8Biplanar420Unorm: return "R8BG8Biplanar420Unorm";
      case TextureFormat::R10X6BG10X6Biplanar420Unorm: return "R10X6BG10X6Biplanar420Unorm";
      case TextureFormat::R8BG8A8Triplanar420Unorm: return "R8BG8A8Triplanar420Unorm";
      case TextureFormat::R8BG8Biplanar422Unorm: return "R8BG8Biplanar422Unorm";
      case TextureFormat::R8BG8Biplanar444Unorm: return "R8BG8Biplanar444Unorm";
      case TextureFormat::R10X6BG10X6Biplanar422Unorm: return "R10X6BG10X6Biplanar422Unorm";
      case TextureFormat::R10X6BG10X6Biplanar444Unorm: return "R10X6BG10X6Biplanar444Unorm";
      case TextureFormat::OpaqueYCbCrAndroid: return "OpaqueYCbCrAndroid";
      #endif
      default: return "Unknown";
    }
  }


}

template <>
struct fmt::formatter<InitializationState> : fmt::formatter<std::string_view> {
    auto format(InitializationState state, fmt::format_context& ctx) const {
        std::string_view name = "Unknown";
        switch (state) {
            case InitializationState::Uninitalised:      name = "Uninitalised"; break;
            case InitializationState::RequestingAdapter: name = "RequestingAdapter"; break;
            case InitializationState::RequestingDevice:  name = "RequestingDevice"; break;
            case InitializationState::ReceivedAdapterAndDevice: name = "ReceivedAdapterAndDevice"; break;
            case InitializationState::Ready:             name = "Ready"; break;
            case InitializationState::Failed:            name = "Failed"; break;
        }
        return fmt::formatter<std::string_view>::format(name, ctx);
    }
};

template <>
struct fmt::formatter<wgpu::RequestAdapterStatus> : fmt::formatter<std::string_view> {
  auto format(wgpu::RequestAdapterStatus status, fmt::format_context& ctx) const {
    std::string_view s;
    switch (status) {
      case wgpu::RequestAdapterStatus::Success:           s = "Success"; break;
      case wgpu::RequestAdapterStatus::CallbackCancelled: s = "CallbackCancelled"; break;
      case wgpu::RequestAdapterStatus::Unavailable:       s = "Unavailable"; break;
      case wgpu::RequestAdapterStatus::Error:             s = "Error"; break;
      default:                                            s = "UnknownRequestAdapterStatus"; break;
    }
    return fmt::formatter<std::string_view>::format(s, ctx);
  }
};

template <>
struct fmt::formatter<wgpu::StringView> : fmt::formatter<std::string_view> {
  auto format(const wgpu::StringView &wgpu_str, fmt::format_context &ctx) const {
    std::string_view view = {};
    if (wgpu_str.data){
      view = (wgpu_str.length == WGPU_STRLEN) ? std::string_view{wgpu_str.data} : std::string_view{wgpu_str.data, wgpu_str.length};
    }
    return fmt::formatter<std::string_view>::format(view, ctx);
  }
};

template <>
struct fmt::formatter<wgpu::QueueWorkDoneStatus> : fmt::formatter<std::string_view> {
  auto format(wgpu::QueueWorkDoneStatus status, fmt::format_context& ctx) const {
    std::string_view s;
    switch (status) {
      case wgpu::QueueWorkDoneStatus::Success:           s = "Success"; break;
      case wgpu::QueueWorkDoneStatus::CallbackCancelled: s = "CallbackCancelled"; break;
      case wgpu::QueueWorkDoneStatus::Error:             s = "Error"; break;
      default:                                           s = "UnknownQueueWorkDoneStatus"; break;
    }
    return fmt::formatter<std::string_view>::format(s, ctx);
  }
};

template <>
struct fmt::formatter<wgpu::ErrorType> : fmt::formatter<std::string_view> {
  auto format(wgpu::ErrorType type, fmt::format_context& ctx) const {
    std::string_view s;
    switch (type) {
      case wgpu::ErrorType::NoError:     s = "NoError"; break;
      case wgpu::ErrorType::Validation:  s = "Validation"; break;
      case wgpu::ErrorType::OutOfMemory: s = "OutOfMemory"; break;
      case wgpu::ErrorType::Internal:    s = "Internal"; break;
      case wgpu::ErrorType::Unknown:     s = "Unknown"; break;
      default:                           s = "UnknownErrorType"; break;
    }
    return fmt::formatter<std::string_view>::format(s, ctx);
  }
};

template <>
struct fmt::formatter<wgpu::DeviceLostReason> : fmt::formatter<std::string_view> {
  auto format(wgpu::DeviceLostReason v, fmt::format_context& ctx) const {
    std::string_view s;
    switch (v) {
      case wgpu::DeviceLostReason::Unknown: s = "Unknown"; break;
      case wgpu::DeviceLostReason::Destroyed: s = "Destroyed"; break;
      case wgpu::DeviceLostReason::CallbackCancelled: s = "CallbackCancelled"; break;
      case wgpu::DeviceLostReason::FailedCreation: s = "FailedCreation"; break;
      default: s = "UnknownDeviceLostReason"; break;
    }
    return fmt::formatter<std::string_view>::format(s, ctx);
  }
};

template <>
struct fmt::formatter<wgpu::FeatureName> : fmt::formatter<std::string_view> {
  auto format(wgpu::FeatureName v, fmt::format_context& ctx) const {
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
      #ifndef __EMSCRIPTEN__
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
      #endif
      default: s = "UnknownFeature"; break;
    }
    return fmt::formatter<std::string_view>::format(s, ctx);
  }
};

template <>
struct fmt::formatter<wgpu::AdapterType> : fmt::formatter<std::string_view> {
  auto format(wgpu::AdapterType v, fmt::format_context& ctx) const {
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
    return fmt::formatter<std::string_view>::format(s, ctx);
  }
};

template <>
struct fmt::formatter<wgpu::BackendType> : fmt::formatter<std::string_view> {
  auto format(wgpu::BackendType v, fmt::format_context& ctx) const {
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
    return fmt::formatter<std::string_view>::format(s, ctx);
  }
};