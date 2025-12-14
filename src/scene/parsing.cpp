#include "parsing.hpp"

namespace rte {

void to_json(json& j, const Sphere& s) {
    j = json{{"position", {s.position.x, s.position.y, s.position.z}},
             {"radius", s.radius},
             {"color", {s.color.r, s.color.g, s.color.b}}};
}

void from_json(const json& j, Sphere& s) {
    s.position = glm::vec3(j.at("position")[0], j.at("position")[1], j.at("position")[2]);
    s.radius   = j.at("radius");
    s.color    = glm::vec3(j.at("color")[0], j.at("color")[1], j.at("color")[2]);
}

void to_json(json& j, const Camera& c) {
    j = json{{"position", {c.position.x, c.position.y, c.position.z}},
             {"forward",  {c.forward.x, c.forward.y, c.forward.z}},
             {"fov", c.fov}};
}

void from_json(const json& j, Camera& c) {
    c.position = glm::vec3(j.at("position")[0], j.at("position")[1], j.at("position")[2]);
    c.forward  = glm::vec3(j.at("forward")[0], j.at("forward")[1], j.at("forward")[2]);
    c.fov      = j.at("fov");
}

void to_json(json& j, const Scene& s) {
    j = json{{"camera", s.camera}, {"spheres", s.spheres}};
}

void from_json(const json& j, Scene& s) {
    s.camera  = j.at("camera").get<Camera>();
    s.spheres = j.at("spheres").get<std::vector<Sphere>>();
}

} // namespace rte
