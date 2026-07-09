#ifndef INCLUDE_SCENE_HPP_
#define INCLUDE_SCENE_HPP_
#include <entt/entity/fwd.hpp>
#include <entt/entt.hpp>

#include "GPUContext.hpp"

class Scene{
private:
  entt::registry registry_ = {};
public:
  Scene();
  ~Scene() = default;

  void Initalize();
  void Update(float delta_time);
  void UpdateGPUObjectData(GPUContext &gpu);
};


#endif