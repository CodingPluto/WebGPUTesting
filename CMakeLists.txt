cmake_minimum_required(VERSION 3.25)
project(
  LearnWebGPU
  LANGUAGES CXX C
)


if (NOT EMSCRIPTEN)
  set(TINT_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(TINT_BUILD_FUZZERS OFF CACHE BOOL "" FORCE)
  set(TINT_BUILD_BENCHMARKS OFF CACHE BOOL "" FORCE)
  set(TINT_BUILD_IR TOOLS OFF CACHE BOOL "" FORCE)
  set(TINT_BUILD_IR_BINARY OFF CACHE BOOL "" FORCE)
  set(DAWN_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(DAWN_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
  set(DAWN_BUILD_BENCHMARKS OFF CACHE BOOL "" FORCE)
  set(DAWN_BUILD_PROTOBUF OFF CACHE BOOL "" FORCE)
  set(DAWN_ENABLE_INSTALL off)
  find_package(Dawn CONFIG REQUIRED)
  find_package(glfw3 CONFIG REQUIRED)
endif()

find_package(spdlog CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(App src/main.cpp src/glfw3webgpu.c)

target_link_libraries(App PRIVATE glm::glm)

if (APPLE)
  set_source_files_properties(src/glfw3webgpu.c PROPERTIES COMPILE_FLAGS "-x objective-c++ -w")
  
else()
  set_source_files_properties(src/glfw3webgpu.c PROPERTIES LANGUAGE CXX COMPILE_FLAGS "-w")
endif()

target_link_libraries(App PRIVATE spdlog::spdlog_header_only)



target_include_directories(App PRIVATE ${CMAKE_SOURCE_DIR}/include)
if (NOT EMSCRIPTEN)
  target_link_libraries(App PRIVATE dawn::webgpu_dawn dawn::dawn_public_config)
  target_link_libraries(App PRIVATE glfw)
endif()


if (APPLE)
  target_link_libraries(App 
    PRIVATE 
    "-framework Cocoa"
    "-framework CoreVideo"
    "-framework IOKit"
    "-framework QuartzCore"
  )
endif()

set_target_properties(App PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    COMPILE_WARNING_AS_ERROR ON
    #RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build"
)
if (MSVC)
    target_compile_options(App PRIVATE /W4)
else()
    target_compile_options(App PRIVATE -Wall -Wextra -pedantic)
endif()
if (XCODE)
    set_target_properties(App PROPERTIES
        XCODE_GENERATE_SCHEME ON
        XCODE_SCHEME_ENABLE_GPU_FRAME_CAPTURE_MODE "Metal"
    )
endif()


if (EMSCRIPTEN)
  target_compile_definitions(App PRIVATE SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO)
  target_compile_options(App PRIVATE --use-port=emdawnwebgpu) # Supplies dawn files to compiler. Needed for includes.
  target_compile_definitions(App PRIVATE WEBGPU_BACKEND_EMDAWNWEBGPU)
  target_compile_definitions(App PRIVATE
        DAWN_ENABLE_EMSCRIPTEN=ON
        DAWN_BUILD_SAMPLES=OFF
        DAWN_BUILD_TESTS=OFF
        DAWN_ENABLE_DESKTOP_GL=OFF
        DAWN_ENABLE_VULKAN=OFF
        DAWN_ENABLE_METAL=ON
        DAWN_ENABLE_D3D11=OFF
        DAWN_ENABLE_D3D12=OFF
        DAWN_USE_GLFW=ON
    )
  target_link_options(App PRIVATE
        -sUSE_GLFW=3
        --use-port=emdawnwebgpu
        --preload-file ${CMAKE_SOURCE_DIR}/assets@assets/
    )
  set_target_properties(App PROPERTIES SUFFIX ".html")
endif()

add_custom_target(copy_assets ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory
      ${CMAKE_SOURCE_DIR}/assets
      ${CMAKE_BINARY_DIR}/assets
  COMMENT "Syncing assets..."
)

# if (DEVELOPER_MODE)
#   target_compile_definitions(App PRIVATE RESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets")
# else()
#   target_compile_definitions(App PRIVATE RESOURCE_DIR="./assets")
# endif()