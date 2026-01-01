#pragma once
#ifdef __ARM_NEON__

#include <arm_neon.h>
#include <glm/glm.hpp>

namespace rte {

struct Vec3_x4 {
    float32x4_t x;
    float32x4_t y;
    float32x4_t z;

    Vec3_x4 operator+(const Vec3_x4& other) const {
        return {
            vaddq_f32(x, other.x),
            vaddq_f32(y, other.y),
            vaddq_f32(z, other.z)
        };
    }

    Vec3_x4& operator+=(const Vec3_x4& other) {
        x = vaddq_f32(x, other.x);
        y = vaddq_f32(y, other.y);
        z = vaddq_f32(z, other.z);
        return *this;
    }

    Vec3_x4 operator-(const Vec3_x4& other) const {
        return {
            vsubq_f32(x, other.x),
            vsubq_f32(y, other.y),
            vsubq_f32(z, other.z)
        };
    }

    Vec3_x4& operator-=(const Vec3_x4& other) {
        x = vsubq_f32(x, other.x);
        y = vsubq_f32(y, other.y);
        z = vsubq_f32(z, other.z);
        return *this;
    }

    Vec3_x4 operator-() const {
        return {
            vnegq_f32(x),
            vnegq_f32(y),
            vnegq_f32(z)
        };
    }

    Vec3_x4 operator*(const Vec3_x4& other) const {
        return {
            vmulq_f32(x, other.x),
            vmulq_f32(y, other.y),
            vmulq_f32(z, other.z)
        };
    }

    Vec3_x4 operator*(const float32x4_t scalar) const {
        return {
            vmulq_f32(x, scalar),
            vmulq_f32(y, scalar),
            vmulq_f32(z, scalar)
        };
    }

    Vec3_x4 operator*(const float scalar) const {
        float32x4_t scalar_x4 = vdupq_n_f32(scalar);
        return {
            vmulq_f32(x, scalar_x4),
            vmulq_f32(y, scalar_x4),
            vmulq_f32(z, scalar_x4)
        };
    }

    Vec3_x4& operator*=(const Vec3_x4& other) {
        x = vmulq_f32(x, other.x);
        y = vmulq_f32(y, other.y);
        z = vmulq_f32(z, other.z);
        return *this;
    }

    Vec3_x4& operator*=(const float32x4_t scalar) {
        x = vmulq_f32(x, scalar);
        y = vmulq_f32(y, scalar);
        z = vmulq_f32(z, scalar);
        return *this;
    }

    Vec3_x4 normalized() const;
    Vec3_x4 clamp01() const;
    Vec3_x4 select(uint32x4_t mask, const Vec3_x4& other) const;
    Vec3_x4 select(uint32x4_t mask, const float32x4_t otherXYZ) const;

    Vec3_x4(float32x4_t x, float32x4_t y, float32x4_t z) : x{x}, y{y}, z{z} {}

    explicit Vec3_x4(glm::vec3 vec) {
        x = vdupq_n_f32(vec.x);
        y = vdupq_n_f32(vec.y);
        z = vdupq_n_f32(vec.z);
    }

    explicit Vec3_x4(float32x4_t xyz) : x(xyz), y(xyz), z(xyz) {}

    explicit Vec3_x4(float xyz) : x(vdupq_n_f32(xyz)), 
            y(vdupq_n_f32(xyz)), 
            z(vdupq_n_f32(xyz)) {}

    Vec3_x4() : x(vdupq_n_f32(0.0f)), 
            y(vdupq_n_f32(0.0f)), 
            z(vdupq_n_f32(0.0f)) {}
};

float32x4_t dot_x4(const Vec3_x4& a, const Vec3_x4& b);

inline Vec3_x4 operator*(const float32x4_t scalar, const Vec3_x4& vec) {
    return vec * scalar;
}

inline Vec3_x4 operator*(float scalar, const Vec3_x4& vec) {
    return vec * scalar;
}

} // namespace rte

#endif // __ARM_NEON__
