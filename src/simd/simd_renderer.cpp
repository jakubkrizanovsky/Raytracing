#ifdef __ARM_NEON__

#include "simd_renderer.hpp"
#include <core/constants.hpp>

// std
#include <vector>
#include <iostream>

namespace rte {

constexpr uint MAX_REFLECTIONS = 5;
constexpr float MIN_HIT_DISTANCE = 0.001f;

void SIMDRenderer::prepareFrame() {
    camera->prepareFrame(extent);
    uint8_t* data = reinterpret_cast<uint8_t*>(stagingData);
    for (auto y = 0; y < extent.height; y++) {
        for (auto x = 0; x < extent.width; x+=4) {
            uint32_t pixelIndex = x + y * extent.width;

            Rayx4 ray = camera->getRay(x, y);
            Vec3x4 pixelColor = raycast(ray);

            uint8x16_t bgra = packBGRA(pixelColor);
            vst1q_u8(&data[pixelIndex * 4], bgra);
        }
    }
}

void SIMDRenderer::setScene(std::shared_ptr<Scene> newScene) {
    CpuRenderer::setScene(newScene);
    camera = std::make_unique<SIMDCamera>(scene->camera);
}

Vec3x4 SIMDRenderer::raycast(Rayx4 sceneRay) {
    Vec3x4 lightColor = BLACK;

    Rayx4 rays[MAX_REFLECTIONS + 1];
    RaycastHitx4 hits[MAX_REFLECTIONS] {};
    Vec3x4 hitColors[MAX_REFLECTIONS] {};

    rays[0] = sceneRay;

    for (auto i = 0; i < MAX_REFLECTIONS; i++) {

        hits[i].distance = vdupq_n_f32(std::numeric_limits<float>::max());
        hits[i].mask = vdupq_n_u32(0);

        bool hasHit = false;
        RaycastHitx4 hit{};

        for (Sphere& sphere : scene->spheres) {
            uint32x4_t hitMask = raySphereIntersect(rays[i], sphere, &hit);
            uint32x4_t closerMask = vcltq_f32(hit.distance, hits[i].distance);
            uint32x4_t mask = vandq_u32(hitMask, closerMask);

            // select hit values based on mask
            hits[i].normal.x = vbslq_f32(mask, hit.normal.x, hits[i].normal.x);
            hits[i].normal.y = vbslq_f32(mask, hit.normal.y, hits[i].normal.y);
            hits[i].normal.z = vbslq_f32(mask, hit.normal.z, hits[i].normal.z);
            hits[i].position.x = vbslq_f32(mask, hit.position.x, hits[i].position.x);
            hits[i].position.y = vbslq_f32(mask, hit.position.y, hits[i].position.y);
            hits[i].position.z = vbslq_f32(mask, hit.position.z, hits[i].position.z);
            hits[i].distance = vbslq_f32(mask, hit.distance, hits[i].distance);
            hits[i].mask = vorrq_u32(hits[i].mask, hitMask);

            Vec3x4 sphereColor(sphere.color);
            hitColors[i].x = vbslq_f32(mask, sphereColor.x, hitColors[i].x);
            hitColors[i].y = vbslq_f32(mask, sphereColor.y, hitColors[i].y);
            hitColors[i].z = vbslq_f32(mask, sphereColor.z, hitColors[i].z);
        }

        rays[i+1].origin = hits[i].position;
        rays[i+1].direction = reflect(rays[i].direction, hits[i].normal);
    }

    for (int i = MAX_REFLECTIONS - 1; i >= 0; i--) {
        Vec3x4 rayDir = -rays[i].direction;
        float32x4_t reflectionIntensity = dot_x4(rayDir, hits[i].normal);
        lightColor *= reflectionIntensity;
        lightColor += ambientLight;
        lightColor += shadowRay(hits[i]);
        lightColor *= hitColors[i];

        //clamp01
        lightColor.x = vmaxq_f32(vdupq_n_f32(0.0f), vminq_f32(vdupq_n_f32(1.0f), lightColor.x));
        lightColor.y = vmaxq_f32(vdupq_n_f32(0.0f), vminq_f32(vdupq_n_f32(1.0f), lightColor.y));
        lightColor.z = vmaxq_f32(vdupq_n_f32(0.0f), vminq_f32(vdupq_n_f32(1.0f), lightColor.z));

        //mask if not hit
        static const float32x4_t black = vdupq_n_f32(0.0f);
        lightColor.x = vbslq_f32(hits[i].mask, lightColor.x, black);
        lightColor.y = vbslq_f32(hits[i].mask, lightColor.y, black);
        lightColor.z = vbslq_f32(hits[i].mask, lightColor.z, black);
    }


    return lightColor;
}

Vec3x4 SIMDRenderer::shadowRay(RaycastHitx4 hit) {
    Rayx4 shadowRay = {hit.position, inverseLightDirection};

    uint32x4_t mask = vdupq_n_u32(0);
    for (Sphere& sphere : scene->spheres) {
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

    hit->distance = vminq_f32(x1, x2);
    hit->position = ray.origin + ray.direction * hit->distance;
    hit->normal = (hit->position - spherePosition).normalized();
    hit->mask = mask;

    return mask;
}

uint8x16_t SIMDRenderer::packBGRA(Vec3x4& color) {
    // scale to range 0-255
    color *= vdupq_n_f32(255.0f);

    // convert to uint8
    uint32x4_t r = vcvtq_u32_f32(color.x);
    uint32x4_t g = vcvtq_u32_f32(color.y);
    uint32x4_t b = vcvtq_u32_f32(color.z);

    uint16x4_t r16 = vmovn_u32(r);
    uint16x4_t g16 = vmovn_u32(g);
    uint16x4_t b16 = vmovn_u32(b);

    uint8x8_t r8 = vmovn_u16(vcombine_u16(r16, r16));
    uint8x8_t g8 = vmovn_u16(vcombine_u16(g16, g16));
    uint8x8_t b8 = vmovn_u16(vcombine_u16(b16, b16));
    uint8x8_t a8 = vdup_n_u8(255);

    // interleave to BGRA
    uint8x8_t br = vzip1_u8(b8, r8); // b0 r0 b1 r1 b2 r2 b3 r3
    uint8x8_t ga = vzip1_u8(g8, a8); // g0 a0 g1 a1 g2 a2 g3 a3

    uint8x8_t bgra_lo = vzip1_u8(br, ga); // b0 g0 r0 a0 b1 g1 r1 a1
    uint8x8_t bgra_hi = vzip2_u8(br, ga); // b2 g2 r2 a2 b3 g3 r3 a3

    return vcombine_u8(bgra_lo, bgra_hi);
}

Vec3x4 SIMDRenderer::reflect(Vec3x4 rayDirection, Vec3x4 normal) {
    return rayDirection - 2.0f * normal * dot_x4(rayDirection, normal);
}

} // namespace rte

#endif // __ARM_NEON__
