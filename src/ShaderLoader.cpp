#include "ShaderLoader.hpp"

#include <fstream>
#include <iterator>
#include <spdlog/spdlog.h>

wgpu::ShaderModule ShaderLoader::Load(const wgpu::Device& device, const std::string& path) {
  std::string shader_source = ReadFileToString(path);
  spdlog::trace("Shader source: {}", shader_source);
  wgpu::ShaderModuleDescriptor shader_descriptor = {};
  wgpu::ShaderSourceWGSL shader_code_descriptor = {};
  shader_code_descriptor.nextInChain = nullptr; // why does this exist?
  shader_code_descriptor.sType = wgpu::SType::ShaderSourceWGSL;
  shader_descriptor.nextInChain = &shader_code_descriptor;
  shader_code_descriptor.code = shader_source.c_str();
  
  return device.CreateShaderModule(&shader_descriptor);
}

std::filesystem::file_time_type ShaderLoader::GetLastEdited(const std::string& path) {
  std::error_code ec = {};
  auto current = std::filesystem::last_write_time(path,ec);
  return current;
}

std::string ShaderLoader::ReadFileToString(const std::string& path) {
  std::ifstream file(path, std::ios::binary);
  if (!file){
    spdlog::warn("Failed to open and read file: {}", path);
    spdlog::warn("Returning empty string.");
    return std::string();
  }
  return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}