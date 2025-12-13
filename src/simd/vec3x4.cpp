#ifdef __ARM_NEON__

#include "vec3x4.hpp"

namespace rte {

float32x4_t dot_x4(const Vec3x4& a, const Vec3x4& b) {
    float32x4_t xx = vmulq_f32(a.x, b.x);
    float32x4_t yy = vmulq_f32(a.y, b.y);
    float32x4_t zz = vmulq_f32(a.z, b.z);

    return vaddq_f32(vaddq_f32(xx, yy), zz);
}

Vec3x4 Vec3x4::normalized() const {
    float32x4_t length = vsqrtq_f32(dot_x4(*this, *this));
    return {
        vdivq_f32(x, length),
        vdivq_f32(y, length),
        vdivq_f32(z, length)
    };
}

}

#endif // __ARM_NEON__
