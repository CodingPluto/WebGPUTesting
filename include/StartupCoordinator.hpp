#ifndef INCLUDE_STARTUP_COORDINATOR_HPP
#define INCLUDE_STARTUP_COORDINATOR_HPP

#include "GPUContext.hpp"
#include "ImGuiManager.hpp"
#include "formatted_webgpu.h"

class StartupCoordinator{
  InitializationState initialized_state_ = InitializationState::Uninitalised;
  public:
  bool IsInitalized() {return initialized_state_ == InitializationState::Ready;}
  inline void SetInitalizedState(InitializationState state){initialized_state_ = state;}
  void StartupTick(GPUContext &gpu, ImGuiManager &imgui_manager, GLFWwindow *window);
};





#endif // INCLUDE_STARTUP_COORDINATOR_HPP