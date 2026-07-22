Hello future Chris!

Prequesites for this repository:
- clang that supports C++23
- cmake
- emsdk (if you want to build for web) (make sure added to path)
- vcpkg (make sure added to path)
- ninja
- pkg-config

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



Next steps:
- Add what was recommended by ChatGPT for passing the render data.
- Finish the boid simulation code.


GPUContext owns rendering and GPU state.
A separate RenderDataPacker/RenderExtractor converts EnTT components into GpuObjectData.
FrameRenderData owns the temporary vector instead of exposing GPUContext’s internal scratchpad.