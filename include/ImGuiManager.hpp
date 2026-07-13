#ifndef INCLUDE_IM_GUI_MANAGER
#define INCLUDE_IM_GUI_MANAGER
#include <GLFW/glfw3.h>
#include <webgpu/webgpu_cpp.h>

class ImGuiManager {
 public:
  void Initialize(GLFWwindow* window, wgpu::Device device, wgpu::TextureFormat surface_format);
  void BeginFrame();
  void EndFrame(wgpu::RenderPassEncoder render_pass);
  void Shutdown();
};


#endif // INCLUDE_IM_GUI_MANAGER