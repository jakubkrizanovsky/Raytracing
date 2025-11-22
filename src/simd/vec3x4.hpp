#pragma once

#include <arm_neon.h>

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

    Vec3x4 normalize() {
        float32x4_t length = vaddq_f32(vaddq_f32(x, y), z);
        return {
            vdivq_f32(x, length),
            vdivq_f32(y, length),
            vdivq_f32(z, length)
        };
    }
};

inline float32x4_t dot_x4(Vec3x4& a, Vec3x4& b) {
    float32x4_t xx = vmulq_f32(a.x, b.x);
    float32x4_t yy = vmulq_f32(a.y, b.y);
    float32x4_t zz = vmulq_f32(a.z, b.z);

    return vaddq_f32(vaddq_f32(xx, yy), zz);
}

} // namespace rte
