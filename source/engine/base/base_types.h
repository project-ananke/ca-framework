/* date = April 8th 2022 10:42 am */

#ifndef BASE_TYPES_H
#define BASE_TYPES_H

// NOTE(sir->w): For what is a C program without stdio.h? Even if you can't see it?
#include <stdint.h>

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

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

#include "../generated/vector.h"

#endif //BASE_TYPES_H
