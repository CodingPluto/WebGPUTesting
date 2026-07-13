#ifndef INCLUDE_GPUCONTEXT_HPP_
#define INCLUDE_GPUCONTEXT_HPP_


#include <sys/types.h>
#include <webgpu/webgpu_cpp.h>
#include <filesystem>
#include <spdlog/spdlog.h>
#include <fstream>
#include <glm/glm.hpp>
#include <array>

#include "formatted_webgpu.h"
struct ObjectData {
  glm::vec3 color = glm::vec3(1.0,1.0,1.0);
  float padding_1 = 0.0;
  glm::mat4 model_matrix = glm::mat4(1.0);
};

class GPUContext {
private:
  static constexpr size_t kMaxObjects = 50000;
  float camera_x = 0;
  float camera_y = 0;
  wgpu::Buffer storage_buffer_;
  // --- WebGPU Handles ---
  wgpu::Instance instance_ = nullptr;
  wgpu::Adapter adapter_ = nullptr;
  wgpu::Device device_ = nullptr;
  wgpu::Queue queue_ = nullptr;
  wgpu::Surface surface_ = nullptr;
  wgpu::SurfaceConfiguration surface_configuration_ = {};
  
  wgpu::TextureFormat surface_format_ = wgpu::TextureFormat::Undefined;
  
  wgpu::ShaderModule shader_module_ = nullptr;
  wgpu::RenderPipeline pipeline_ = nullptr;
  wgpu::ComputePipeline compute_pipeline_ = nullptr;
  
  wgpu::PipelineLayout pipeline_layout_ = nullptr;
  wgpu::BindGroupLayout bind_group_layout_ = nullptr;
  wgpu::BindGroup bind_group_ = nullptr;
  wgpu::RenderPassEncoder render_pass_ = nullptr;
  wgpu::CommandEncoder encoder_ = nullptr;
  
  wgpu::Buffer vertex_buffer_ = nullptr;
  wgpu::Buffer index_buffer_ = nullptr;
  wgpu::Buffer uniform_buffer_ = nullptr;
  wgpu::Buffer buffer_2_ = nullptr; // Note: consider renaming for clarity later
  
  glm::mat4 projection_matrix_ = {};
  glm::mat4 view_matrix_ = {};

  uint32_t index_count_ = 0;

  std::filesystem::file_time_type shader_last_edited_ = {};
  InitializationState initialized_state_ = InitializationState::Uninitalised;


  std::vector<ObjectData> object_data_scratchpad = {};
public:
  std::vector<ObjectData>& GetObjectDataScratchPadReference(){ return object_data_scratchpad;}
  void StartAdapterRequest(const wgpu::RequestAdapterOptions *options);
  void StartDeviceRequest(const wgpu::DeviceDescriptor *descriptor);
  void OutputFeatures(const wgpu::SupportedFeatures &features);
  void OutputLimits(const wgpu::Limits &limits);
  void InspectDevice(wgpu::Device device);
  void InspectAdapter(const wgpu::Adapter &adapter);

  void InitializeInstance();
  void InitializeSurface(class GLFWwindow *window);
  //void ConfigureSurface(GLFWwindow* window);

  void Update(float delta_time, double total_time_elapsed_);
  void Render();
  void InitializeCallbacks();

  void ConfigureSurface();
  void CreatePipelineLayout();
  void CreateRenderPipeline();
  void CreateComputePipeline();
  void CreateResources();
  void HotReloadShaders();

  void UpdateViewProjectionMatrices();

  std::pair<wgpu::SurfaceTexture, wgpu::TextureView> GetNextSurfaceViewData();

  void CreateShaderModules();
  wgpu::RenderPassEncoder& GetRenderPassEncoder() { return render_pass_; }
  wgpu::Device& GetDevice(){ return device_; }
  wgpu::TextureFormat& GetSurfaceFormat(){ return surface_format_; }
  InitializationState GetInitalizationState(){ return initialized_state_;}
};

[[nodiscard]] consteval wgpu::CallbackMode GetPlatformCallbackMode(){
  #ifdef __EMSCRIPTEN__
    return wgpu::CallbackMode::AllowSpontaneous;
  #endif
  return wgpu::CallbackMode::AllowProcessEvents;
}



inline bool LoadGeometry(
    const std::filesystem::path& path,
    std::vector<float>& pointData,
    std::vector<uint16_t>& indexData
) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }

    pointData.clear();
    indexData.clear();

    enum class Section {
        None,
        Points,
        Indices,
    };
    Section currentSection = Section::None;

    float value;
    uint16_t index;
    std::string line;
    while (!file.eof()) {
        getline(file, line);
        
        // overcome the `CRLF` problem
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        
        if (line == "[points]") {
            currentSection = Section::Points;
        }
        else if (line == "[indices]") {
            currentSection = Section::Indices;
        }
        else if (line[0] == '#' || line.empty()) {
            // Do nothing, this is a comment
        }
        else if (currentSection == Section::Points) {
            std::istringstream iss(line);
            // Get x, y, r, g, b
            for (int i = 0; i < 5; ++i) {
                iss >> value;
                pointData.push_back(value);
            }
        }
        else if (currentSection == Section::Indices) {
            std::istringstream iss(line);
            // Get corners #0 #1 and #2
            for (int i = 0; i < 3; ++i) {
                iss >> index;
                indexData.push_back(index);
            }
        }
    }
    return true;
}

#endif // INCLUDE_GPUCONTEXT_HPP_