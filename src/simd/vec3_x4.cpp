#ifdef __ARM_NEON__

#include "vec3_x4.hpp"

namespace rte {

float32x4_t dot_x4(const Vec3_x4& a, const Vec3_x4& b) {
    float32x4_t xx = vmulq_f32(a.x, b.x);
    float32x4_t yy = vmulq_f32(a.y, b.y);
    float32x4_t zz = vmulq_f32(a.z, b.z);

    return vaddq_f32(vaddq_f32(xx, yy), zz);
}

Vec3_x4 Vec3_x4::normalized() const {
    float32x4_t length = vsqrtq_f32(dot_x4(*this, *this));
    return {
        vdivq_f32(x, length),
        vdivq_f32(y, length),
        vdivq_f32(z, length)
    };
}

Vec3_x4 Vec3_x4::clamp01() const {
    float32x4_t zero = vdupq_n_f32(0.0f);
    float32x4_t one = vdupq_n_f32(1.0f);
    return {
        vmaxq_f32(zero, vminq_f32(one, x)),
        vmaxq_f32(zero, vminq_f32(one, y)),
        vmaxq_f32(zero, vminq_f32(one, z))
    };
}

Vec3_x4 Vec3_x4::select(uint32x4_t mask, const Vec3_x4 &other) const {
    return {
        vbslq_f32(mask, x, other.x),
        vbslq_f32(mask, y, other.y),
        vbslq_f32(mask, z, other.z)
    };
}

Vec3_x4 Vec3_x4::select(uint32x4_t mask, const float32x4_t otherXYZ) const {
    return {
        vbslq_f32(mask, x, otherXYZ),
        vbslq_f32(mask, y, otherXYZ),
        vbslq_f32(mask, z, otherXYZ)
    };
}

} // namespace rte

#endif // __ARM_NEON__
