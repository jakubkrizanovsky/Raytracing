#ifdef __ARM_NEON__

#include "simd_renderer.hpp"
#include <core/constants.hpp>

#include <tbb/parallel_for.h>

// std
#include <vector>
#include <iostream>

namespace rte {

constexpr uint MAX_REFLECTIONS = 5;
constexpr float MIN_HIT_DISTANCE = 0.001f;

void SIMDRenderer::prepareFrame() {
    camera->updateCameraData(scene->camera);
    camera->prepareFrame(extent);
    uint8_t* data = reinterpret_cast<uint8_t*>(stagingData);
    
    tbb::parallel_for(static_cast<uint32_t>(0), extent.height, [&](uint32_t y) {
    //for (auto y = 0; y < extent.height; y++) {
        for (auto x = 0; x < extent.width; x+=4) {
            uint32_t pixelIndex = x + y * extent.width;

            Ray_x4 ray = camera->getRay(x, y);
            Vec3_x4 pixelColor = raycast(ray);
            //Vec3_x4 pixelColor = raycastRecursive(ray, MAX_REFLECTIONS);

            uint8x16_t bgra = packBGRA(pixelColor);
            vst1q_u8(&data[pixelIndex * 4], bgra);
        }
    });
}

void SIMDRenderer::setScene(std::shared_ptr<Scene> newScene) {
    CpuRenderer::setScene(newScene);
    camera = std::make_unique<SIMDCamera>(scene->camera);
    inverseLightDirection = Vec3_x4(-scene->lightData.directionalLightDirection);
    ambientLight = Vec3_x4(scene->lightData.ambientLightColor);
}

Vec3_x4 SIMDRenderer::raycast(Ray_x4 ray) {
    Vec3_x4 lightColor = Vec3_x4(BLACK);
    Vec3_x4 throughput = Vec3_x4(WHITE);

    for (auto i = 0; i < MAX_REFLECTIONS; i++) {

        RaycastHit_x4 hit{};
        hit.distance = vdupq_n_f32(std::numeric_limits<float>::max());
        hit.mask = vdupq_n_u32(0);
        Vec3_x4 hitColor;

        RaycastHit_x4 currentHit{};

        for (Sphere& sphere : scene->spheres) {
            uint32x4_t hitMask = raySphereIntersect(ray, sphere, &currentHit);
            uint32x4_t closerMask = vcltq_f32(currentHit.distance, hit.distance);
            uint32x4_t mask = vandq_u32(hitMask, closerMask);

            // select hit values based on mask
            hit.normal = currentHit.normal.select(mask, hit.normal);
            hit.position = currentHit.position.select(mask, hit.position);
            hit.distance = vbslq_f32(mask, currentHit.distance, hit.distance);
            hit.mask = vorrq_u32(hit.mask, hitMask);

            Vec3_x4 sphereColor(sphere.color);
            hitColor = sphereColor.select(mask, hitColor);
        }

        // stop if none of the rays hit
        if (vmaxvq_u32(hit.mask) == 0) {
            break;
        }

        Vec3_x4 rayDir = -ray.direction;
        float32x4_t reflectionIntensity = dot_x4(rayDir, hit.normal);
        reflectionIntensity = vmulq_f32(vdupq_n_f32(DIFFUSE_REFLECTION_CONSTANT), reflectionIntensity);

        // calculate light that will be added in this iteration
        Vec3_x4 addLightColor = Vec3_x4(BLACK);
        addLightColor += ambientLight;
        addLightColor += shadowRay(hit);
        addLightColor *= hitColor;
        addLightColor += specular(ray, hit);
        addLightColor *= throughput;

        // mask if not hit
        addLightColor = addLightColor.select(hit.mask, vdupq_n_f32(0.0f));

        // add light to final pixel color
        lightColor += addLightColor;
        lightColor = lightColor.clamp01();

        // create ray for next interation
        ray.origin = hit.position;
        ray.direction = reflect(ray.direction, hit.normal);

        // adjust throughput for next interation
        throughput *= reflectionIntensity * hitColor;
    }

    return lightColor;
}

Vec3_x4 SIMDRenderer::raycastOld(Ray_x4 sceneRay) {
    Vec3_x4 lightColor = Vec3_x4(BLACK);

    Ray_x4 rays[MAX_REFLECTIONS + 1];
    RaycastHit_x4 hits[MAX_REFLECTIONS] {};
    Vec3_x4 hitColors[MAX_REFLECTIONS] {};

    rays[0] = sceneRay;

    for (auto i = 0; i < MAX_REFLECTIONS; i++) {

        hits[i].distance = vdupq_n_f32(std::numeric_limits<float>::max());
        hits[i].mask = vdupq_n_u32(0);

        RaycastHit_x4 hit{};

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

            Vec3_x4 sphereColor(sphere.color);
            hitColors[i].x = vbslq_f32(mask, sphereColor.x, hitColors[i].x);
            hitColors[i].y = vbslq_f32(mask, sphereColor.y, hitColors[i].y);
            hitColors[i].z = vbslq_f32(mask, sphereColor.z, hitColors[i].z);
        }

        rays[i+1].origin = hits[i].position;
        rays[i+1].direction = reflect(rays[i].direction, hits[i].normal);
    }

    for (int i = MAX_REFLECTIONS - 1; i >= 0; i--) {
        Vec3_x4 rayDir = -rays[i].direction;
        float32x4_t reflectionIntensity = dot_x4(rayDir, hits[i].normal);
        reflectionIntensity = vmulq_f32(vdupq_n_f32(DIFFUSE_REFLECTION_CONSTANT), reflectionIntensity);
        lightColor *= reflectionIntensity;
        lightColor += ambientLight;
        lightColor += shadowRay(hits[i]);
        lightColor *= hitColors[i];
        lightColor += specular(rays[i], hits[i]);
        lightColor = lightColor.clamp01();

        //mask if not hit
        static const float32x4_t black = vdupq_n_f32(0.0f);
        lightColor.x = vbslq_f32(hits[i].mask, lightColor.x, black);
        lightColor.y = vbslq_f32(hits[i].mask, lightColor.y, black);
        lightColor.z = vbslq_f32(hits[i].mask, lightColor.z, black);
    }


    return lightColor;
}

Vec3_x4 SIMDRenderer::raycastRecursive(Ray_x4 ray, unsigned int recursive) {
    Vec3_x4 lightColor = Vec3_x4(BLACK);
    if(recursive == 0) {
        return lightColor;
    }

    RaycastHit_x4 hit{};
    hit.distance = vdupq_n_f32(std::numeric_limits<float>::max());
    hit.mask = vdupq_n_u32(0);

    Vec3_x4 hitColor;

    RaycastHit_x4 currentHit{};
    for (Sphere& sphere : scene->spheres) {
        uint32x4_t hitMask = raySphereIntersect(ray, sphere, &currentHit);
        uint32x4_t closerMask = vcltq_f32(currentHit.distance, hit.distance);
        uint32x4_t mask = vandq_u32(hitMask, closerMask);

        // select hit values based on mask
        hit.normal.x = vbslq_f32(mask, currentHit.normal.x, hit.normal.x);
        hit.normal.y = vbslq_f32(mask, currentHit.normal.y, hit.normal.y);
        hit.normal.z = vbslq_f32(mask, currentHit.normal.z, hit.normal.z);
        hit.position.x = vbslq_f32(mask, currentHit.position.x, hit.position.x);
        hit.position.y = vbslq_f32(mask, currentHit.position.y, hit.position.y);
        hit.position.z = vbslq_f32(mask, currentHit.position.z, hit.position.z);
        hit.distance = vbslq_f32(mask, currentHit.distance, hit.distance);
        hit.mask = vorrq_u32(hit.mask, hitMask);

        Vec3_x4 sphereColor(sphere.color);
        hitColor.x = vbslq_f32(mask, sphereColor.x, hitColor.x);
        hitColor.y = vbslq_f32(mask, sphereColor.y, hitColor.y);
        hitColor.z = vbslq_f32(mask, sphereColor.z, hitColor.z);
    }

    //check if any ray hit
    if (vmaxvq_u32(hit.mask) > 0) {
        Ray_x4 reflectionRay = {hit.position, reflect(ray.direction, hit.normal)};
        Vec3_x4 reflectedColor = raycastRecursive(reflectionRay, recursive - 1);

        float32x4_t reflectionIntensity = dot_x4(-ray.direction, hit.normal);
        reflectionIntensity = vmulq_f32(vdupq_n_f32(DIFFUSE_REFLECTION_CONSTANT), reflectionIntensity);

        lightColor += reflectionIntensity * reflectedColor;
        lightColor += ambientLight;
        lightColor += shadowRay(hit);
        lightColor *= hitColor;
        lightColor += specular(ray, hit);
        lightColor = lightColor.clamp01();

        //mask if not hit
        static const float32x4_t black = vdupq_n_f32(0.0f);
        lightColor.x = vbslq_f32(hit.mask, lightColor.x, black);
        lightColor.y = vbslq_f32(hit.mask, lightColor.y, black);
        lightColor.z = vbslq_f32(hit.mask, lightColor.z, black);
    }

    return lightColor;
}

Vec3_x4 SIMDRenderer::shadowRay(RaycastHit_x4 hit) {
    Ray_x4 shadowRay = {hit.position, inverseLightDirection};

    uint32x4_t mask = vdupq_n_u32(0);
    for (Sphere& sphere : scene->spheres) {
        RaycastHit_x4 shadowHit;
        uint32x4_t hitMask = raySphereIntersect(shadowRay, sphere, &shadowHit);
        mask = vorrq_u32(mask, hitMask);
    }

    float32x4_t intensity = dot_x4(shadowRay.direction, hit.normal);
    intensity = vbslq_f32(mask, vdupq_n_f32(0.0f), intensity);
    return intensity * Vec3_x4(scene->lightData.directionalLightColor);
}

Vec3_x4 SIMDRenderer::specular(Ray_x4 &ray, RaycastHit_x4 &hit) {
    Vec3_x4 lightReflection = reflect(-inverseLightDirection, hit.normal);

    float32x4_t phongIntensity = dot_x4(lightReflection, -ray.direction);
    phongIntensity = vmaxq_f32(vdupq_n_f32(0.0f), vminq_f32(vdupq_n_f32(1.0f), phongIntensity));

    // exponentiation by squaring
    for(int i = 0; i < SPECULAR_EXPONENT_POW2; i++) {
        phongIntensity = vmulq_f32(phongIntensity, phongIntensity);
    }

    return Vec3_x4(SPECULAR_REFLECTION_CONSTANT * phongIntensity);
}

uint32x4_t SIMDRenderer::raySphereIntersect(Ray_x4 ray, Sphere &sphere, RaycastHit_x4* hit) {
    Vec3_x4 spherePosition = Vec3_x4(sphere.position);

    Vec3_x4 positionDelta = spherePosition - ray.origin;

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

uint8x16_t SIMDRenderer::packBGRA(Vec3_x4& color) {
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

Vec3_x4 SIMDRenderer::reflect(Vec3_x4 rayDirection, Vec3_x4 normal) {
    return rayDirection - 2.0f * normal * dot_x4(rayDirection, normal);
}

} // namespace rte

#endif // __ARM_NEON__
