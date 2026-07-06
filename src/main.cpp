#include "GPUContext.hpp"
#include <glm/fwd.hpp>
#include <cassert>
#include <spdlog/logger.h>
#include <string>

#include <webgpu/webgpu.h>
#include <webgpu/webgpu_cpp.h>
#include <glm/glm.hpp>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#include <GLFW/glfw3.h>
#endif
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "App.hpp"


#ifdef _WIN32
#include <windows.h>
#elif __APPLE__
#include <mach-o/dyld.h>
#else
#include <unistd.h>
#endif

void FixWorkingDirectory() {
    std::filesystem::path exePath;

#ifdef _WIN32
    wchar_t buffer[MAX_PATH];
    GetModuleFileNameW(NULL, buffer, MAX_PATH);
    exePath = std::filesystem::path(buffer);
#elif __APPLE__
    char buffer[PATH_MAX];
    uint32_t size = sizeof(buffer);
    if (_NSGetExecutablePath(buffer, &size) == 0) {
        exePath = std::filesystem::path(buffer);
    }
#else // Linux
    char buffer[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", buffer, PATH_MAX);
    if (count != -1) {
        exePath = std::filesystem::path(std::string(buffer, count));
    }
#endif

    // Get the directory containing the executable
    if (!exePath.empty()) {
        std::filesystem::path exeDir = exePath.parent_path();
        // Change the current working directory to the executable's folder
        std::filesystem::current_path(exeDir);
    }
}






void Initialize([[maybe_unused]]App &app){}
void Shutdown([[maybe_unused]]App &app){}
void Update([[maybe_unused]]App &app){}
#ifdef __EMSCRIPTEN__
struct EmscriptenArgs{
  App &app;
  GPUContext &gpu;
};
void EmscriptenLoop(void* arg) {
  EmscriptenArgs* emscripten_args = static_cast<EmscriptenArgs*>(arg);
  App &app = emscripten_args->app;
  GPUContext &gpu = emscripten_args->gpu;
  app.Update();
  gpu.Update(app.GetDeltaTime(), app.GetTotalTimeElapsed());
  if (!app.IsRunning()) {
      emscripten_cancel_main_loop();
      app.Shutdown();
  }
}
#endif
int main([[maybe_unused]] int argc, [[maybe_unused]] char*argv[]){
  #ifdef _WIN32
    LoadLibraryA("d3dcompiler_47.dll");
  #endif
  FixWorkingDirectory();
  App app = {};
  GPUContext gpu = {};
  app.Initalize(1280, 720, "WebGPU Boids");
  gpu.InitializeInstance();
  gpu.InitializeSurface(app.GetWindow());
  #ifdef __EMSCRIPTEN__
    EmscriptenArgs emscripten_args = {.app = app, .gpu = gpu};
    emscripten_set_main_loop_arg(EmscriptenLoop, &emscripten_args, 0, true);
  #else
    while (app.IsRunning()){
      app.Update();
      gpu.Update(app.GetDeltaTime(), app.GetTotalTimeElapsed());
    }
  app.Shutdown();
  #endif
}

/*
  app.gpu.surface = wgpu::Surface::Acquire(glfwCreateWindowWGPUSurface(app.gpu.instance.Get(), app.window));
  if (!app.gpu.surface){
    spdlog::critical("Failed to create a surface for the window");
    assert(false);
  }
*/


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


  // wgpu::CommandEncoderDescriptor encoder_descriptor = {.nextInChain = nullptr, .label = "My command encoder"};
  // wgpu::CommandEncoder encoder = device.CreateCommandEncoder(&encoder_descriptor);
  // encoder.InsertDebugMarker("do one thing");
  // wgpu::CommandBufferDescriptor command_buffer_descriptor = {.nextInChain = nullptr, .label = "My command buffer"};
  // wgpu::CommandBuffer command = encoder.Finish(&command_buffer_descriptor);
  // std::print("Submitting command..\n");
  // queue.Submit(1, &command);
  // std::print("Command submitted\n");
  //   queue.OnSubmittedWorkDone(wgpu::CallbackMode::AllowSpontaneous,[](wgpu::QueueWorkDoneStatus status, wgpu::StringView message){
  //   std::print("queue work finished. Status: {} ; Message: {}\n", status, message);
  // });












// void PlayingWithBuffers(GPUContext &ctx){
//   wgpu::BufferDescriptor buffer_descriptor = {
//     .label = "Some GPU-side data buffer",
//     .usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::CopySrc,
//     .size = 16,
//     .mappedAtCreation = false
//   };
//   wgpu::Buffer buffer_1 = ctx.device.CreateBuffer(&buffer_descriptor);
//   buffer_descriptor.label = "Output Buffer";
//   buffer_descriptor.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::MapRead;
//   ctx.buffer_2 = ctx.device.CreateBuffer(&buffer_descriptor);
//   std::vector<uint8_t> numbers(16);
//   for (uint8_t i = 0; i < 16; ++i) numbers[i] = i;
//   ctx.queue.WriteBuffer(buffer_1, 0, numbers.data(), numbers.size());
//   wgpu::CommandEncoder encoder = ctx.device.CreateCommandEncoder(); // might need Default
//   encoder.CopyBufferToBuffer(buffer_1, 0, ctx.buffer_2, 0, 16);
//   wgpu::CommandBuffer command_copy = encoder.Finish();
//   ctx.queue.Submit(1, &command_copy);
//   ctx.queue.OnSubmittedWorkDone(GetPlatformCallbackMode(),[&ctx](wgpu::QueueWorkDoneStatus status, wgpu::StringView message){
//     spdlog::info("Copied data from buffer from 1 to 2?: status: [{}], message '{}'" , status, message);
//     ctx.buffer_2.MapAsync(wgpu::MapMode::Read, 0, 16, GetPlatformCallbackMode(), [&ctx]([[maybe_unused]]wgpu::MapAsyncStatus status, [[maybe_unused]]wgpu::StringView message){
//       spdlog::info("Buffer 2 mapped with status: {}", status);
//       uint8_t* buffer_data = (uint8_t*)ctx.buffer_2.GetConstMappedRange(0,16);
//       std::string str = "Buffer: [";
//       for (int i = 0; i < 16; ++i){
//         str += std::to_string(buffer_data[i]) + ", ";
//       }
//       str += "]";
//       spdlog::info(str);
//       ctx.buffer_2.Unmap();
//     });
//   });
// }






// Learning about the GPU
// a buffer is a chunk of memory allocated in VRAM.
// WriteBuffer copies the CPU side of the memory during transfer to its own location, that then it is put from there onto the GPU.
// Can be disabled with mapping.



