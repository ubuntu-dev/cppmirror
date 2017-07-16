#if !defined(_SGL_MATH_H)

//
// Environment.
//
#if !defined(SGL_SETUP_ENVIRONMENT)
    #define SGL_SETUP_ENVIRONMENT

    #define SGL_COMPILER_MSVC 0
    #define SGL_COMPILER_CLANG 0
    #define SGL_COMPILER_GCC 0

    #define SGL_ENVIRONMENT64 0
    #define SGL_ENVIRONMENT32 0

    #define SGL_OS_WIN32 0
    #define SGL_OS_LINUX 0

    // Compiler
    #if defined(__clang__)
        #undef SGL_COMPILER_CLANG
        #define SGL_COMPILER_CLANG 1
    #elif defined(_MSC_VER)
        #undef SGL_COMPILER_MSVC
        #define SGL_COMPILER_MSVC 1
    #elif (defined(__GNUC__) || defined(__GNUG__)) // This has to be after __clang__, because Clang also defines this.
        #undef SGL_COMPILER_GCC
        #define SGL_COMPILER_GCC 1
    #else
        #error "Could not detect compiler."
    #endif

    // OS.
    #if defined(__linux__)
        #undef SGL_OS_LINUX
        #define SGL_OS_LINUX 1
    #elif defined(_WIN32)
        #undef SGL_OS_WIN32
        #define SGL_OS_WIN32 1
    #else
        #error "Could not detect OS"
    #endif

    // Environment.
    #if SGL_OS_LINUX
        #if (__x86_64__ || __ppc64__)
            #undef SGL_ENVIRONMENT64
            #define SGL_ENVIRONMENT64 1
        #else
            #undef SGL_ENVIRONMENT32
            #define SGL_ENVIRONMENT32 1
        #endif
    #elif SGL_OS_WIN32
        #if defined(_WIN64)
            #undef SGL_ENVIRONMENT64
            #define SGL_ENVIRONMENT64 1
        #else
            #undef SGL_ENVIRONMENT32
            #define SGL_ENVIRONMENT32 1
        #endif
    #endif
#endif

#if SGL_OS_WIN32
    // TODO(Jonny): Is this specific to MSVC or to Windows?
    #include <intrin.h>
#else
    #include <x86intrin.h>
#endif

//
// Types.
//
#include <stdint.h>

#if !defined(SGL_TYPES_SETUP)
    #define SGL_TYPES_SETUP

    typedef uint64_t Uint64;
    typedef int64_t Int64;

    typedef uint32_t Uint32;
    typedef int32_t Int32;

    typedef uint16_t Uint16;
    typedef int16_t Int16;

    typedef uint8_t Uint8;
    typedef int8_t Int8;

    // Int and Uint should always be 4 bytes.
    typedef Uint32 Uint;
    typedef Int32 Int;

    typedef Uint8 Byte;

    typedef float Float32;
    typedef double Float64;
    typedef Float32 Float;

    typedef char Char;
    typedef void Void;
    typedef Int Bool;

    typedef uintptr_t Uintptr;
    typedef intptr_t Intptr;
#endif

static Float const pi = 3.1415f;

Float normalise(Float v, Float max, Float min);
Float distance_sq(Float x1, Float y1, Float x2, Float y2);
Float square(Float x);
Float degrees_to_radians(Float n);

//
// Sine and cosine.
//
Float sine(Float x);
Float cosine(Float x);
void sine_cosine(Float x, Float *s, Float *c);
__m128 vec_absolute(__m128 m);
__m128 vec_sine(__m128 vec_num);

//
// Mat4x4.
//
typedef struct Mat4x4 {
    __m128 e[4];
} Mat4x4;

Mat4x4 mat4x4_set_trans_scale(Float x, Float y, Float scalex, Float scaley);
Mat4x4 mat4x4_set_trans_scale_rot(Float x, Float y, Float scalex, Float scaley, Float angle);
Mat4x4 mat4x4_mul(Mat4x4 *a, Mat4x4 *b);

Mat4x4 mat4x4_get_zero(void);
Mat4x4 mat4x4_get_iden(void);
Mat4x4 mat4x4_set_trans(Float x, Float y);
Mat4x4 mat4x4_get_scale(Float x, Float y);
Mat4x4 mat4x4_get_rot_x(int angle);
Mat4x4 mat4x4_get_rot_y(int angle);
Mat4x4 mat4x4_get_rot_z(int angle);
Float *mat4x4_as_float_arr(Float *arr, Mat4x4 *mat);


#if !defined(SGL_ASSERT)
    #if SGL_COMPILER_MSVC
        #define SGL_ASSERT(x, ...) do { if(!(x)) { __debugbreak(); } } while(0)
    #else
        #define SGL_ASSERT(x, ...) do { if(!(x)) { *(Uintptr volatile *)0 = 0; } } while(0) // TODO(Jonny): Find out what debugbreak is actually on Linux.
    #endif
#endif

__m128 vec_absolute(__m128 vec) {
    __m128 vec_sign = _mm_castsi128_ps(_mm_set1_epi32(0x80000000));
    __m128 res = _mm_andnot_ps(vec_sign, vec);

    return(res);
}

__m128 vec_sine(__m128 vec_num) {
    __m128 vec_2pi = _mm_set1_ps(pi * 2);
    __m128 vec =_mm_cmpnlt_ps(vec_num, _mm_set1_ps(pi));
    vec = _mm_and_ps(vec, vec_2pi);
    vec_num = _mm_sub_ps(vec_num, vec);
    vec =_mm_cmpngt_ps(vec_num, _mm_set1_ps(-pi));
    vec = _mm_and_ps(vec, vec_2pi);
    vec_num = _mm_add_ps(vec_num, vec);

    __m128 vec_abs = vec_absolute(vec_num);
    vec = _mm_mul_ps(vec_abs, _mm_set1_ps(-4.0f / (pi * pi)));
    vec = _mm_add_ps(vec, _mm_set1_ps(4.0f / pi));

    __m128 vec_res = _mm_mul_ps(vec, vec_num);
    vec_abs = vec_absolute(vec_res);
    vec = _mm_mul_ps(vec_abs, vec_res);
    vec = _mm_sub_ps(vec, vec_res);
    vec = _mm_mul_ps(vec, _mm_set1_ps(0.225f));
    vec_res = _mm_add_ps(vec, vec_res);

    return(vec_res);
}

Float sine(Float x) {
    __m128 vec_num = _mm_set1_ps(x);
    __m128 vec_sin = vec_sine(vec_num);
    Float res = _mm_cvtss_f32(vec_sin);

    return(res);
}

Float cosine(Float x) {
    __m128 vec_num = _mm_set1_ps(x + pi / 2.0f);
    __m128 vec_cos = vec_sine(vec_num);
    Float res = _mm_cvtss_f32(vec_cos);

    return(res);
}

void sine_cosine(Float x, Float *s, Float *c) {
    __m128 vec_both = _mm_set_ps(0.0f, 0.0f, x + pi / 2.0f, x);
    __m128 vec_sincos = vec_sine(vec_both);
    __m128 vec_cos = _mm_shuffle_ps(vec_sincos, vec_sincos, _MM_SHUFFLE(0, 0, 0, 1));

    *s = _mm_cvtss_f32(vec_sincos);
    *c = _mm_cvtss_f32(vec_cos);
}

Float normalise(Float v, Float max, Float min) {
    Float res = ((v - min) / (max - min));

    return(res);
}

Float distance_sq(Float x1, Float y1, Float x2, Float y2) {
    Float res = square(x2 - x1) + square(y2 - y1);

    return(res);
}

Float square(Float n) {
    Float res = n * n;

    return(res);
}

Float degrees_to_radians(Float n) {
    Float res = n * 0.0174533f;

    return(res);
}

//
// Mat4x4
//

Mat4x4 mat4x4_set_trans_scale(Float x, Float y, Float scalex, Float scaley) {
    Mat4x4 res;
    res.e[0] = _mm_setr_ps(scalex, 0.0f,   0.0f, 0.0f);
    res.e[1] = _mm_setr_ps(0.0f,   scaley, 0.0f, 0.0f);
    res.e[2] = _mm_setr_ps(0.0f,   0.0f,   1.0f, 0.0f);
    res.e[3] = _mm_setr_ps(x,      y,      0.0f, 1.0f);

    return(res);
}

// TODO(Jonny): Pass the player pin in here, for the rotation.
Mat4x4 mat4x4_set_trans_scale_rot(Float x, Float y, Float scalex, Float scaley, Float angle) {
    while(angle < 0.0f) {
        angle += 360.0f;
    }
    while(angle > 360.0f) {
        angle -= 360.0f;
    }

    SGL_ASSERT((angle >= 0.0f) && (angle <= 360.0f));

    Mat4x4 iden  = mat4x4_get_iden();
    Mat4x4 rot   = mat4x4_get_rot_z(angle);
    Mat4x4 scale = mat4x4_get_scale(scalex, scaley);
    Mat4x4 trans = mat4x4_set_trans(x, y);

    Mat4x4 res;
    res = mat4x4_mul(&iden, &scale);
    res = mat4x4_mul(&res,  &rot);
    res = mat4x4_mul(&res,  &trans);

    return(res);
}

Mat4x4 mat4x4_get_zero(void) {
    Mat4x4 res;

    res.e[0] = _mm_setr_ps(0.0f, 0.0f, 0.0f, 0.0f);
    res.e[1] = _mm_setr_ps(0.0f, 0.0f, 0.0f, 0.0f);
    res.e[2] = _mm_setr_ps(0.0f, 0.0f, 0.0f, 0.0f);
    res.e[3] = _mm_setr_ps(0.0f, 0.0f, 0.0f, 0.0f);

    return(res);
}

Mat4x4 mat4x4_get_iden(void) {
    Mat4x4 res;

    res.e[0] = _mm_setr_ps(1.0f, 0.0f, 0.0f, 0.0f);
    res.e[1] = _mm_setr_ps(0.0f, 1.0f, 0.0f, 0.0f);
    res.e[2] = _mm_setr_ps(0.0f, 0.0f, 1.0f, 0.0f);
    res.e[3] = _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f);

    return(res);
}

Mat4x4 mat4x4_set_trans(Float x, Float y) {
    Mat4x4 res;

    res.e[0] = _mm_setr_ps(1.0f, 0.0f, 0.0f, 0.0f);
    res.e[1] = _mm_setr_ps(0.0f, 1.0f, 0.0f, 0.0f);
    res.e[2] = _mm_setr_ps(0.0f, 0.0f, 1.0f, 0.0f);
    res.e[3] = _mm_setr_ps(x,    y,    0.0f, 1.0f);

    return(res);
}

Mat4x4 mat4x4_get_scale(Float x, Float y) {
    Mat4x4 res;
    res.e[0] = _mm_setr_ps(x,    0.0f, 0.0f, 0.0f);
    res.e[1] = _mm_setr_ps(0.0f, y,    0.0f, 0.0f);
    res.e[2] = _mm_setr_ps(0.0f, 0.0f, 1.0f, 0.0f);
    res.e[3] = _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f);

    return(res);
}

// TODO(Jonny): Check x and y.
Mat4x4 mat4x4_get_rot_x(int angle) {
    Float rot = degrees_to_radians(angle);

    Float s, c;
    sine_cosine(rot, &s, &c);

    Mat4x4 res;
    res.e[0] = _mm_setr_ps(c,    s,    0.0f, 0.0f);
    res.e[1] = _mm_setr_ps(s,    c,    0.0f, 0.0f);
    res.e[2] = _mm_setr_ps(0.0f, 0.0f, 1.0f, 0.0f);
    res.e[3] = _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f);

    return(res);
}

Mat4x4 mat4x4_get_rot_y(int angle) {
    Float rot = degrees_to_radians(angle);

    Float s, c;
    sine_cosine(rot, &s, &c);

    Mat4x4 res;
    res.e[0] = _mm_setr_ps(c,    0.0f, -s,   0.0f);
    res.e[1] = _mm_setr_ps(0.0f, 1.0f, 0.0f, 0.0f);
    res.e[2] = _mm_setr_ps(s,    0.0f, c,    0.0f);
    res.e[3] = _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f);

    return(res);
}

Mat4x4 mat4x4_get_rot_z(int angle) {
    Float rot = degrees_to_radians(angle);

    Float s, c;
    sine_cosine(rot, &s, &c);

    Mat4x4 res;
    res.e[0] = _mm_setr_ps(c,    s,    0.0f, 0.0f);
    res.e[1] = _mm_setr_ps(-s,   c,    0.0f, 0.0f);
    res.e[2] = _mm_setr_ps(0.0f, 0.0f, 1.0f, 0.0f);
    res.e[3] = _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f);

    return(res);
}

Float *mat4x4_as_float_arr(Float *buf, Mat4x4 *mat) {
    _mm_store_ps(buf,      mat->e[0]);
    _mm_store_ps(buf + 4,  mat->e[1]);
    _mm_store_ps(buf + 8,  mat->e[2]);
    _mm_store_ps(buf + 12, mat->e[3]);

    return(buf);
}

#define SGLM_MM_SHUFFLE(x, y, z, w) ((x) | ((y) << 2) | ((z) << 4) | ((w) << 6)) // Redefinition of intels __MM_SHUFFLE.
#define SGLM_COPY_ONE_LANE_TO_ALL(l, v) _mm_shuffle_ps((v), (v), SGLM_MM_SHUFFLE(l, l, l, l))
#define SGLM_MADD(a, b, c) _mm_add_ps(_mm_mul_ps((a), (b)), (c))

Mat4x4 mat4x4_mul(Mat4x4 *a, Mat4x4 *b) {
    Mat4x4 res;

    res.e[0] = _mm_mul_ps(b->e[0], SGLM_COPY_ONE_LANE_TO_ALL(0, a->e[0]));
    res.e[1] = _mm_mul_ps(b->e[0], SGLM_COPY_ONE_LANE_TO_ALL(0, a->e[1]));
    res.e[2] = _mm_mul_ps(b->e[0], SGLM_COPY_ONE_LANE_TO_ALL(0, a->e[2]));
    res.e[3] = _mm_mul_ps(b->e[0], SGLM_COPY_ONE_LANE_TO_ALL(0, a->e[3]));

    res.e[0] = SGLM_MADD(b->e[1], SGLM_COPY_ONE_LANE_TO_ALL(1, a->e[0]), res.e[0]);
    res.e[1] = SGLM_MADD(b->e[1], SGLM_COPY_ONE_LANE_TO_ALL(1, a->e[1]), res.e[1]);
    res.e[2] = SGLM_MADD(b->e[1], SGLM_COPY_ONE_LANE_TO_ALL(1, a->e[2]), res.e[2]);
    res.e[3] = SGLM_MADD(b->e[1], SGLM_COPY_ONE_LANE_TO_ALL(1, a->e[3]), res.e[3]);

    res.e[0] = SGLM_MADD(b->e[2], SGLM_COPY_ONE_LANE_TO_ALL(2, a->e[0]), res.e[0]);
    res.e[1] = SGLM_MADD(b->e[2], SGLM_COPY_ONE_LANE_TO_ALL(2, a->e[1]), res.e[1]);
    res.e[2] = SGLM_MADD(b->e[2], SGLM_COPY_ONE_LANE_TO_ALL(2, a->e[2]), res.e[2]);
    res.e[3] = SGLM_MADD(b->e[2], SGLM_COPY_ONE_LANE_TO_ALL(2, a->e[3]), res.e[3]);

    res.e[0] = SGLM_MADD(b->e[3], SGLM_COPY_ONE_LANE_TO_ALL(3, a->e[0]), res.e[0]);
    res.e[1] = SGLM_MADD(b->e[3], SGLM_COPY_ONE_LANE_TO_ALL(3, a->e[1]), res.e[1]);
    res.e[2] = SGLM_MADD(b->e[3], SGLM_COPY_ONE_LANE_TO_ALL(3, a->e[2]), res.e[2]);
    res.e[3] = SGLM_MADD(b->e[3], SGLM_COPY_ONE_LANE_TO_ALL(3, a->e[3]), res.e[3]);

    return(res);
}

int main(int argc, char **argv) {

}

#define _SGL_MATH_H
#endif // !defined(_SGL_MATH_H)
