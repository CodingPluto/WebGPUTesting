#ifndef INCLUDE_RENDER_DATA_PACKER_HPP
#define INCLUDE_RENDER_DATA_PACKER_HPP


#include <entt/entity/fwd.hpp>
#include <spdlog/spdlog.h>
#include <entt/entt.hpp>

#include "GPUContext.hpp"
#include "Components.hpp"
#include "FrameRenderData.hpp"




template<typename T>
inline void UpdateObjectData([[maybe_unused]]const T &component, [[maybe_unused]]GPUObjectData &object_data){
  spdlog::error("Unknown component. No specialisation available");
}
inline void UpdateObjectData(const TransformComponent &transform, GPUObjectData &object_data){
  object_data.model_matrix = glm::mat4(1.0);
  object_data.model_matrix = glm::translate(object_data.model_matrix, glm::vec3(transform.position, 0.0f));
  object_data.model_matrix = glm::rotate(object_data.model_matrix, transform.rotation, glm::vec3(0.0, 0.0, 1.0));
  object_data.model_matrix = glm::scale(object_data.model_matrix, glm::vec3(transform.scale, 1.0));
}
inline void UpdateObjectData(const ColorComponent &color, GPUObjectData &object_data){
  object_data.color = color.color;
}
template<typename... Components>
void FillGPUData(const entt::registry& registry, FrameRenderData &frame_render_data){
  #ifndef NDEBUG // Entity checking for TransformComponent
    auto ensure_transform_component = [&]<typename T>(){
      for (auto entity : registry.view<const T>()){ // readonly
        if (!registry.all_of<TransformComponent>(entity)){
          constexpr const char* error_message = "Entity: {} has render components but no transform component";
          if (const auto* name_component = registry.try_get<NameComponent>(entity)){
            spdlog::critical(error_message, name_component->name);
          }
          else{
            spdlog::critical(error_message,static_cast<uint32_t>(entity));
          }
          assert(false);
        }
      }
    };
    (ensure_transform_component.template operator()<Components>(), ...);
  #endif
  auto view = registry.view<const TransformComponent>();
  for (entt::entity entity : view){
    GPUObjectData object_data = {};
    UpdateObjectData(view.get<TransformComponent>(entity), object_data);
    auto try_update = [&object_data, &registry, &entity]<typename T>(){
      if (const auto* component = registry.try_get<T>(entity)){
        UpdateObjectData(*component, object_data);
      }
    };
    (try_update.template operator()<Components>(),...);
    frame_render_data.add(object_data);
  }
  // ok might use groups in future to help with cache locality.
}


// WIP HERE
inline void UpdateGPUObjectData([[maybe_unused]]GPUContext &gpu, FrameRenderData &frame_render_data, const entt::registry &registry) {
  auto objects = frame_render_data.Objects();
  frame_render_data.ClearObjects();
  frame_render_data.Reserve(registry.view<const TransformComponent, const ColorComponent>().size_hint());
  FillGPUData<TransformComponent,ColorComponent>(registry, frame_render_data);
  gpu.UpdateObjectDataScratchPad(objects);
}


#endif