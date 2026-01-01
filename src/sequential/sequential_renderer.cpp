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
    camera->updateCameraData(scene->camera);
    camera->prepareFrame(extent);

    uint8_t* data = reinterpret_cast<uint8_t*>(stagingData);
    for (auto y = 0; y < extent.height; y++) {
        for (auto x = 0; x < extent.width; x++) {
            Ray ray = camera->getRay(x, y);
            glm::vec3 pixelColor = raycast(ray);
            
            uint32_t pixelIndex = (x + y * extent.width) * 4;
            data[pixelIndex + 0] = static_cast<uint8_t>(pixelColor.b * 255);
            data[pixelIndex + 1] = static_cast<uint8_t>(pixelColor.g * 255);
            data[pixelIndex + 2] = static_cast<uint8_t>(pixelColor.r * 255);
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
    glm::vec3 throughput = WHITE;

    for (auto i = 0; i < MAX_REFLECTIONS; i++) {  
        RaycastHit hit{};      
        hit.distance = std::numeric_limits<float>::max();
        glm::vec3 hitColor;

        RaycastHit currentHit{};
        for (Sphere& sphere : scene->spheres) {
            if (raySphereIntersect(ray, sphere, currentHit) &&
                    currentHit.distance < hit.distance) 
            {
                hit = currentHit;
                hitColor = sphere.color;
            }
        }

        if (hit.hit) {
            float reflectionIntensity = DIFFUSE_REFLECTION_CONSTANT * glm::dot(-ray.direction, hit.normal);
            
            lightColor += throughput * hitColor * scene->lightData.ambientLightColor;
            lightColor += throughput * hitColor * shadowRay(hit);
            lightColor += throughput * specular(ray, hit);
            
            lightColor = glm::clamp(lightColor, BLACK, WHITE);

            ray = {hit.position, glm::reflect(ray.direction, hit.normal)};
            
            throughput *= reflectionIntensity * hitColor;
        } else {
            break;
        }
    }

    return lightColor;
}

glm::vec3 SequentialRenderer::raycastOld(Ray &ray) {
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
        if (hits[i].hit) {
            float reflectionIntensity = DIFFUSE_REFLECTION_CONSTANT * glm::dot(-rays[i].direction, hits[i].normal);
            lightColor *= reflectionIntensity;
            lightColor += scene->lightData.ambientLightColor;
            lightColor += shadowRay(hits[i]);
            lightColor *= hitColors[i];
            lightColor += specular(rays[i], hits[i]);

            lightColor = glm::clamp(lightColor, BLACK, WHITE);
        }
    }

    return lightColor;
}

glm::vec3 SequentialRenderer::raycastRecursive(Ray &ray, unsigned int recursive) {
    glm::vec3 lightColor = BLACK;
    if(recursive == 0) {
        return lightColor;
    }

    RaycastHit hit{};
    hit.distance = std::numeric_limits<float>::max();
    glm::vec3 hitColor;

    RaycastHit currentHit{};
    for (Sphere& sphere : scene->spheres) {
        if (raySphereIntersect(ray, sphere, currentHit) && currentHit.distance < hit.distance) {
            hit = currentHit;
            hitColor = sphere.color;
        }
    }

    if (hit.hit) {
        Ray reflectionRay = {hit.position, glm::reflect(ray.direction, hit.normal)};
        glm::vec3 reflectedColor = raycastRecursive(reflectionRay, recursive - 1);

        float reflectionIntensity = DIFFUSE_REFLECTION_CONSTANT * glm::dot(-ray.direction, hit.normal);
        lightColor += reflectionIntensity * reflectedColor;
        lightColor += scene->lightData.ambientLightColor;
        lightColor += shadowRay(hit);
        lightColor *= hitColor;
        lightColor += specular(ray, hit);

        lightColor = glm::clamp(lightColor, BLACK, WHITE);
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

glm::vec3 SequentialRenderer::specular(Ray &ray, RaycastHit &hit) {
    glm::vec3 lightReflection = glm::reflect(scene->lightData.directionalLightDirection, hit.normal);

    float phongIntensity = glm::dot(lightReflection, -ray.direction);
    phongIntensity = glm::clamp(phongIntensity, 0.0f, 1.0f);

    return SPECULAR_REFLECTION_CONSTANT * pow(phongIntensity, SPECULAR_EXPONENT) * WHITE;
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
    hit.hit = true;

    return true;
}

} // namespace rte
