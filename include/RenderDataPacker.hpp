#ifndef INCLUDE_RENDER_DATA_PACKER_HPP
#define INCLUDE_RENDER_DATA_PACKER_HPP


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
// inline void UpdateObjectData(const ColorComponent &color, ObjectData &object_data){
//   object_data.color = color.color;
// }


template<typename... Components>
void FillGPUData(const entt::registry& registry, std::vector<ObjectData>& objects_data){
  auto view = registry.view<Components...>();
  objects_data.reserve(view.size());
  for (auto entity : view){
    ObjectData object_data = {};
    UpdateObjectData(view.template get<Components>(entity)..., object_data);
    objects_data.push_back(object_data);
  }
}


#endif