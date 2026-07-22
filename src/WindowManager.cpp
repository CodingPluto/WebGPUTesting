#include "WindowManager.hpp"

#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <GLFW/glfw3.h>
#else
#include <GLFW/glfw3.h>
#endif

#include <spdlog/spdlog.h>

#include "App.hpp"


void WindowManager::Initalize(uint16_t width, uint16_t height, const std::string& title){
  if (!glfwInit()){
    spdlog::critical("Failed to initalise GLFW!");
    assert(false);
  }
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  window_ = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
  spdlog::info("Created Window with dimensions: {}x{}", width, height);
  if (!window_){
    spdlog::critical("Failed to open Window");
    assert(false);
  }
  glfwSetWindowUserPointer(window_, this);
  glfwSetWindowFocusCallback(window_, []([[maybe_unused]]GLFWwindow* window, [[maybe_unused]]int focused){
    auto* window_manager = static_cast<WindowManager*>(glfwGetWindowUserPointer(window));
    window_manager->window_focused_ = static_cast<bool>(focused);
  });
}

void WindowManager::Update(App &app, [[maybe_unused]]const Input &input){
  glfwPollEvents();
  // if (input.IsKeyDown(GLFW_KEY_ESCAPE)){
  //   spdlog::info("Escape key pressed, closing window");
  //   glfwSetWindowShouldClose(window_, GLFW_TRUE);
  // }
  if (glfwWindowShouldClose(window_)) app.StopApp();
}

void WindowManager::Shutdown(){
  glfwDestroyWindow(window_);
  glfwTerminate();
}