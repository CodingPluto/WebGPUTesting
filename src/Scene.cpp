#include <entt/entity/fwd.hpp>
#include <glm/geometric.hpp>
#include <spdlog/spdlog.h>
#include <string>
#include <sys/types.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <GLFW/glfw3.h>

#include "GPUContext.hpp"
#include "Scene.hpp"
#include "Components.hpp"
#include "RenderDataPacker.hpp"
#include "Input.hpp"

Scene::Scene() {}

void Scene::Initalize() {
    spdlog::info("Initalising scene");
    //auto entity = registry.create();
    //spdlog::info("EnTT successfully integrated! Test Entity ID: {}", static_cast<uint32_t>(entity));
    for (int i = 0; i < 100; ++i){
      entt::entity entity = registry_.create();
      registry_.emplace<TransformComponent>(entity, glm::vec2(100 + (i % 10) * 50, 100 + (i / 10) * 50), 0, glm::vec2(1.0f, 1.0f));
      if (i % 5 == 0){
        registry_.emplace<ColorComponent>(entity, glm::vec3(0.5, 0.5, 0.5));
      }
      registry_.emplace<NameComponent>(entity, "Boid #" + std::to_string(i));
      registry_.emplace<BoidComponent>(entity, i);
    }
}

void Scene::Update([[maybe_unused]]float delta_time, [[maybe_unused]] const Input &input_, [[maybe_unused]] const WindowManager &window_manager_){
  auto view = registry_.view<const BoidComponent, TransformComponent>();
  for (entt::entity boid_entity : view){
    [[maybe_unused]]auto& this_boid_transform = view.get<TransformComponent>(boid_entity);
    auto& this_boid = const_cast<BoidComponent&>(view.get<BoidComponent>(boid_entity));
    for (entt::entity other_boid_entity : view){
      [[maybe_unused]]auto& other_boid_transform = view.get<TransformComponent>(other_boid_entity);
      //auto& other_boid = const_cast<BoidComponent&>(view.get<BoidComponent>(other_boid_entity));
      
      float distance = glm::distance(this_boid_transform.position, other_boid_transform.position);
      if (distance > 0 && distance < 100){
        glm::vec2 difference = this_boid_transform.position - other_boid_transform.position;
        this_boid.velocity +=  (difference * delta_time) * 0.01f;
      }

      //this_boid_transform.position += this_boid_transform.position;
    }
    this_boid_transform.position += this_boid.velocity;
    if (this_boid_transform.position.x < 0) this_boid_transform.position.x = 0;
    if (this_boid_transform.position.y < 0) this_boid_transform.position.y = 0;
    if (this_boid_transform.position.x > 1280 - 50) this_boid_transform.position.x = 1280 - 50;
    if (this_boid_transform.position.y > 720 - 50) this_boid_transform.position.y = 720 - 50;
  }
  if (input_.IsKeyDown(GLFW_KEY_SPACE)){
    spdlog::info("Space key is down");
  }
  if (input_.IsKeyPressed(GLFW_KEY_SPACE)){
    spdlog::info("Space key was pressed");
  }
  if (input_.IsKeyReleased(GLFW_KEY_SPACE)){
    spdlog::info("Space key was released");
  }
  //spdlog::info("Mouse Delta: {}", glm::to_string(input_.GetMouseDelta()));
}




