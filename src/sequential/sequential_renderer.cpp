#include "sequential_renderer.hpp"
#include <core/constants.hpp>

// std
#include <vector>
#include <limits>
#include <iostream>

namespace rte {

constexpr uint MAX_REFLECTIONS = 5;
constexpr float MIN_HIT_DISTANCE = 0.001f;

void SequentialRenderer::prepareFrame() {
    camera->prepareFrame(extent);

    uint8_t* data = reinterpret_cast<uint8_t*>(stagingData);
    for (auto y = 0; y < extent.height; y++) {
        for (auto x = 0; x < extent.width; x++) {
            Ray ray = camera->getRay(x, y);
            glm::vec3 pixelColor = raycast(ray);
            
            uint32_t pixelIndex = (x + y * extent.width) * 4;
            data[pixelIndex + 2] = static_cast<uint8_t>(pixelColor.r * 255);
            data[pixelIndex + 1] = static_cast<uint8_t>(pixelColor.g * 255);
            data[pixelIndex + 0] = static_cast<uint8_t>(pixelColor.b * 255);
            data[pixelIndex + 3] = 255;
        }
    }
}

void SequentialRenderer::setScene(std::shared_ptr<Scene> newScene) {
    CpuRenderer::setScene(newScene);
    camera = std::make_unique<SequentialCamera>(scene->camera);
}

glm::vec3 SequentialRenderer::raycast(Ray &ray) {
    glm::vec3 lightColor = BLACK;
    Ray rays[MAX_REFLECTIONS + 1];
    RaycastHit hits[MAX_REFLECTIONS] {};
    glm::vec3 hitColors[MAX_REFLECTIONS] {};
    rays[0] = ray;

    for (auto i = 0; i < MAX_REFLECTIONS; i++) {        
        hits[i].distance = std::numeric_limits<float>::max();
        RaycastHit hit{};
        for (Sphere& sphere : scene->spheres) {
            if (raySphereIntersect(rays[i], sphere, hit) &&
                    hit.distance < hits[i].distance) 
            {
                hits[i] = hit;
                hitColors[i] = sphere.color;
                rays[i+1] = {hit.position, glm::reflect(rays[i].direction, hit.normal)};
            }
        }
    }

    for (int i = MAX_REFLECTIONS - 1; i >= 0; i--) {
        if (hits[i].distance > MIN_HIT_DISTANCE) { // has hit
            float reflectionIntensity = glm::dot(-rays[i].direction, hits[i].normal);
            lightColor *= reflectionIntensity;
            lightColor += scene->lightData.ambientLightColor;
            lightColor += shadowRay(hits[i]);
            lightColor *= hitColors[i];
            lightColor = glm::clamp(lightColor, BLACK, WHITE);
        }
    }

    return lightColor;
}

glm::vec3 SequentialRenderer::shadowRay(RaycastHit hit) {
    Ray shadowRay = {hit.position, - scene->lightData.directionalLightDirection};

    for (Sphere& sphere : scene->spheres) {
        RaycastHit shadowHit{};
        if (raySphereIntersect(shadowRay, sphere, shadowHit)) {
            return BLACK;
        }
    }

    float intensity = glm::dot(shadowRay.direction, hit.normal);
    return intensity * scene->lightData.directionalLightColor;
}

bool SequentialRenderer::raySphereIntersect(Ray& ray, Sphere& sphere, RaycastHit& hit) {
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

    if(x1 > MIN_HIT_DISTANCE) {
        hit.distance = x1;
    } else if(x2 > MIN_HIT_DISTANCE) {
        hit.distance = x2;
    } else {
        return false;
    }

    hit.position = ray.origin + ray.direction * hit.distance;
    hit.normal = glm::normalize(hit.position - sphere.position);

    return true;
}

} // namespace rte
