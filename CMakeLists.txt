cmake_minimum_required(VERSION 3.25)
project(
  LearnWebGPU
  LANGUAGES CXX C
)

if (NOT emscripten)
  set(DAWN_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(DAWN_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
  set(DAWN_ENABLE_INSTALL off)
  add_subdirectory(external/dawn)
endif()

add_executable(App src/main.cpp)



target_include_directories(App PRIVATE ${CMAKE_SOURCE_DIR}/include)


if (NOT emscripten)
  target_include_directories(App PRIVATE ${CMAKE_SOURCE_DIR}/external/dawn/include)
  target_link_libraries(App PUBLIC webgpu_dawn)
endif()


set_target_properties(App PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    COMPILE_WARNING_AS_ERROR ON
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

#if (NOT EMSCRPITEN)
#  add_subdirectory(glfw)
#endif()

if (EMSCRIPTEN)
  target_link_options(App PRIVATE
        -sUSE_GLFW=3
        --use-port=emdawnwebgpu
        -sASYNCIFY
    )
  set_target_properties(App PROPERTIES SUFFIX ".html")
endif()