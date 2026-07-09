#include <entt/entity/fwd.hpp>
#include <spdlog/spdlog.h>

#include "GPUContext.hpp"
#include "Scene.hpp"
#include "Components.hpp"
#include "RenderDataPacker.hpp"

Scene::Scene() {}

void Scene::Initalize() {
    spdlog::info("Initalising scene");
    //auto entity = registry.create();
    //spdlog::info("EnTT successfully integrated! Test Entity ID: {}", static_cast<uint32_t>(entity));
    for (int i = 0; i < 100; ++i){
      entt::entity entity = registry_.create();
      registry_.emplace<TransformComponent>(entity, glm::vec2(100 + (i % 10) * 50, 100 + (i / 10) * 50), 0, glm::vec2(1.0f, 1.0f));
      //registry_.emplace<ColorComponent>(entity, glm::vec4(1, 1, 1, 1));
    }

}



void Scene::UpdateGPUObjectData([[maybe_unused]]GPUContext &gpu) {
  std::vector<ObjectData> objects_data = {};
  FillGPUData<TransformComponent>(registry_, objects_data);
  //FillGPUData<ColorComponent>(registry_, objects_data);
  gpu.SetObjects(objects_data);
}


// template <typename... Types>
// struct ComponentList {};
// using Components = ComponentList<TransformComponent>;

