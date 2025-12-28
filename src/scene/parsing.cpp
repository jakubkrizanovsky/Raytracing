#include "parsing.hpp"

namespace glm {

void from_json(const nlohmann::json& j, vec3& v) {
    v.x = j.at(0);
    v.y = j.at(1);
    v.z = j.at(2);
}

}

namespace rte {

void from_json(const json& j, Sphere& s) {
    s.position = j.at("position");
    s.radius   = j.at("radius");
    s.color    = j.at("color");
}

void from_json(const json& j, Camera& c) {
    c.position = j.at("position");
    c.forward  = j.at("forward");
    c.fov      = j.at("fov");

    c.focusPoint = j.value("focusPoint", glm::vec3(0.0f));
    c.orbitVelocity = j.value("orbitVelocity", 0.0f);
}

void from_json(const json& j, LightData& l) {
    l.directionalLightDirection = glm::normalize(j.at("directionalLightDirection").get<glm::vec3>());
    l.directionalLightColor = j.at("directionalLightColor");
    l.ambientLightColor = j.at("ambientLightColor");
}

void from_json(const json& j, Scene& s) {
    s.camera  = j.at("camera").get<Camera>();
    s.lightData = j.at("lightData").get<LightData>();
    s.spheres = j.at("spheres").get<std::vector<Sphere>>();
}

} // namespace rte
