#include <entt/entity/fwd.hpp>
#include <glm/geometric.hpp>
#include <spdlog/spdlog.h>
#include <string>
#include <sys/types.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

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
      if (i % 5 == 0){
        registry_.emplace<ColorComponent>(entity, glm::vec3(0.5, 0.5, 0.5));
      }
      registry_.emplace<NameComponent>(entity, "Boid #" + std::to_string(i));
      registry_.emplace<BoidComponent>(entity, i);
    }
}

// void Seperate(BoidComponent &boid, TransformComponent &transform, const entt::registry &registry){
//   auto view = registry.view<const BoidComponent, TransformComponent>();
//   //glm::vec2 steering = {0,0};
//   //int count = 0;
//   const float avoid_factor = 0.05;
//   float desired_separation = 25;
//   glm::vec2 movement_vector = {0,0};
//   for (entt::entity boid : view){
//     auto& interacting_transform = view.get<TransformComponent>(boid);
//     float distance = glm::distance(transform.position, interacting_transform.position);
//     if (distance > 0 && distance < desired_separation){
//       movement_vector += transform.position - interacting_transform.position;
//       //movement_vector = movement_vector / distance;
//       //count += 1;
//     }
//   }

//   boid.velocity += movement_vector * avoid_factor;
  
//   // if (count > 0){
//   //   steering = steering / static_cast<float>(count);
//   //   steering = steering * boid.max_speed - boid.velocity;
//   // }
// }

// void MatchVelocity(BoidComponent &boid, TransformComponent &transform, const entt::registry &registry){
//   const float matching_factor = 0.05;
//   const float visual_range = 75;
//   uint32_t neighbours = 0;
//   glm::vec2 average_velocity = {0,0};
//   auto view = registry.view<const BoidComponent, TransformComponent>();
//   for (entt::entity entity : view){
//     auto& interacting_transform = view.get<TransformComponent>(entity);
//     auto& interacting_boid = view.get<BoidComponent>(entity);
//     float distance = glm::distance(transform.position, interacting_transform.position);
//     if (distance > 0 && distance < visual_range){
//       average_velocity += interacting_boid.velocity;
//       neighbours += 1;
//     }
//     if (neighbours > 0){
//       average_velocity = average_velocity / static_cast<float>(neighbours);
//       boid.velocity += (average_velocity - boid.velocity) * matching_factor;
//     }
//   }
// }

// void FlyTorwardsCenter(BoidComponent &boid, TransformComponent &transform, const entt::registry &registry){
//   const float centering_factor = 0.005;
//   glm::vec2 center = {400,400};
//   auto view = registry.view<const BoidComponent, TransformComponent>();
//   for (entt::entity entity : view){
//     auto& interacting_transform = view.get<TransformComponent>(entity);
//     auto& interacting_boid = view.get<BoidComponent>(entity);

//     if (glm::distance)
//   }

// }


// void UpdateBoidSystem(BoidComponent &boid, TransformComponent &transform, const entt::registry &registry){


//   // Seperate
// }

//UpdateBoidSystem(boid, transform_component, registry_);

void Scene::Update([[maybe_unused]]float delta_time){
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

  }



}




