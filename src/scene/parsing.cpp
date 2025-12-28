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

void to_json(json& j, const LightData& l) {
    j = json{{"directionalLightDirection", {l.directionalLightDirection.x, l.directionalLightDirection.y, l.directionalLightDirection.z}},
             {"directionalLightColor", {l.directionalLightColor.x, l.directionalLightColor.y, l.directionalLightColor.z}},
             {"ambientLightColor", {l.ambientLightColor.x, l.ambientLightColor.y, l.ambientLightColor.z}}
    };
}

void from_json(const json& j, LightData& l) {
    l.directionalLightDirection = glm::normalize(glm::vec3(j.at("directionalLightDirection")[0], j.at("directionalLightDirection")[1], j.at("directionalLightDirection")[2]));
    l.directionalLightColor = glm::vec3(j.at("directionalLightColor")[0], j.at("directionalLightColor")[1], j.at("directionalLightColor")[2]);
    l.ambientLightColor = glm::vec3(j.at("ambientLightColor")[0], j.at("ambientLightColor")[1], j.at("ambientLightColor")[2]);
}

void to_json(json& j, const Scene& s) {
    j = json{{"camera", s.camera}, {"lightData", s.lightData}, {"spheres", s.spheres}};
}

void from_json(const json& j, Scene& s) {
    s.camera  = j.at("camera").get<Camera>();
    s.lightData = j.at("lightData").get<LightData>();
    s.spheres = j.at("spheres").get<std::vector<Sphere>>();
}

} // namespace rte
