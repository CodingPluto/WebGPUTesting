#ifndef INCLUDE_INPUT_HPP
#define INCLUDE_INPUT_HPP

#include <glm/glm.hpp>
#include <glfw/glfw3.h>

#include "WindowManager.hpp"

class Input{
private:
  //static std::vector<int> keys_pressed = {};
  GLFWwindow* window_ = nullptr;
  int keys_pressed_previous_frame[GLFW_KEY_LAST + 1] = {}; // Array to store the state of keys (pressed or not)
  double x_mouse_previous_frame_ = 0.0;
  double y_mouse_previous_frame_ = 0.0;
public:
Input() = default;
  void Initialize(GLFWwindow* window);
  [[nodiscard]]
  bool IsKeyDown(int key) const;
  glm::vec2 GetMousePosition() const;
  bool IsMouseButtonDown(int button) const;
  bool IsKeyPressed(int key) const;
  bool IsKeyReleased(int key) const;
  glm::vec2 GetMouseDelta() const;
  void Update();

};


#endif // INCLUDE_INPUT_HPP