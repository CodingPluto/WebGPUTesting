#include "ImGuiManager.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_wgpu.h>
#include <spdlog/spdlog.h>

#include "formatted_webgpu.h"

void ImGuiManager::Initialize(GLFWwindow* window, wgpu::Device device, wgpu::TextureFormat surface_format) {
  spdlog::debug("Attempting to initalise ImGui");
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.DisplaySize = ImVec2(1280.0f, 720.0f);
  io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOther(window, true);
  ImGui_ImplWGPU_InitInfo initalization_info = {};
  initalization_info.Device = device.Get();
  initalization_info.NumFramesInFlight = 2;
  initalization_info.RenderTargetFormat = static_cast<WGPUTextureFormat>(surface_format); // check this works
  spdlog::debug("Surface format is {}", surface_format);
  initalization_info.DepthStencilFormat = WGPUTextureFormat_Undefined;
  initalization_info.PipelineMultisampleState.count = 1;
  ImGui_ImplWGPU_Init(&initalization_info);
  spdlog::debug("Successfully initalised ImGui");
}

void ImGuiManager::BeginFrame() {

  ImGui_ImplWGPU_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  ImGuiIO& io = ImGui::GetIO();
  io.DisplaySize = ImVec2(1280.0f, 720.0f);
  io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
  ImGui::Begin("Test!!");
  ImGui::Text("This is test text.");
  ImGui::End();
}

void ImGuiManager::EndFrame(wgpu::RenderPassEncoder render_pass) {
  ImGui::Render(); // might need to swap these two lines.
  ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), render_pass.Get());
}

void ImGuiManager::Shutdown() {
  ImGui_ImplWGPU_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}