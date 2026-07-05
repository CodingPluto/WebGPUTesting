Hello future Chris!

Prequesites for this repository:
- clang that supports C++23
- cmake
- emsdk (if you want to build for web) (make sure added to path)
- vcpkg (make sure added to path)
- ninja


To initalise the project and install dawn:
vcpkg install


Web builds:
if web builds are seemingly not updating, use
CMD + SHIFT + R to force the browser to clear the cache and
update the page.


Change .clangd to whatever platform you are targeting.
e.g. If I am targeting mac debug, change to:


CompileFlags:
  CompilationDatabase: build/native/macos/debug



src/main.cpp
Purpose: The ultimate orchestrator. It instantiates the App, the GPU Context, and the EnTT registry, wiring them together.
src/App.hpp & src/App.cpp
Purpose: OS-level concerns. Manages the GLFW window, input polling, Emscripten loop integration, and calculating delta_time. It knows nothing about WebGPU.
src/GPUContext.hpp & src/GPUContext.cpp
Purpose: The WebGPU . Holds your wgpu::Device, wgpu::Queue, and wgpu::Surface. Contains the initialization callbacks (Adapter -> Device -> Surface).
src/AssetLoader.hpp & src/AssetLoader.cpp
Purpose: Free functions only. std::string LoadWGSL(path) and bool LoadGeometry(path, out_vertices, out_indices). Keeps file I/O out of your rendering logic.
src/BoidsSystem.hpp & src/BoidsSystem.cpp
Purpose: The heart of the simulation. This is where your compute shader will integrate the first-order linear equations for the boids' velocity and position.
Contents: Owns the wgpu::ComputePipeline, wgpu::RenderPipeline, and the SSBO buffers (your custom allocator / 10A choice). It has an Init(GPUContext&, entt::registry&), an Update(delta_time), and a Draw(wgpu::RenderPassEncoder&).