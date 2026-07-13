#ifndef INCLUDE_COMPONENTS_HPP_
#define INCLUDE_COMPONENTS_HPP_
#include <glm/detail/qualifier.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


struct TransformComponent{
  glm::vec2 position = {0.0, 0.0};
  float rotation = {0.0f};
  glm::vec2 scale = {1.0, 1.0};
};

struct ColorComponent{
  glm::vec3 color = {1.0,1.0,1.0};
};

struct NameComponent{
  std::string name = "DEFAULT_NAME";
};

struct BoidComponent{
  uint32_t boid_number = 0;
  glm::vec2 velocity = {};
  glm::vec2 acceleration = {};
  float max_speed = 4;
  float max_force = 2;
  float perception_radius = 50;
};

// template <typename... Types>
// struct ComponentList {};
// using Components = ComponentList<TransformComponent, VelocityComponent>;


#endif // INCLUDE_COMPONENTS_HPP_