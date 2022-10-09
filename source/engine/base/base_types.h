/* date = April 8th 2022 10:42 am */

#ifndef BASE_TYPES_H
#define BASE_TYPES_H

// NOTE(sir->w): For what is a C program without stdio.h? Even if you can't see it?
#include <stdint.h>

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef u8  b8;
typedef u16 b16;
typedef u32 b32;
typedef u64 b64;

typedef float  f32;
typedef double f64;

#define TRUE (1)
#define FALSE (0)

#define styx_function static
#define styx_global static
#define styx_persist static

#if defined(STYX_COMPILER_CLANG) || defined(STYX_COMPILER_GCC)
#define styx_inline static inline
#else
#define styx_inline inline
#endif

typedef struct vec3f vec3f;
struct vec3f 
{
    f32 x;
    f32 y;
    f32 z;
};

typedef struct vec3c vec3c;
struct vec3c
{
    u8 r;
    u8 g;
    u8 b;
};

#define vec3c_lit(r1, g1, b1) ((vec3c){.r = r1, .g = g1, .b = b1})
#define vec3f_lit(x1, y1, z1) ((vec3f){.x = x1, .y = y1, .z = z1})

#endif //BASE_TYPES_H
