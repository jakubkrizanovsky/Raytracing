#include "simd_renderer.hpp"

#include "vec3_field.hpp"
#include "vec3x4.hpp"
#include "rayx4.hpp"

// std
#include <vector>
#include <iostream>

namespace rte {

constexpr glm::vec3 ZERO = {0, 0, 0};
constexpr glm::vec3 RIGHT = {1, 0, 0};
constexpr glm::vec3 UP = {0, 1, 0};
constexpr glm::vec3 FORWARD = {0, 0, 1};
constexpr glm::vec3 ONE = {1, 1, 1};
constexpr uint MAX_REFLECTIONS = 5;
constexpr float MIN_HIT_DISTANCE = 0.001f;

void SIMDRenderer::prepareFrame() {
    const glm::vec3 cameraPosition = {0, 0, -10};
    const glm::vec3 cameraForward = FORWARD;
    const float cameraSizeX = 4.0f;
    const float cameraSizeY = 3.0f;

    const glm::vec3 topLeft = cameraPosition - 0.5f * cameraSizeX * RIGHT + 0.5f * cameraSizeY * UP;

    const float xDiff = cameraSizeX / extent.width;
    const float yDiff = cameraSizeY / extent.height;
    
    RayField rays {};
    for (auto y = 0; y < extent.height; y++) {
        for (auto x = 0; x < extent.width; x++) {
            uint32_t index = x + y * extent.width;
            glm::vec3 pixelPosition = topLeft + x * xDiff * RIGHT - y * yDiff * UP;

            rays.origins.x.push_back(pixelPosition.x);
            rays.origins.y.push_back(pixelPosition.y);
            rays.origins.z.push_back(pixelPosition.z);

            rays.directions.x.push_back(cameraForward.x);
            rays.directions.y.push_back(cameraForward.y);
            rays.directions.z.push_back(cameraForward.z);
        }
    }

    Vec3Field pixelColors = raycast(rays);

    uint8_t* data = reinterpret_cast<uint8_t*>(stagingData);

    for (uint32_t i = 0; i < extent.width * extent.height; i++) {
        data[4 * i + 2] = static_cast<uint8_t>(pixelColors.x[i] * 255);
        data[4 * i + 1] = static_cast<uint8_t>(pixelColors.y[i] * 255);
        data[4 * i + 0] = static_cast<uint8_t>(pixelColors.z[i] * 255);
        data[4 * i + 3] = 255;
    }
}

Vec3Field SIMDRenderer::raycast(RayField sceneRays) {
    Vec3Field lightColors {};
    lightColors.x.resize(sceneRays.origins.x.size());
    lightColors.y.resize(sceneRays.origins.y.size());
    lightColors.z.resize(sceneRays.origins.z.size());

    for(auto a = 0; a < sceneRays.origins.x.size(); a += 4) {

        Rayx4 rays[MAX_REFLECTIONS + 1];
        RaycastHitx4 hits[MAX_REFLECTIONS] {};
        Vec3x4 hitColors[MAX_REFLECTIONS] {};

        float32x4_t originsX = vld1q_f32(&sceneRays.origins.x[a]);
        float32x4_t originsY = vld1q_f32(&sceneRays.origins.y[a]);
        float32x4_t originsZ = vld1q_f32(&sceneRays.origins.z[a]);
        Vec3x4 origins = {originsX, originsY, originsZ};

        float32x4_t directionsX = vld1q_f32(&sceneRays.directions.x[a]);
        float32x4_t directionsY = vld1q_f32(&sceneRays.directions.y[a]);
        float32x4_t directionsZ = vld1q_f32(&sceneRays.directions.z[a]);
        Vec3x4 directions = {directionsX, directionsY, directionsZ};

        rays[0] = {origins, directions};

        const Vec3x4 white = {1.0f};
        const Vec3x4 black = {0.0f};
        Vec3x4 lightColor(0.0f);

        //for (auto i = 0; i < MAX_REFLECTIONS; i++) {

            hits[0].distance = vdupq_n_f32(std::numeric_limits<float>::max());

            bool hasHit = false;
            RaycastHitx4 hit{};

            for (Sphere& sphere : spheres) {
                uint32x4_t hitMask = raySphereIntersect(rays[0], sphere, &hit);
                uint32x4_t closerMask = vcltq_f32(hit.distance, hits[0].distance);
                uint32x4_t mask = vandq_u32(hitMask, closerMask);

                // select hit values based on mask
                hits[0].normal.x = vbslq_f32(mask, hit.normal.x, hits[0].normal.x);
                hits[0].normal.y = vbslq_f32(mask, hit.normal.y, hits[0].normal.y);
                hits[0].normal.z = vbslq_f32(mask, hit.normal.z, hits[0].normal.z);
                hits[0].position.x = vbslq_f32(mask, hit.position.x, hits[0].position.x);
                hits[0].position.y = vbslq_f32(mask, hit.position.y, hits[0].position.y);
                hits[0].position.z = vbslq_f32(mask, hit.position.z, hits[0].position.z);
                hits[0].distance = vbslq_f32(mask, hit.distance, hits[0].distance);
                hits[0].mask = vorrq_u32(hits[0].mask, hitMask);

                Vec3x4 sphereColor(sphere.color);
                hitColors[0].x = vbslq_f32(mask, sphereColor.x, hitColors[0].x);
                hitColors[0].y = vbslq_f32(mask, sphereColor.y, hitColors[0].y);
                hitColors[0].z = vbslq_f32(mask, sphereColor.z, hitColors[0].z);

                //TODO - calculate next ray
                //rays[i+1] = {hit.position, reflect(rays[i].direction, hit.normal)};
            }
        //}

        // for (int i = MAX_REFLECTIONS - 1; i >= 0; i--) {
            Vec3x4 rayDir = -rays[0].direction;
            float32x4_t reflectionIntensity = dot_x4(rayDir, hits[0].normal);
            reflectionIntensity = vmaxq_f32(vdupq_n_f32(0.0f), reflectionIntensity);
            lightColor = lightColor * reflectionIntensity;
            lightColor += ambientLight;
            lightColor += shadowRay(hits[0]);
            lightColor *= hitColors[0];

            //clamp01
            lightColor.x = vmaxq_f32(vdupq_n_f32(0.0f), vminq_f32(vdupq_n_f32(1.0f), lightColor.x));
            lightColor.y = vmaxq_f32(vdupq_n_f32(0.0f), vminq_f32(vdupq_n_f32(1.0f), lightColor.y));
            lightColor.z = vmaxq_f32(vdupq_n_f32(0.0f), vminq_f32(vdupq_n_f32(1.0f), lightColor.z));

            //mask if not hit
            lightColor.x = vbslq_f32(hits[0].mask, lightColor.x, black.x);
            lightColor.y = vbslq_f32(hits[0].mask, lightColor.y, black.y);
            lightColor.z = vbslq_f32(hits[0].mask, lightColor.z, black.z);
        //}

        vst1q_f32(&lightColors.x[a], lightColor.x);
        vst1q_f32(&lightColors.y[a], lightColor.y);
        vst1q_f32(&lightColors.z[a], lightColor.z);
    }

    return lightColors;
}

Vec3x4 SIMDRenderer::shadowRay(RaycastHitx4 hit) {
    Rayx4 shadowRay = {hit.position, inverseLightDirection};

    uint32x4_t mask;
    for (Sphere& sphere : spheres) {
        RaycastHitx4 shadowHit;
        uint32x4_t hitMask = raySphereIntersect(shadowRay, sphere, &shadowHit);
        mask = vorrq_u32(mask, hitMask);
    }

    float32x4_t intensity = dot_x4(shadowRay.direction, hit.normal);
    intensity = vbslq_f32(mask, vdupq_n_f32(0.0f), intensity);
    return {intensity, intensity, intensity};
}

uint32x4_t SIMDRenderer::raySphereIntersect(Rayx4 ray, Sphere &sphere, RaycastHitx4* hit) {
    Vec3x4 spherePosition = {
        vdupq_n_f32(sphere.position.x),
        vdupq_n_f32(sphere.position.y),
        vdupq_n_f32(sphere.position.z)
    };

    Vec3x4 positionDelta = spherePosition - ray.origin;

    float32x4_t a = dot_x4(ray.direction, ray.direction);
    float32x4_t b = vmulq_f32(vdupq_n_f32(-2.0f), dot_x4(positionDelta, ray.direction));
    float32x4_t c = vsubq_f32(dot_x4(positionDelta, positionDelta), 
            vdupq_n_f32(sphere.radius * sphere.radius));

    float32x4_t determinant = vsubq_f32(vmulq_f32(b, b),
            vmulq_f32(vdupq_n_f32(4.0f), vmulq_f32(a, c)));

    float32x4_t x1 = vdivq_f32(vaddq_f32(vsqrtq_f32(determinant), b), vmulq_f32(vdupq_n_f32(-2.0f), a));
    float32x4_t x2 = vdivq_f32(vsubq_f32(vsqrtq_f32(determinant), b), vmulq_f32(vdupq_n_f32(2.0f), a));

    uint32x4_t mask = vcgtq_f32(determinant, vdupq_n_f32(0));
    x1 = vbslq_f32(mask, x1, vdupq_n_f32(std::numeric_limits<float>::max()));
    x2 = vbslq_f32(mask, x2, vdupq_n_f32(std::numeric_limits<float>::max()));

    uint32x4_t maskX1 = vcgtq_f32(x1, vdupq_n_f32(MIN_HIT_DISTANCE));
    x1 = vbslq_f32(maskX1, x1, vdupq_n_f32(std::numeric_limits<float>::max()));
    uint32x4_t maskX2 = vcgtq_f32(x2, vdupq_n_f32(MIN_HIT_DISTANCE));
    x2 = vbslq_f32(maskX2, x2, vdupq_n_f32(std::numeric_limits<float>::max()));

    mask = vandq_u32(mask, vorrq_u32(maskX1, maskX2));

    float32x4_t distance = vminq_f32(x1, x2);
    Vec3x4 position = ray.origin + ray.direction * distance;
    Vec3x4 normal = (position - spherePosition).normalized();
    
    hit->distance = distance;
    hit->position = position;
    hit->normal = normal;
    hit->mask = mask;

    return mask;
}

glm::vec3 SIMDRenderer::reflect(glm::vec3 rayDirection, glm::vec3 normal) {
    return rayDirection - 2.0f * normal * glm::dot(rayDirection, normal);
}

} // namespace rte
