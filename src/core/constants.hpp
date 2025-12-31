#pragma once

#include <glm/glm.hpp>

constexpr glm::vec3 ZERO = {0, 0, 0};
constexpr glm::vec3 RIGHT = {1, 0, 0};
constexpr glm::vec3 UP = {0, 1, 0};
constexpr glm::vec3 FORWARD = {0, 0, 1};
constexpr glm::vec3 ONE = {1, 1, 1};

constexpr glm::vec3 BLACK = {0, 0, 0};
constexpr glm::vec3 WHITE = {1, 1, 1};

constexpr float DIFFUSE_REFLECTION_CONSTANT = 0.8f;
constexpr float SPECULAR_REFLECTION_CONSTANT = 0.9f;
constexpr unsigned int SPECULAR_EXPONENT_POW2 = 6;
constexpr float SPECULAR_EXPONENT = 1 << SPECULAR_EXPONENT_POW2;
