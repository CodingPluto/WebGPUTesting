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
[[nodiscard]] constexpr std::string_view format_as(InitializationState state) noexcept {
  switch (state) {
    case InitializationState::Uninitalised:             return "Uninitalised";
    case InitializationState::RequestingAdapter:        return "RequestingAdapter";
    case InitializationState::RequestingDevice:         return "RequestingDevice";
    case InitializationState::ReceivedAdapterAndDevice: return "ReceivedAdapterAndDevice";
    case InitializationState::Ready:                    return "Ready";
    case InitializationState::Failed:                   return "Failed";
  }
  return "Unknown";
}

namespace wgpu {

    [[nodiscard]] constexpr std::string_view format_as(RequestAdapterStatus status) noexcept {
        switch (status) {
            case RequestAdapterStatus::Success:           return "Success";
            case RequestAdapterStatus::CallbackCancelled: return "CallbackCancelled";
            case RequestAdapterStatus::Unavailable:       return "Unavailable";
            case RequestAdapterStatus::Error:             return "Error";
            default:                                      return "UnknownRequestAdapterStatus";
        }
    }

    [[nodiscard]] constexpr std::string_view format_as(QueueWorkDoneStatus status) noexcept {
        switch (status) {
            case QueueWorkDoneStatus::Success:           return "Success";
            case QueueWorkDoneStatus::CallbackCancelled: return "CallbackCancelled";
            case QueueWorkDoneStatus::Error:             return "Error";
            default:                                     return "UnknownQueueWorkDoneStatus";
        }
    }

    [[nodiscard]] constexpr std::string_view format_as(ErrorType type) noexcept {
        switch (type) {
            case ErrorType::NoError:     return "NoError";
            case ErrorType::Validation:  return "Validation";
            case ErrorType::OutOfMemory: return "OutOfMemory";
            case ErrorType::Internal:    return "Internal";
            case ErrorType::Unknown:     return "Unknown";
            default:                     return "UnknownErrorType";
        }
    }

    [[nodiscard]] constexpr std::string_view format_as(DeviceLostReason v) noexcept {
        switch (v) {
            case DeviceLostReason::Unknown:           return "Unknown";
            case DeviceLostReason::Destroyed:         return "Destroyed";
            case DeviceLostReason::CallbackCancelled: return "CallbackCancelled";
            case DeviceLostReason::FailedCreation:    return "FailedCreation";
            default:                                  return "UnknownDeviceLostReason";
        }
    }

    [[nodiscard]] constexpr std::string_view format_as(FeatureName v) noexcept {
        switch (v) {
            case FeatureName::CoreFeaturesAndLimits:             return "CoreFeaturesAndLimits";
            case FeatureName::DepthClipControl:                  return "DepthClipControl";
            case FeatureName::Depth32FloatStencil8:              return "Depth32FloatStencil8";
            case FeatureName::TextureCompressionBC:              return "TextureCompressionBC";
            case FeatureName::TextureCompressionBCSliced3D:       return "TextureCompressionBCSliced3D";
            case FeatureName::TextureCompressionETC2:            return "TextureCompressionETC2";
            case FeatureName::TextureCompressionASTC:            return "TextureCompressionASTC";
            case FeatureName::TextureCompressionASTCSliced3D:     return "TextureCompressionASTCSliced3D";
            case FeatureName::TimestampQuery:                    return "TimestampQuery";
            case FeatureName::IndirectFirstInstance:             return "IndirectFirstInstance";
            case FeatureName::ShaderF16:                         return "ShaderF16";
            case FeatureName::RG11B10UfloatRenderable:           return "RG11B10UfloatRenderable";
            case FeatureName::BGRA8UnormStorage:                 return "BGRA8UnormStorage";
            case FeatureName::Float32Filterable:                 return "Float32Filterable";
            case FeatureName::Float32Blendable:                  return "Float32Blendable";
            case FeatureName::ClipDistances:                     return "ClipDistances";
            case FeatureName::DualSourceBlending:                return "DualSourceBlending";
            case FeatureName::Subgroups:                         return "Subgroups";
            case FeatureName::TextureFormatsTier1:               return "TextureFormatsTier1";
            case FeatureName::TextureFormatsTier2:               return "TextureFormatsTier2";
            case FeatureName::PrimitiveIndex:                    return "PrimitiveIndex";
            #ifndef __EMSCRIPTEN__
            case FeatureName::TextureComponentSwizzle:           return "TextureComponentSwizzle";
            case FeatureName::DawnInternalUsages:                                return "DawnInternalUsages";
            case FeatureName::DawnMultiPlanarFormats:                            return "DawnMultiPlanarFormats";
            case FeatureName::DawnNative:                                        return "DawnNative";
            case FeatureName::ChromiumExperimentalTimestampQueryInsidePasses:    return "ChromiumExperimentalTimestampQueryInsidePasses";
            case FeatureName::ImplicitDeviceSynchronization:                     return "ImplicitDeviceSynchronization";
            case FeatureName::TransientAttachments:                              return "TransientAttachments";
            case FeatureName::MSAARenderToSingleSampled:                         return "MSAARenderToSingleSampled";
            case FeatureName::D3D11MultithreadProtected:                         return "D3D11MultithreadProtected";
            case FeatureName::ANGLETextureSharing:                               return "ANGLETextureSharing";
            case FeatureName::PixelLocalStorageCoherent:                         return "PixelLocalStorageCoherent";
            case FeatureName::PixelLocalStorageNonCoherent:                      return "PixelLocalStorageNonCoherent";
            case FeatureName::Unorm16TextureFormats:                             return "Unorm16TextureFormats";
            case FeatureName::MultiPlanarFormatExtendedUsages:                   return "MultiPlanarFormatExtendedUsages";
            case FeatureName::MultiPlanarFormatP010:                             return "MultiPlanarFormatP010";
            case FeatureName::HostMappedPointer:                                 return "HostMappedPointer";
            case FeatureName::MultiPlanarRenderTargets:                          return "MultiPlanarRenderTargets";
            case FeatureName::MultiPlanarFormatNv12a:                            return "MultiPlanarFormatNv12a";
            case FeatureName::FramebufferFetch:                                  return "FramebufferFetch";
            case FeatureName::BufferMapExtendedUsages:                           return "BufferMapExtendedUsages";
            case FeatureName::AdapterPropertiesMemoryHeaps:                      return "AdapterPropertiesMemoryHeaps";
            case FeatureName::AdapterPropertiesD3D:                              return "AdapterPropertiesD3D";
            case FeatureName::AdapterPropertiesVk:                               return "AdapterPropertiesVk";
            case FeatureName::DawnFormatCapabilities:                            return "DawnFormatCapabilities";
            case FeatureName::DawnDrmFormatCapabilities:                         return "DawnDrmFormatCapabilities";
            case FeatureName::MultiPlanarFormatNv16:                             return "MultiPlanarFormatNv16";
            case FeatureName::MultiPlanarFormatNv24:                             return "MultiPlanarFormatNv24";
            case FeatureName::MultiPlanarFormatP210:                             return "MultiPlanarFormatP210";
            case FeatureName::MultiPlanarFormatP410:                             return "MultiPlanarFormatP410";
            case FeatureName::SharedTextureMemoryVkDedicatedAllocation:          return "SharedTextureMemoryVkDedicatedAllocation";
            case FeatureName::SharedTextureMemoryAHardwareBuffer:                return "SharedTextureMemoryAHardwareBuffer";
            case FeatureName::SharedTextureMemoryDmaBuf:                         return "SharedTextureMemoryDmaBuf";
            case FeatureName::SharedTextureMemoryOpaqueFD:                       return "SharedTextureMemoryOpaqueFD";
            case FeatureName::SharedTextureMemoryZirconHandle:                   return "SharedTextureMemoryZirconHandle";
            case FeatureName::SharedTextureMemoryDXGISharedHandle:               return "SharedTextureMemoryDXGISharedHandle";
            case FeatureName::SharedTextureMemoryD3D11Texture2D:                 return "SharedTextureMemoryD3D11Texture2D";
            case FeatureName::SharedTextureMemoryIOSurface:                       return "SharedTextureMemoryIOSurface";
            case FeatureName::SharedTextureMemoryEGLImage:                       return "SharedTextureMemoryEGLImage";
            case FeatureName::SharedFenceVkSemaphoreOpaqueFD:                    return "SharedFenceVkSemaphoreOpaqueFD";
            case FeatureName::SharedFenceSyncFD:                                 return "SharedFenceSyncFD";
            case FeatureName::SharedFenceVkSemaphoreZirconHandle:                 return "SharedFenceVkSemaphoreZirconHandle";
            case FeatureName::SharedFenceDXGISharedHandle:                       return "SharedFenceDXGISharedHandle";
            case FeatureName::SharedFenceMTLSharedEvent:                         return "SharedFenceMTLSharedEvent";
            case FeatureName::SharedBufferMemoryD3D12Resource:                   return "SharedBufferMemoryD3D12Resource";
            case FeatureName::StaticSamplers:                                    return "StaticSamplers";
            case FeatureName::YCbCrVulkanSamplers:                               return "YCbCrVulkanSamplers";
            case FeatureName::ShaderModuleCompilationOptions:                    return "ShaderModuleCompilationOptions";
            case FeatureName::DawnLoadResolveTexture:                            return "DawnLoadResolveTexture";
            case FeatureName::DawnPartialLoadResolveTexture:                     return "DawnPartialLoadResolveTexture";
            case FeatureName::MultiDrawIndirect:                                 return "MultiDrawIndirect";
            case FeatureName::DawnTexelCopyBufferRowAlignment:                   return "DawnTexelCopyBufferRowAlignment";
            case FeatureName::FlexibleTextureViews:                              return "FlexibleTextureViews";
            case FeatureName::ChromiumExperimentalSubgroupMatrix:                return "ChromiumExperimentalSubgroupMatrix";
            case FeatureName::SharedFenceEGLSync:                                return "SharedFenceEGLSync";
            case FeatureName::DawnDeviceAllocatorControl:                        return "DawnDeviceAllocatorControl";
            case FeatureName::AdapterPropertiesWGPU:                             return "AdapterPropertiesWGPU";
            case FeatureName::SharedBufferMemoryD3D12SharedMemoryFileMappingHandle: return "SharedBufferMemoryD3D12SharedMemoryFileMappingHandle";
            case FeatureName::SharedTextureMemoryD3D12Resource:                   return "SharedTextureMemoryD3D12Resource";
            case FeatureName::ChromiumExperimentalSamplingResourceTable:         return "ChromiumExperimentalSamplingResourceTable";
            case FeatureName::SubgroupSizeControl:                               return "SubgroupSizeControl";
            case FeatureName::AtomicVec2uMinMax:                                 return "AtomicVec2uMinMax";
            case FeatureName::Unorm16FormatsForExternalTexture:                  return "Unorm16FormatsForExternalTexture";
            case FeatureName::OpaqueYCbCrAndroidForExternalTexture:              return "OpaqueYCbCrAndroidForExternalTexture";
            case FeatureName::Unorm16Filterable:                                 return "Unorm16Filterable";
            case FeatureName::RenderPassRenderArea:                              return "RenderPassRenderArea";
            case FeatureName::AdapterPropertiesDrm:                              return "AdapterPropertiesDrm";
            #endif
            default:                                                             return "UnknownFeature";
        }
    }

    // Note: Since format_as functions are ideally constexpr, if you want to keep your asserts, 
    // it's best to handle them directly. (C++14 and up allows asserts in constexpr functions)
    inline std::string_view format_as(AdapterType v) {
        switch (v) {
            case AdapterType::DiscreteGPU:   return "DiscreteGPU";
            case AdapterType::IntegratedGPU: return "IntegratedGPU";
            case AdapterType::CPU:           return "CPU";
            case AdapterType::Unknown:       return "Unknown";
            default:
                assert(false);
                return "UnknownAdapterType";
        }
    }

    inline std::string_view format_as(BackendType v) {
        switch (v) {
            case BackendType::Undefined: return "Undefined";
            case BackendType::Null:      return "Null";
            case BackendType::WebGPU:    return "WebGPU";
            case BackendType::D3D11:     return "D3D11";
            case BackendType::D3D12:     return "D3D12";
            case BackendType::Metal:     return "Metal";
            case BackendType::Vulkan:    return "Vulkan";
            case BackendType::OpenGL:    return "OpenGL";
            case BackendType::OpenGLES:  return "OpenGLES";
            default:
                assert(false);
                return "UnknownBackendType";
        }
    }

    [[nodiscard]] constexpr std::string_view format_as(MapAsyncStatus status) noexcept {
        switch (status) {
            case MapAsyncStatus::Success:           return "Success";
            case MapAsyncStatus::CallbackCancelled: return "CallbackCancelled";
            case MapAsyncStatus::Error:             return "Error";
            case MapAsyncStatus::Aborted:           return "Aborted";
            default:                                return "UnknownMapAsyncStatus";
        }
    }

} // namespace wgpu


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