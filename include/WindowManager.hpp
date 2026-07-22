#ifndef INCLUDE_WINDOW_MANAGER_HPP
#define INCLUDE_WINDOW_MANAGER_HPP

#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <GLFW/glfw3.h>
#else
#include <GLFW/glfw3.h>
#endif


class WindowManager{
  GLFWwindow* window_ = nullptr;
  bool window_focused_ = true;
  public:
  void Initalize(uint16_t width, uint16_t height, const std::string& title);
  [[nodiscard]] GLFWwindow* GetNativeWindowHandle() const {return window_;};
  void Update(class App &app, const class Input &input);
  void Shutdown();
  [[nodiscard]] bool IsFocused() const { return window_focused_; }
};



#endif // INCLUDE_WINDOW_MANAGER_HPP