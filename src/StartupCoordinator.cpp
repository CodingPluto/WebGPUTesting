#include "StartupCoordinator.hpp"
#include "DebugTools.hpp"
#include "GPULogger.hpp"
#include "GPUContext.hpp"
#include "ImGuiManager.hpp"

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

void StartupCoordinator::StartupTick(GPUContext &gpu){
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
      // wgpu::BufferDescriptor desc = {};
      // desc.size = 100 * 1024 * 1024; // 100MB (Way over your 240-byte limit)
      // desc.usage = wgpu::BufferUsage::CopySrc;

      // // This will fail validation instantly and fire your UncapturedErrorCallback
      // wgpu::Buffer badBuffer = gpu.GetDevice().CreateBuffer(&desc);
      initialized_state_ = InitializationState::Ready;
      return;
    default:
      spdlog::error("Invalid InitializationState enum value");
      return;
  }
}