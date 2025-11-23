#include "sequential_renderer.hpp"

// std
#include <vector>
#include <limits>
#include <iostream>

namespace rte {

constexpr glm::vec3 ZERO = {0, 0, 0};
constexpr glm::vec3 RIGHT = {1, 0, 0};
constexpr glm::vec3 UP = {0, 1, 0};
constexpr glm::vec3 FORWARD = {0, 0, 1};
constexpr glm::vec3 ONE = {1, 1, 1};
constexpr uint MAX_REFLECTIONS = 5;
constexpr float MIN_HIT_DISTANCE = 0.001f;

void SequentialRenderer::prepareFrame() {
    const glm::vec3 cameraPosition = {0, 0, -10};
    const glm::vec3 cameraForward = FORWARD;
    const float cameraSizeX = 4.0f;
    const float cameraSizeY = 3.0f;

    const glm::vec3 topLeft = cameraPosition - 0.5f * cameraSizeX * RIGHT + 0.5f * cameraSizeY * UP;

    const float xDiff = cameraSizeX / extent.width;
    const float yDiff = cameraSizeY / extent.height;

    uint8_t* data = reinterpret_cast<uint8_t*>(stagingData);
    glm::vec3 pixelColor;
    uint32_t pixelIndex;
    Ray ray;
    for (auto y = 0; y < extent.height; y++) {
        for (auto x = 0; x < extent.width; x++) {
            pixelIndex = (x + y * extent.width) * 4;

            glm::vec3 pixelPosition = topLeft + x * xDiff * RIGHT - y * yDiff * UP;
            ray = {pixelPosition, cameraForward};
            pixelColor = raycast(ray);
            
            data[pixelIndex + 2] = static_cast<uint8_t>(pixelColor.r * 255);
            data[pixelIndex + 1] = static_cast<uint8_t>(pixelColor.g * 255);
            data[pixelIndex + 0] = static_cast<uint8_t>(pixelColor.b * 255);
            data[pixelIndex + 3] = 255;
        }
    }
}

glm::vec3 SequentialRenderer::raycast(Ray& ray) {
    glm::vec3 lightColor = {0, 0, 0};
    Ray rays[MAX_REFLECTIONS + 1];
    RaycastHit hits[MAX_REFLECTIONS] {};
    glm::vec3 hitColors[MAX_REFLECTIONS] {};
    rays[0] = ray;

    for (auto i = 0; i < MAX_REFLECTIONS; i++) {        
        hits[i].distance = std::numeric_limits<float>::max();
        RaycastHit hit{};
        for (Sphere& sphere : spheres) {
            if (raySphereIntersect(rays[i], sphere, &hit) &&
                    hit.distance < hits[i].distance) 
            {
                hits[i] = hit;
                hitColors[i] = sphere.color;
                rays[i+1] = {hit.position, reflect(rays[i].direction, hit.normal)};
            }
        }
    }

    for (int i = MAX_REFLECTIONS - 1; i >= 0; i--) {
        if (hits[i].distance > MIN_HIT_DISTANCE) { // has hit
            float reflectionIntensity = glm::dot(-rays[i].direction, hits[i].normal);
            lightColor *= reflectionIntensity;
            lightColor += ambientLight;
            lightColor += shadowRay(hits[i]);
            lightColor *= hitColors[i];
            lightColor = glm::clamp(lightColor, ZERO, ONE);
        }
    }

    return lightColor;
}

glm::vec3 SequentialRenderer::shadowRay(RaycastHit hit) {
    Ray shadowRay = {hit.position, inverseLightDirection};

    for (Sphere& sphere : spheres) {
        RaycastHit shadowHit;
        if (raySphereIntersect(shadowRay, sphere, &shadowHit)) {
            return {0, 0, 0};
        }
    }

    float intensity = glm::dot(shadowRay.direction, hit.normal);
    return intensity * ONE;
}

bool SequentialRenderer::raySphereIntersect(Ray& ray, Sphere &sphere, RaycastHit* hit) {
    glm::vec3 positionDelta = sphere.position - ray.origin;

    float a = glm::dot(ray.direction, ray.direction);
    float b = -2.0f * glm::dot(positionDelta, ray.direction);
    float c = glm::dot(positionDelta, positionDelta) - sphere.radius * sphere.radius;

    float determinant = b * b - 4.0f * a * c;

    float x1, x2;
    if (determinant < 0) {
        return false;
    } else {
        x1 = (-b - sqrt(determinant)) / (2.0f * a);
        x2 = (-b + sqrt(determinant)) / (2.0f * a);
    }

    float distance = 0;
    if(x1 > MIN_HIT_DISTANCE) {
        distance = x1;
    } else if(x2 > MIN_HIT_DISTANCE) {
        distance = x2;
    } else {
        return false;
    }

    glm::vec3 position = ray.origin + ray.direction * distance;
    glm::vec3 normal = glm::normalize(position - sphere.position);

    hit->position = position;
    hit->normal = normal;
    hit->distance = distance;

    return true;
}

glm::vec3 SequentialRenderer::reflect(glm::vec3 rayDirection, glm::vec3 normal) {
    return rayDirection - 2.0f * normal * glm::dot(rayDirection, normal);
}

} // namespace rte
