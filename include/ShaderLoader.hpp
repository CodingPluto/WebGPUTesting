#ifndef INCLUDE_SHADER_LOADER_HPP_
#define INCLUDE_SHADER_LOADER_HPP_

#include <string>
#include <filesystem>
#include <webgpu/webgpu_cpp.h>

class ShaderLoader {
 public:
  ShaderLoader() = delete;
  [[nodiscard]] static wgpu::ShaderModule Load(const wgpu::Device& device, const std::string& path);
  [[nodiscard]] static std::filesystem::file_time_type GetLastEdited(const std::string& path);
 private:
  [[nodiscard]] static std::string ReadFileToString(const std::string& path);
};

#endif  // INCLUDE_SHADER_LOADER_HPP_