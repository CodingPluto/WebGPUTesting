#include "Input.hpp"

#include <spdlog/spdlog.h>
#include <glfw/glfw3.h>
#include <glm/common.hpp>


bool Input::IsKeyDown(int key) const{
  spdlog::trace("Checking if key {} is down", key);
  return glfwGetKey(window_, key) == GLFW_PRESS;
}

glm::vec2 Input::GetMousePosition() const{
  double x, y;
  glfwGetCursorPos(window_, &x, &y);
  return glm::vec2(static_cast<float>(x), static_cast<float>(y));
}

bool Input::IsKeyPressed(int key) const{
  return glfwGetKey(window_, key) == GLFW_PRESS && !keys_pressed_previous_frame[key];
}
bool Input::IsKeyReleased(int key) const{ // not entirely sure that logic is correct
  return glfwGetKey(window_, key) == GLFW_RELEASE && keys_pressed_previous_frame[key];
}
glm::vec2 Input::GetMouseDelta() const{
  return glm::abs(GetMousePosition() - glm::vec2(static_cast<float>(x_mouse_previous_frame_), static_cast<float>(y_mouse_previous_frame_)));
}

void Input::Initialize(GLFWwindow* window){
  // glfwSetKeyCallback(window_manager.GetNativeWindowHandle(), [](GLFWwindow* window, int key, [[maybe_unused]]int scancode, int action, [[maybe_unused]]int mods){
  //   Input::keys_pressed.push_back(key);
  // });
  if (!window){
    spdlog::critical("Input system failed to initalize, window is null");
    assert(false);
  }
  window_ = window;
  spdlog::info("Input system initalized");
}
// bool Input::IsKeyPressed(int key) const{

// }
// bool Input::IsKeyReleased(int key) const{

// }
// bool Input::IsMouseButtonDown(int button) const{

// }
// glm::vec2 Input::GetMousePosition() const{

// }
// glm::vec2 Input::GetMouseDelta() const{

// }

void Input::Update(){
  for (int key = 0; key <= GLFW_KEY_LAST; ++key) {
    keys_pressed_previous_frame[key] = glfwGetKey(window_, key);
  }
  glfwGetCursorPos(window_, &x_mouse_previous_frame_, &y_mouse_previous_frame_);
 //Input::keys_pressed.clear();
}