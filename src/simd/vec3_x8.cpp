#ifdef __ARM_NEON__

#include "vec3_x8.hpp"

namespace rte {

float16x8_t dot_x8(const Vec3_x8& a, const Vec3_x8& b) {
    float16x8_t xx = vmulq_f16(a.x, b.x);
    float16x8_t yy = vmulq_f16(a.y, b.y);
    float16x8_t zz = vmulq_f16(a.z, b.z);

    return vaddq_f16(vaddq_f16(xx, yy), zz);
}

Vec3_x8 Vec3_x8::normalized() const {
    float16x8_t length = vsqrtq_f16(dot_x8(*this, *this));
    return {
        vdivq_f16(x, length),
        vdivq_f16(y, length),
        vdivq_f16(z, length)
    };
}

}

#endif // __ARM_NEON__
