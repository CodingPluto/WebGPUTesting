#ifndef INCLUDE_RENDER_DATA_PACKER_HPP
#define INCLUDE_RENDER_DATA_PACKER_HPP


#include <entt/entity/fwd.hpp>
#include <spdlog/spdlog.h>
#include <entt/entt.hpp>

#include "GPUContext.hpp"
#include "Components.hpp"




template<typename T>
inline void UpdateObjectData([[maybe_unused]]const T &component, [[maybe_unused]]ObjectData &object_data){
  spdlog::error("Unknown component. No specialisation available");
}
inline void UpdateObjectData(const TransformComponent &transform, ObjectData &object_data){
  object_data.model_matrix = glm::mat4(1.0);
  object_data.model_matrix = glm::translate(object_data.model_matrix, glm::vec3(transform.position, 0.0f));
  object_data.model_matrix = glm::rotate(object_data.model_matrix, transform.rotation, glm::vec3(0.0, 0.0, 1.0));
  object_data.model_matrix = glm::scale(object_data.model_matrix, glm::vec3(transform.scale, 1.0));
}
inline void UpdateObjectData(const ColorComponent &color, ObjectData &object_data){
  object_data.color = color.color;
}
template<typename... Components>
void FillGPUData(const entt::registry& registry, std::vector<ObjectData>& objects_data){
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
  objects_data.clear();
  objects_data.reserve(view.size());
  for (entt::entity entity : view){
    ObjectData object_data = {};
    UpdateObjectData(view.get<TransformComponent>(entity), object_data);
    auto try_update = [&object_data, &registry, &entity]<typename T>(){
      if (const auto* component = registry.try_get<T>(entity)){
        UpdateObjectData(*component, object_data);
      }
    };
    (try_update.template operator()<Components>(),...);
    objects_data.push_back(object_data);
  }
  // ok might use groups in future to help with cache locality.
}

inline void UpdateGPUObjectData([[maybe_unused]]GPUContext &gpu, const entt::registry &registry) {
  std::vector<ObjectData>& objects = gpu.GetObjectDataScratchPadReference();
  FillGPUData<TransformComponent,ColorComponent>(registry, objects);
}


#endif