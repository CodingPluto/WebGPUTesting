#include "StartupCoordinator.hpp"
#include "DebugTools.hpp"
#include "GPULogger.hpp"
#include "GPUContext.hpp"
#include "ImGuiManager.hpp"

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

void StartupCoordinator::StartupTick(GPUContext &gpu, ImGuiManager &imgui_manager, GLFWwindow *window){
  spdlog::info("Initalization stage: {}", initialized_state_);
  switch (initialized_state_){
    case InitializationState::Uninitalised: {
      gpu.SetupAdapterRequest(*this);
      initialized_state_ = InitializationState::RequestedAdapter;
      return;
    }
    case InitializationState::RequestedAdapter:
      gpu.ProcessEvents();
      return;
    case InitializationState::RequestingDevice:{
      GPULogger::InspectAdapter(gpu.GetAdapter());
      gpu.SetupDeviceRequest(*this);
      initialized_state_ = InitializationState::RequestedDevice;
      return;
    }
    case InitializationState::RequestedDevice:
      gpu.ProcessEvents();
      return;
    case InitializationState::ReceivedAdapterAndDevice:
      GPULogger::InspectDevice(gpu.GetDevice());
      gpu.ConfigureQueue();
      gpu.CreateShaderModules();
      gpu.ConfigureSurface();
      gpu.CreateRenderPipeline();
      gpu.CreateComputePipeline();
      imgui_manager.Initialize(window, gpu.GetDevice(), gpu.GetSurfaceFormat());
      initialized_state_ = InitializationState::Ready;
      return;
    default:
      spdlog::error("Invalid InitializationState enum value");
      return;
  }
}