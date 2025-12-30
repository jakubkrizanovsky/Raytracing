#ifdef __ARM_NEON__

#include "simd_renderer.hpp"
#include <core/constants.hpp>

#include <tbb/parallel_for.h>

// std
#include <vector>
#include <iostream>

namespace rte {

constexpr uint MAX_REFLECTIONS = 5;
constexpr float MIN_HIT_DISTANCE = 0.005f;

void SIMDRenderer::prepareFrame() {
    camera->updateCameraData(scene->camera);
    camera->prepareFrame(extent);
    uint8_t* data = reinterpret_cast<uint8_t*>(stagingData);
    
    tbb::parallel_for(static_cast<uint32_t>(0), extent.height, [&](uint32_t y) {
    //for (auto y = 0; y < extent.height; y++) {
        for (auto x = 0; x < extent.width; x+=8) {
            uint32_t pixelIndex = x + y * extent.width;

            Ray_x8 ray = camera->getRay(x, y);
            Vec3_x8 pixelColor = raycast(ray);

            uint8x8x4_t bgra = packBGRA(pixelColor);
            vst4_u8(&data[pixelIndex * 4], bgra);
        }
    });
}

void SIMDRenderer::setScene(std::shared_ptr<Scene> newScene) {
    CpuRenderer::setScene(newScene);
    camera = std::make_unique<SIMDCamera>(scene->camera);
    inverseLightDirection = Vec3_x8(-scene->lightData.directionalLightDirection);
    ambientLight = Vec3_x8(scene->lightData.ambientLightColor);
}

Vec3_x8 SIMDRenderer::raycast(Ray_x8 sceneRay) {
    Vec3_x8 lightColor = BLACK;

    Ray_x8 rays[MAX_REFLECTIONS + 1];
    RaycastHit_x8 hits[MAX_REFLECTIONS] {};
    Vec3_x8 hitColors[MAX_REFLECTIONS] {};

    rays[0] = sceneRay;

    for (auto i = 0; i < MAX_REFLECTIONS; i++) {

        hits[i].distance = vdupq_n_f16(std::numeric_limits<float>::max());
        hits[i].mask = vdupq_n_u16(0);

        bool hasHit = false;
        RaycastHit_x8 hit{};

        for (Sphere& sphere : scene->spheres) {
            uint16x8_t hitMask = raySphereIntersect(rays[i], sphere, &hit);
            uint16x8_t closerMask = vcltq_f16(hit.distance, hits[i].distance);
            uint16x8_t mask = vandq_u16(hitMask, closerMask);

            // select hit values based on mask
            hits[i].normal.x = vbslq_f16(mask, hit.normal.x, hits[i].normal.x);
            hits[i].normal.y = vbslq_f16(mask, hit.normal.y, hits[i].normal.y);
            hits[i].normal.z = vbslq_f16(mask, hit.normal.z, hits[i].normal.z);
            hits[i].position.x = vbslq_f16(mask, hit.position.x, hits[i].position.x);
            hits[i].position.y = vbslq_f16(mask, hit.position.y, hits[i].position.y);
            hits[i].position.z = vbslq_f16(mask, hit.position.z, hits[i].position.z);
            hits[i].distance = vbslq_f16(mask, hit.distance, hits[i].distance);
            hits[i].mask = vorrq_u16(hits[i].mask, hitMask);

            Vec3_x8 sphereColor(sphere.color);
            hitColors[i].x = vbslq_f16(mask, sphereColor.x, hitColors[i].x);
            hitColors[i].y = vbslq_f16(mask, sphereColor.y, hitColors[i].y);
            hitColors[i].z = vbslq_f16(mask, sphereColor.z, hitColors[i].z);
        }

        rays[i+1].origin = hits[i].position;
        rays[i+1].direction = reflect(rays[i].direction, hits[i].normal);
    }

    for (int i = MAX_REFLECTIONS - 1; i >= 0; i--) {
        Vec3_x8 rayDir = -rays[i].direction;
        float16x8_t reflectionIntensity = dot_x8(rayDir, hits[i].normal);
        lightColor *= reflectionIntensity;
        lightColor += ambientLight;
        lightColor += shadowRay(hits[i]);
        lightColor *= hitColors[i];

        //clamp01
        lightColor.x = vmaxq_f16(vdupq_n_f16(0.0f), vminq_f16(vdupq_n_f16(1.0f), lightColor.x));
        lightColor.y = vmaxq_f16(vdupq_n_f16(0.0f), vminq_f16(vdupq_n_f16(1.0f), lightColor.y));
        lightColor.z = vmaxq_f16(vdupq_n_f16(0.0f), vminq_f16(vdupq_n_f16(1.0f), lightColor.z));

        //mask if not hit
        static const float16x8_t black = vdupq_n_f16(0.0f);
        lightColor.x = vbslq_f16(hits[i].mask, lightColor.x, black);
        lightColor.y = vbslq_f16(hits[i].mask, lightColor.y, black);
        lightColor.z = vbslq_f16(hits[i].mask, lightColor.z, black);
    }


    return lightColor;
}

Vec3_x8 SIMDRenderer::shadowRay(RaycastHit_x8 hit) {
    Ray_x8 shadowRay = {hit.position, inverseLightDirection};

    uint16x8_t mask = vdupq_n_u16(0);
    for (Sphere& sphere : scene->spheres) {
        RaycastHit_x8 shadowHit;
        uint16x8_t hitMask = raySphereIntersect(shadowRay, sphere, &shadowHit);
        mask = vorrq_u16(mask, hitMask);
    }

    float16x8_t intensity = dot_x8(shadowRay.direction, hit.normal);
    intensity = vbslq_f16(mask, vdupq_n_f16(0.0f), intensity);
    return intensity * scene->lightData.directionalLightColor;
}

uint16x8_t SIMDRenderer::raySphereIntersect(Ray_x8 ray, Sphere &sphere, RaycastHit_x8* hit) {
    Vec3_x8 spherePosition = {
        vdupq_n_f16(sphere.position.x),
        vdupq_n_f16(sphere.position.y),
        vdupq_n_f16(sphere.position.z)
    };

    Vec3_x8 positionDelta = spherePosition - ray.origin;

    float16x8_t a = dot_x8(ray.direction, ray.direction);
    float16x8_t b = vmulq_f16(vdupq_n_f16(-2.0f), dot_x8(positionDelta, ray.direction));
    float16x8_t c = vsubq_f16(dot_x8(positionDelta, positionDelta), 
            vdupq_n_f16(sphere.radius * sphere.radius));

    float16x8_t determinant = vsubq_f16(vmulq_f16(b, b),
            vmulq_f16(vdupq_n_f16(4.0f), vmulq_f16(a, c)));

    float16x8_t x1 = vdivq_f16(vaddq_f16(vsqrtq_f16(determinant), b), vmulq_f16(vdupq_n_f16(-2.0f), a));
    float16x8_t x2 = vdivq_f16(vsubq_f16(vsqrtq_f16(determinant), b), vmulq_f16(vdupq_n_f16(2.0f), a));

    uint16x8_t mask = vcgtq_f16(determinant, vdupq_n_f16(0));
    x1 = vbslq_f16(mask, x1, vdupq_n_f16(std::numeric_limits<float>::max()));
    x2 = vbslq_f16(mask, x2, vdupq_n_f16(std::numeric_limits<float>::max()));

    uint16x8_t maskX1 = vcgtq_f16(x1, vdupq_n_f16(MIN_HIT_DISTANCE));
    x1 = vbslq_f16(maskX1, x1, vdupq_n_f16(std::numeric_limits<float>::max()));
    uint16x8_t maskX2 = vcgtq_f16(x2, vdupq_n_f16(MIN_HIT_DISTANCE));
    x2 = vbslq_f16(maskX2, x2, vdupq_n_f16(std::numeric_limits<float>::max()));

    mask = vandq_u16(mask, vorrq_u16(maskX1, maskX2));

    hit->distance = vminq_f16(x1, x2);
    hit->position = ray.origin + ray.direction * hit->distance;
    hit->normal = (hit->position - spherePosition).normalized();
    hit->mask = mask;

    return mask;
}

uint8x8x4_t SIMDRenderer::packBGRA(Vec3_x8& color) {
    // scale to range 0-255
    color *= vdupq_n_f16(255.0f);

    // convert to uint8
    uint16x8_t r16 = vcvtq_u16_f16(color.x);
    uint16x8_t g16 = vcvtq_u16_f16(color.y);
    uint16x8_t b16 = vcvtq_u16_f16(color.z);

    uint8x8_t r8 = vmovn_u16(r16);
    uint8x8_t g8 = vmovn_u16(g16);
    uint8x8_t b8 = vmovn_u16(b16);
    uint8x8_t a8 = vdup_n_u8(255);

    return { b8, g8, r8, a8 };
}

Vec3_x8 SIMDRenderer::reflect(Vec3_x8 rayDirection, Vec3_x8 normal) {
    return rayDirection - 2.0f * normal * dot_x8(rayDirection, normal);
}

} // namespace rte

#endif // __ARM_NEON__
