#pragma once

#include <arm_neon.h>
#include <glm/glm.hpp>

namespace rte {

struct Vec3x4 {
    float32x4_t x;
    float32x4_t y;
    float32x4_t z;

    Vec3x4 operator+(const Vec3x4& other) {
        return {
            vaddq_f32(x, other.x),
            vaddq_f32(y, other.y),
            vaddq_f32(z, other.z)
        };
    }

    Vec3x4& operator+=(const Vec3x4& other) {
        x = vaddq_f32(x, other.x);
        y = vaddq_f32(y, other.y);
        z = vaddq_f32(z, other.z);
        return *this;
    }

    Vec3x4 operator-(const Vec3x4& other) {
        return {
            vsubq_f32(x, other.x),
            vsubq_f32(y, other.y),
            vsubq_f32(z, other.z)
        };
    }

    Vec3x4 operator*(const float32x4_t scalar) {
        return {
            vmulq_f32(x, scalar),
            vmulq_f32(y, scalar),
            vmulq_f32(z, scalar)
        };
    }

    Vec3x4& operator*=(const Vec3x4& other) {
        x = vmulq_f32(x, other.x);
        y = vmulq_f32(y, other.y);
        z = vmulq_f32(z, other.z);
        return *this;
    }

    Vec3x4 operator-() {
        return {
            vnegq_f32(x),
            vnegq_f32(y),
            vnegq_f32(z)
        };
    }

    Vec3x4 normalized();

    Vec3x4(float32x4_t x, float32x4_t y, float32x4_t z) : x{x}, y{y}, z{z} {}

    Vec3x4(glm::vec3 vec) {
        x = vdupq_n_f32(vec.x);
        y = vdupq_n_f32(vec.y);
        z = vdupq_n_f32(vec.z);
    }

    Vec3x4(float xyz) {
        x = vdupq_n_f32(xyz);
        y = vdupq_n_f32(xyz);
        z = vdupq_n_f32(xyz);
    }

    Vec3x4() {}
};

float32x4_t dot_x4(Vec3x4& a, Vec3x4& b);

// inline Vec3x4 select(uint32x4_t mask, Vec3x4 v1, Vec3x4 v2) {

// }

} // namespace rte
