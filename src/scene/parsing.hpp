#pragma once

#include "scene.hpp"
#include "camera.hpp"
#include <core/sphere.hpp>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace rte {

void to_json(json& j, const Sphere& s);
void from_json(const json& j, Sphere& s);

void to_json(json& j, const Camera& c);
void from_json(const json& j, Camera& c);

void to_json(json& j, const Scene& s);
void from_json(const json& j, Scene& s);

} // namespace rte
