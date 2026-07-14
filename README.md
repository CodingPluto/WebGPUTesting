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



Next steps:
- Add a seperate logger static class for GPUContext to remove the bloat from it. This way passing in the variables to the function makes sense.
- Clean up the App initalisation code such that we might be able to remove some getters and some bloat.
- Finish the boid simulation code.