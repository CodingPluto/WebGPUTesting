#include <iostream>
#include <cassert>
#define WEBGPU_CPP_IMPLEMENTATION
#ifndef __EMSCRIPTEN__
#include <webgpu/webgpu.h>
#include <vector>
#include <iomanip>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif // __EMSCRIPTEN__


WGPUAdapter requestAdapterSync(WGPUInstance instance, WGPURequestAdapterOptions const *options){
  struct UserData{
    WGPUAdapter adapter = nullptr;
    bool requestEnded = false;
  };
  UserData userData;

  WGPURequestAdapterCallback callback = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, WGPUStringView message, [[maybe_unused]]void* userdata1, [[maybe_unused]]void* userdata2){
      UserData& userData = *reinterpret_cast<UserData*>(userdata1);
      if (status == WGPURequestAdapterStatus_Success){
        userData.adapter = adapter;
      } else {
        std::cerr << "Couldn't get WebGPU adapter:" << message.data << std::endl;
      }
      userData.requestEnded = true;
  };
  WGPURequestAdapterCallbackInfo callbackInfo{
    .nextInChain = nullptr,
    .mode = WGPUCallbackMode_AllowSpontaneous,
    .callback = callback,
    .userdata1 = &userData,
    .userdata2 = nullptr
  };
  wgpuInstanceRequestAdapter(instance, options, callbackInfo);

  #ifdef __EMSCRIPTEN__
      while (!userData.requestEnded) {
          emscripten_sleep(100);
      }
  #endif // __EMSCRIPTEN__

  assert(userData.requestEnded);

  return userData.adapter;

}

std::string_view featureToString(WGPUFeatureName f) {
    switch (f) {
        case WGPUFeatureName_CoreFeaturesAndLimits:
            return "CoreFeaturesAndLimits";

        case WGPUFeatureName_DepthClipControl:
            return "DepthClipControl";

        case WGPUFeatureName_Depth32FloatStencil8:
            return "Depth32FloatStencil8";

        case WGPUFeatureName_TextureCompressionBC:
            return "TextureCompressionBC";

        case WGPUFeatureName_TextureCompressionBCSliced3D:
            return "TextureCompressionBCSliced3D";

        case WGPUFeatureName_TextureCompressionETC2:
            return "TextureCompressionETC2";

        case WGPUFeatureName_TextureCompressionASTC:
            return "TextureCompressionASTC";

        case WGPUFeatureName_TextureCompressionASTCSliced3D:
            return "TextureCompressionASTCSliced3D";

        case WGPUFeatureName_TimestampQuery:
            return "TimestampQuery";

        case WGPUFeatureName_IndirectFirstInstance:
            return "IndirectFirstInstance";

        case WGPUFeatureName_ShaderF16:
            return "ShaderF16";

        case WGPUFeatureName_RG11B10UfloatRenderable:
            return "RG11B10UfloatRenderable";

        case WGPUFeatureName_BGRA8UnormStorage:
            return "BGRA8UnormStorage";

        case WGPUFeatureName_Float32Filterable:
            return "Float32Filterable";

        case WGPUFeatureName_Float32Blendable:
            return "Float32Blendable";

        case WGPUFeatureName_ClipDistances:
            return "ClipDistances";

        case WGPUFeatureName_DualSourceBlending:
            return "DualSourceBlending";

        case WGPUFeatureName_Subgroups:
            return "Subgroups";

        case WGPUFeatureName_TextureFormatsTier1:
            return "TextureFormatsTier1";

        case WGPUFeatureName_TextureFormatsTier2:
            return "TextureFormatsTier2";

        case WGPUFeatureName_PrimitiveIndex:
            return "PrimitiveIndex";

        case WGPUFeatureName_TextureComponentSwizzle:
            return "TextureComponentSwizzle";

        case WGPUFeatureName_Unorm16TextureFormats:
            return "Unorm16TextureFormats";

        case WGPUFeatureName_MultiDrawIndirect:
            return "MultiDrawIndirect";

        default:
            return "UnknownFeature";
    }
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char*argv[]){
  std::cout << "Using Emscripten: " << std::flush;
  #ifdef __EMSCRIPTEN__
    std::cout << "true" << std::flush;
  #else
    std::cout << "false" << std::flush;
  #endif
  std::cout << std::endl;


  WGPUInstanceDescriptor desc = {};
  desc.nextInChain = nullptr;
  #ifdef __EMSCRIPTEN__
    WGPUInstance instance = wgpuCreateInstance(nullptr);
  #else
    WGPUInstance instance = wgpuCreateInstance(&desc);
  #endif
  if (!instance){
    std::cerr << "Could not initalise WebGPU!" << std::endl;
    return 1;
  }
  std::cout << "WGPU instance: " << instance << std::endl;

  WGPURequestAdapterOptions adapterOpts = {};
  adapterOpts.nextInChain = nullptr;
  WGPUAdapter adapter = requestAdapterSync(instance, &adapterOpts);
  std::cout << "Adapter got: " << adapter << std::endl;
  wgpuInstanceRelease(instance);

  // adapter has limits, features, and properties.

  #ifndef __EMSCRIPTEN__
  WGPULimits supportedLimits = {};
  supportedLimits.nextInChain = nullptr;

  bool success = wgpuAdapterGetLimits(adapter, &supportedLimits) == WGPUStatus_Success;

  if (success) {
      std::cout << "Adapter limits:" << std::endl;
      std::cout << " - maxTextureDimension1D: " << supportedLimits.maxTextureDimension1D << std::endl;
      std::cout << " - maxTextureDimension2D: " << supportedLimits.maxTextureDimension2D << std::endl;
      std::cout << " - maxTextureDimension3D: " << supportedLimits.maxTextureDimension3D << std::endl;
      std::cout << " - maxTextureArrayLayers: " << supportedLimits.maxTextureArrayLayers << std::endl;
  }
  #endif // NOT __EMSCRIPTEN__

  WGPUSupportedFeatures features;
  wgpuAdapterGetFeatures(adapter, &features);

  for (size_t i = 0; i < features.featureCount; ++i){
    std::cout << "feature: " << featureToString(*(features.features + i)) <<  " : " << std::hex << std::showbase << std::uppercase << *(features.features + i) << std::endl;
    std::cout << std::nouppercase << std::dec;
  }





  wgpuAdapterRelease(adapter);
}

#endif

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

//cmake -G Ninja -S . -B build
//cmake --build build -- -j$(sysctl -n hw.logicalcpu)
//./build/App
/*
cmake -G Ninja -S . -B build-emscripten \
  -DCMAKE_TOOLCHAIN_FILE=$EMSDK/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake \
  -DCMAKE_BUILD_TYPE=Release \
  -DEMSCRIPTEN=ON*/
// OR emcmake cmake -G Ninja -S . -B build/web/debug -DCMAKE_BUILD_TYPE=Debug

  //cmake --build build/web/debug -- -j$(sysctl -n hw.logicalcpu)

/*
cd "/Users/chris/Documents/Github Repos/WebGPUTesting"
chmod +x build-emscripten/dawn/protoc.js-33.0.0
 */


 /*
 emcmake cmake -B out/wasm -G Ninja \
  -DDAWN_ENABLE_EMSCRIPTEN=ON \
  -DDAWN_BUILD_SAMPLES=OFF \
  -DDAWN_BUILD_TESTS=OFF \
  -DDAWN_ENABLE_DESKTOP_GL=OFF \
  -DDAWN_ENABLE_VULKAN=OFF \
  -DDAWN_ENABLE_METAL=OFF \
  -DDAWN_ENABLE_D3D11=OFF \
  -DDAWN_ENABLE_D3D12=OFF \
  -DDAWN_USE_GLFW=OFF
  */
 