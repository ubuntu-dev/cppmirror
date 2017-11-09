#if !defined(_SGLM_MATH_H)

//
// Environment.
//
#define SGLM_COMPILER_MSVC 0
#define SGLM_COMPILER_CLANG 0
#define SGLM_COMPILER_GCC 0

#define SGLM_ENVIRONMENT64 0
#define SGLM_ENVIRONMENT32 0

#define SGLM_OS_WIN32 0
#define SGLM_OS_LINUX 0

// Compiler
#if defined(__clang__)
    #undef SGLM_COMPILER_CLANG
    #define SGLM_COMPILER_CLANG 1
#elif defined(_MSC_VER)
    #undef SGLM_COMPILER_MSVC
    #define SGLM_COMPILER_MSVC 1
#elif (defined(__GNUC__) || defined(__GNUG__)) // This has to be after __clang__, because Clang also defines this.
    #undef SGLM_COMPILER_GCC
    #define SGLM_COMPILER_GCC 1
#else
    #error "Could not detect compiler."
#endif

// OS.
#if defined(__linux__)
    #undef SGLM_OS_LINUX
    #define SGLM_OS_LINUX 1
#elif defined(_WIN32)
    #undef SGLM_OS_WIN32
    #define SGLM_OS_WIN32 1
#else
    #error "Could not detect OS"
#endif

// Environment.
#if SGLM_OS_LINUX
    #if (__x86_64__ || __ppc64__)
        #undef SGLM_ENVIRONMENT64
        #define SGLM_ENVIRONMENT64 1
    #else
        #undef SGLM_ENVIRONMENT32
        #define SGLM_ENVIRONMENT32 1
    #endif
#elif SGLM_OS_WIN32
    #if defined(_WIN64)
        #undef SGLM_ENVIRONMENT64
        #define SGLM_ENVIRONMENT64 1
    #else
        #undef SGLM_ENVIRONMENT32
        #define SGLM_ENVIRONMENT32 1
    #endif
#endif

#if defined(SGLM_IMPLEMENTATION)

#if SGLM_OS_WIN32
    // TODO(Jonny): Is this specific to MSVC or to Windows?
    #include <intrin.h>
#else
    #include <x86intrin.h>
#endif

//
// Types.
//
typedef int sglm_Bool;

#define SGLM_PI 3.1415f

float sglm_normalise(float v, float max, float min);
float sglm_distance_sq(float x1, float y1, float x2, float y2);
float sglm_square(float x);
float sglm_degrees_to_radians(float n);

//
// Sine and cosine.
//
float sglm_sine(float x);
float sglm_cosine(float x);
void sglm_sine_cosine(float x, float *s, float *c);
__m128 sglm_vec_absolute(__m128 m);
__m128 sglm_vec_sine(__m128 vec_num);

//
// Vec.
//
union sglm_V2 {
    float e[2];
    struct { float x, y; };
};

sglm_V2 sglm_v2(float x, float y);
sglm_Bool sglm_v2_compare(sglm_V2 a, sglm_V2 b);

sglm_V2 sglm_v2_add(sglm_V2 a, sglm_V2 b);
sglm_V2 sglm_v2_sub(sglm_V2 a, sglm_V2 b);
sglm_V2 sglm_v2_mul(sglm_V2 vec, float s);
sglm_V2 sglm_v2_dev(sglm_V2 vec, float s);
float sglm_v2_dist_squared(sglm_V2 a, sglm_V2 b);
sglm_V2 sglm_v2_hadamard(sglm_V2 a, sglm_V2 b);
float sglm_v2_inner(sglm_V2 a, sglm_V2 b);
float sglm_v2_length_sq(sglm_V2 x);
sglm_V2 sglm_v2_invert(sglm_V2 v);
float sglm_v2_distance_sq(sglm_V2 a, sglm_V2 b);
sglm_V2 sglm_v2_midpoint(sglm_V2 a, sglm_V2 b);

//
// Mat4x4.
//
struct sglm_Mat4x4 {
    __m128 e[4];
};

sglm_Mat4x4 sglm_mat4x4_set_trans_scale(float x, float y, float scalex, float scaley);
sglm_Mat4x4 sglm_mat4x4_set_trans_scale_rot(float x, float y, float scalex, float scaley, float angle);
sglm_Mat4x4 sglm_mat4x4_mul(sglm_Mat4x4 *a, sglm_Mat4x4 *b);

sglm_Mat4x4 sglm_mat4x4_get_zero(void);
sglm_Mat4x4 sglm_mat4x4_get_iden(void);
sglm_Mat4x4 sglm_mat4x4_set_trans(float x, float y);
sglm_Mat4x4 sglm_mat4x4_get_scale(float x, float y);
sglm_Mat4x4 sglm_mat4x4_get_rot_x(int angle);
sglm_Mat4x4 sglm_mat4x4_get_rot_y(int angle);
sglm_Mat4x4 sglm_mat4x4_get_rot_z(int angle);
float *sglm_mat4x4_as_float_arr(float *arr, sglm_Mat4x4 *mat);

#if !defined(SGLM_ASSERT)
    #if SGLM_COMPILER_MSVC
        #define SGLM_ASSERT(x, ...) do { if(!(x)) { __debugbreak(); } } while(0)
    #else
        #define SGLM_ASSERT(x, ...) do { if(!(x)) { *(int volatile *)0 = 0; } } while(0) // TODO(Jonny): Find out what debugbreak is actually on Linux.
    #endif
#endif

__m128 sglm_vec_absolute(__m128 vec) {
    __m128 vec_sign = _mm_castsi128_ps(_mm_set1_epi32(0x80000000));
    __m128 res = _mm_andnot_ps(vec_sign, vec);

    return(res);
}

__m128 sglm_vec_sine(__m128 vec_num) {
    __m128 vec_abs, vec_res, vec_2pi, vec;

    vec_2pi = _mm_set1_ps(SGLM_PI * 2);
    vec =_mm_cmpnlt_ps(vec_num, _mm_set1_ps(SGLM_PI));
    vec = _mm_and_ps(vec, vec_2pi);
    vec_num = _mm_sub_ps(vec_num, vec);
    vec =_mm_cmpngt_ps(vec_num, _mm_set1_ps(-SGLM_PI));
    vec = _mm_and_ps(vec, vec_2pi);
    vec_num = _mm_add_ps(vec_num, vec);

    vec_abs = sglm_vec_absolute(vec_num);
    vec = _mm_mul_ps(vec_abs, _mm_set1_ps(-4.0f / (SGLM_PI * SGLM_PI)));
    vec = _mm_add_ps(vec, _mm_set1_ps(4.0f / SGLM_PI));

    vec_res = _mm_mul_ps(vec, vec_num);
    vec_abs = sglm_vec_absolute(vec_res);
    vec = _mm_mul_ps(vec_abs, vec_res);
    vec = _mm_sub_ps(vec, vec_res);
    vec = _mm_mul_ps(vec, _mm_set1_ps(0.225f));
    vec_res = _mm_add_ps(vec, vec_res);

    return(vec_res);
}

float sglm_sine(float x) {
    __m128 vec_num = _mm_set1_ps(x);
    __m128 vec_sin = sglm_vec_sine(vec_num);
    float res = _mm_cvtss_f32(vec_sin);

    return(res);
}

float sglm_cosine(float x) {
    __m128 vec_num = _mm_set1_ps(x + SGLM_PI / 2.0f);
    __m128 vec_cos = sglm_vec_sine(vec_num);
    float res = _mm_cvtss_f32(vec_cos);

    return(res);
}

void sglm_sine_cosine(float x, float *s, float *c) {
    __m128 vec_both = _mm_set_ps(0.0f, 0.0f, x + SGLM_PI / 2.0f, x);
    __m128 vec_sincos = sglm_vec_sine(vec_both);
    __m128 vec_cos = _mm_shuffle_ps(vec_sincos, vec_sincos, _MM_SHUFFLE(0, 0, 0, 1));

    *s = _mm_cvtss_f32(vec_sincos);
    *c = _mm_cvtss_f32(vec_cos);
}

float sglm_normalise(float v, float max, float min) {
    float res = ((v - min) / (max - min));

    return(res);
}

float sglm_distance_sq(float x1, float y1, float x2, float y2) {
    float res = sglm_square(x2 - x1) + sglm_square(y2 - y1);

    return(res);
}

float sglm_square(float n) {
    float res = n * n;

    return(res);
}

float sglm_degrees_to_radians(float n) {
    float res = n * 0.0174533f;

    return(res);
}


//
// sglm_V2
//
sglm_V2 sglm_v2(float x, float y) {
    sglm_V2 res;
    res.x = x;
    res.y = y;

    return(res);
}

sglm_Bool sglm_v2_compare(sglm_V2 a, sglm_V2 b) {
    sglm_Bool res;
    if((a.x == b.x) && (a.y == b.y)) {
        res = 1; // true
    }
    else {
        res = 0; // false
    }

    return(res);
}

sglm_V2 sglm_v2_add(sglm_V2 a, sglm_V2 b) {
    sglm_V2 res;
    res.x = a.x + b.x;
    res.y = a.y + b.y;

    return(res);
}

sglm_V2 sglm_v2_sub(sglm_V2 a, sglm_V2 b) {
    sglm_V2 res;
    res.x = a.x - b.x;
    res.y = a.y - b.y;

    return(res);
}

sglm_V2 sglm_v2_mul(sglm_V2 vec, float s) {
    vec.x *= s;
    vec.y *= s;

    return(vec);
}

sglm_V2 sglm_v2_dev(sglm_V2 vec, float s) {
    vec.x /= s;
    vec.y /= s;

    return(vec);
}

float sglm_v2_dist_squared(sglm_V2 a, sglm_V2 b) {
    float res = sglm_square(a.x - b.x) + sglm_square(a.y - b.y);

    return(res);
}

sglm_V2 sglm_v2_hadamard(sglm_V2 a, sglm_V2 b) {
    sglm_V2 res;
    res.x = a.x * b.x;
    res.y = a.y * b.y;

    return(res);
}

float sglm_v2_inner(sglm_V2 a, sglm_V2 b) {
    float res = a.x * b.x + a.y * b.y;

    return(res);
}

float sglm_v2_length_sq(sglm_V2 x) {
    float res = sglm_v2_inner(x, x);

    return(res);
}

sglm_V2 sglm_v2_invert(sglm_V2 v) {
    v.x = -v.x;
    v.y = -v.y;

    return(v);
}

float sglm_v2_distance_sq(sglm_V2 a, sglm_V2 b) {
    float res = sglm_distance_sq(a.x, a.y, b.x, b.y);

    return(res);
}

sglm_V2 sglm_v2_midpoint(sglm_V2 a, sglm_V2 b) {
    sglm_V2 res;
    res.x = (a.x + b.x) * 0.5f;
    res.y = (a.y + b.y) * 0.5f;

    return(res);
}

//
// sglm_Mat4x4
//

sglm_Mat4x4 sglm_mat4x4_set_trans_scale(float x, float y, float scalex, float scaley) {
    sglm_Mat4x4 res;
    res.e[0] = _mm_setr_ps(scalex, 0.0f,   0.0f, 0.0f);
    res.e[1] = _mm_setr_ps(0.0f,   scaley, 0.0f, 0.0f);
    res.e[2] = _mm_setr_ps(0.0f,   0.0f,   1.0f, 0.0f);
    res.e[3] = _mm_setr_ps(x,      y,      0.0f, 1.0f);

    return(res);
}

// TODO(Jonny): Pass the player pin in here, for the rotation.
sglm_Mat4x4 sglm_mat4x4_set_trans_scale_rot(float x, float y, float scalex, float scaley, float angle) {
    sglm_Mat4x4 iden, rot, scale, trans, res;

    while(angle < 0.0f) {
        angle += 360.0f;
    }
    while(angle > 360.0f) {
        angle -= 360.0f;
    }

    SGLM_ASSERT((angle >= 0.0f) && (angle <= 360.0f));

    iden  = sglm_mat4x4_get_iden();
    rot   = sglm_mat4x4_get_rot_z(angle);
    scale = sglm_mat4x4_get_scale(scalex, scaley);
    trans = sglm_mat4x4_set_trans(x, y);

    res = sglm_mat4x4_mul(&iden, &scale);
    res = sglm_mat4x4_mul(&res,  &rot);
    res = sglm_mat4x4_mul(&res,  &trans);

    return(res);
}

sglm_Mat4x4 sglm_mat4x4_get_zero(void) {
    sglm_Mat4x4 res;

    res.e[0] = _mm_setr_ps(0.0f, 0.0f, 0.0f, 0.0f);
    res.e[1] = _mm_setr_ps(0.0f, 0.0f, 0.0f, 0.0f);
    res.e[2] = _mm_setr_ps(0.0f, 0.0f, 0.0f, 0.0f);
    res.e[3] = _mm_setr_ps(0.0f, 0.0f, 0.0f, 0.0f);

    return(res);
}

sglm_Mat4x4 sglm_mat4x4_get_iden(void) {
    sglm_Mat4x4 res;

    res.e[0] = _mm_setr_ps(1.0f, 0.0f, 0.0f, 0.0f);
    res.e[1] = _mm_setr_ps(0.0f, 1.0f, 0.0f, 0.0f);
    res.e[2] = _mm_setr_ps(0.0f, 0.0f, 1.0f, 0.0f);
    res.e[3] = _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f);

    return(res);
}

sglm_Mat4x4 sglm_mat4x4_set_trans(float x, float y) {
    sglm_Mat4x4 res;

    res.e[0] = _mm_setr_ps(1.0f, 0.0f, 0.0f, 0.0f);
    res.e[1] = _mm_setr_ps(0.0f, 1.0f, 0.0f, 0.0f);
    res.e[2] = _mm_setr_ps(0.0f, 0.0f, 1.0f, 0.0f);
    res.e[3] = _mm_setr_ps(x,    y,    0.0f, 1.0f);

    return(res);
}

sglm_Mat4x4 sglm_mat4x4_get_scale(float x, float y) {
    sglm_Mat4x4 res;
    res.e[0] = _mm_setr_ps(x,    0.0f, 0.0f, 0.0f);
    res.e[1] = _mm_setr_ps(0.0f, y,    0.0f, 0.0f);
    res.e[2] = _mm_setr_ps(0.0f, 0.0f, 1.0f, 0.0f);
    res.e[3] = _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f);

    return(res);
}

// TODO(Jonny): Check x and y.
sglm_Mat4x4 sglm_mat4x4_get_rot_x(int angle) {
    sglm_Mat4x4 res;
    float rot = sglm_degrees_to_radians(angle);

    float s, c;
    sglm_sine_cosine(rot, &s, &c);

    res.e[0] = _mm_setr_ps(c,    s,    0.0f, 0.0f);
    res.e[1] = _mm_setr_ps(s,    c,    0.0f, 0.0f);
    res.e[2] = _mm_setr_ps(0.0f, 0.0f, 1.0f, 0.0f);
    res.e[3] = _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f);

    return(res);
}

sglm_Mat4x4 sglm_mat4x4_get_rot_y(int angle) {
    sglm_Mat4x4 res;
    float rot = sglm_degrees_to_radians(angle);

    float s, c;
    sglm_sine_cosine(rot, &s, &c);

    res.e[0] = _mm_setr_ps(c,    0.0f, -s,   0.0f);
    res.e[1] = _mm_setr_ps(0.0f, 1.0f, 0.0f, 0.0f);
    res.e[2] = _mm_setr_ps(s,    0.0f, c,    0.0f);
    res.e[3] = _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f);

    return(res);
}

sglm_Mat4x4 sglm_mat4x4_get_rot_z(int angle) {
    sglm_Mat4x4 res;
    float rot = sglm_degrees_to_radians(angle);

    float s, c;
    sglm_sine_cosine(rot, &s, &c);

    res.e[0] = _mm_setr_ps(c,    s,    0.0f, 0.0f);
    res.e[1] = _mm_setr_ps(-s,   c,    0.0f, 0.0f);
    res.e[2] = _mm_setr_ps(0.0f, 0.0f, 1.0f, 0.0f);
    res.e[3] = _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f);

    return(res);
}

float *sglm_mat4x4_as_float_arr(float *buf, sglm_Mat4x4 *mat) {
    _mm_store_ps(buf,      mat->e[0]);
    _mm_store_ps(buf + 4,  mat->e[1]);
    _mm_store_ps(buf + 8,  mat->e[2]);
    _mm_store_ps(buf + 12, mat->e[3]);

    return(buf);
}

#define SGLM_MM_SHUFFLE(x, y, z, w) ((x) | ((y) << 2) | ((z) << 4) | ((w) << 6)) // Redefinition of intels __MM_SHUFFLE.
#define SGLM_COPY_ONE_LANE_TO_ALL(l, v) _mm_shuffle_ps((v), (v), SGLM_MM_SHUFFLE(l, l, l, l))
#define SGLM_MADD(i, j, k) _mm_add_ps(_mm_mul_ps((i), (j)), (k))

sglm_Mat4x4 sglm_mat4x4_mul(sglm_Mat4x4 *a, sglm_Mat4x4 *b) {
    sglm_Mat4x4 res;

    int *ptr;

    res.e[0] = _mm_mul_ps(b->e[0], SGLM_COPY_ONE_LANE_TO_ALL(0, a->e[0]));
    res.e[1] = _mm_mul_ps(b->e[0], SGLM_COPY_ONE_LANE_TO_ALL(0, a->e[1]));
    res.e[2] = _mm_mul_ps(b->e[0], SGLM_COPY_ONE_LANE_TO_ALL(0, a->e[2]));
    res.e[3] = _mm_mul_ps(b->e[0], SGLM_COPY_ONE_LANE_TO_ALL(0, a->e[3]));

    //res.e[0] = SGLM_MADD(b->e[1], SGLM_COPY_ONE_LANE_TO_ALL(1, a->e[0]), res.e[0]);
    //res.e[0] = __mm_add_ps(_m,_mul_ps((b->e[1]), (_mm_shuffle_ps((1), (1), ((a->e[0]) | ((a->e[0]) << 2) | ((a->e[0]) << 4) | ((a->e[0]) << 6)))), (res.e[0]));
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

#endif // defined(SGLM_IMPLEMENTATION)

#define _SGLM_MATH_H
#endif // !defined(_SGLM_MATH_H)
