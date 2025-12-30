#pragma once
#ifdef __ARM_NEON__

#include <arm_neon.h>
#include <glm/glm.hpp>

namespace rte {

struct Vec3_x8 {
    float16x8_t x;
    float16x8_t y;
    float16x8_t z;

    Vec3_x8 operator+(const Vec3_x8& other) const {
        return {
            vaddq_f16(x, other.x),
            vaddq_f16(y, other.y),
            vaddq_f16(z, other.z)
        };
    }

    Vec3_x8& operator+=(const Vec3_x8& other) {
        x = vaddq_f16(x, other.x);
        y = vaddq_f16(y, other.y);
        z = vaddq_f16(z, other.z);
        return *this;
    }

    Vec3_x8 operator-(const Vec3_x8& other) const {
        return {
            vsubq_f16(x, other.x),
            vsubq_f16(y, other.y),
            vsubq_f16(z, other.z)
        };
    }

    Vec3_x8& operator-=(const Vec3_x8& other) {
        x = vsubq_f16(x, other.x);
        y = vsubq_f16(y, other.y);
        z = vsubq_f16(z, other.z);
        return *this;
    }

    Vec3_x8 operator-() const {
        return {
            vnegq_f16(x),
            vnegq_f16(y),
            vnegq_f16(z)
        };
    }

    Vec3_x8 operator*(const Vec3_x8& other) const {
        return {
            vmulq_f16(x, other.x),
            vmulq_f16(y, other.y),
            vmulq_f16(z, other.z)
        };
    }

    Vec3_x8 operator*(const float16x8_t scalar) const {
        return {
            vmulq_f16(x, scalar),
            vmulq_f16(y, scalar),
            vmulq_f16(z, scalar)
        };
    }

    Vec3_x8 operator*(const float scalar) const {
        float16x8_t scalar_x4 = vdupq_n_f16(scalar);
        return {
            vmulq_f16(x, scalar_x4),
            vmulq_f16(y, scalar_x4),
            vmulq_f16(z, scalar_x4)
        };
    }

    Vec3_x8& operator*=(const Vec3_x8& other) {
        x = vmulq_f16(x, other.x);
        y = vmulq_f16(y, other.y);
        z = vmulq_f16(z, other.z);
        return *this;
    }

    Vec3_x8& operator*=(const float16x8_t scalar) {
        x = vmulq_f16(x, scalar);
        y = vmulq_f16(y, scalar);
        z = vmulq_f16(z, scalar);
        return *this;
    }

    Vec3_x8 normalized() const;

    Vec3_x8(float16x8_t x, float16x8_t y, float16x8_t z) : x{x}, y{y}, z{z} {}

    Vec3_x8(glm::vec3 vec) {
        x = vdupq_n_f16(vec.x);
        y = vdupq_n_f16(vec.y);
        z = vdupq_n_f16(vec.z);
    }

    Vec3_x8(float xyz) : x(vdupq_n_f16(xyz)), 
            y(vdupq_n_f16(xyz)), 
            z(vdupq_n_f16(xyz)) {}

    Vec3_x8() : x(vdupq_n_f16(0.0f)), 
            y(vdupq_n_f16(0.0f)), 
            z(vdupq_n_f16(0.0f)) {}
};

float16x8_t dot_x8(const Vec3_x8& a, const Vec3_x8& b);

inline Vec3_x8 operator*(const float16x8_t scalar, const Vec3_x8& vec) {
    return vec * scalar;
}

inline Vec3_x8 operator*(float scalar, const Vec3_x8& vec) {
    return vec * scalar;
}

} // namespace rte

#endif // __ARM_NEON__
