/* date = April 8th 2022 8:24 pm */

#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

#define concat1(a, b) a##b
#define concat(a, b) concat1(a, b)

#define cast(type) (type)
#define array_count(array) (sizeof(array) / sizeof(*array))

#define disable_buffering(stream) setvbuf(stream, 0, _IONBF, 0)

// NOTE(sir->w): This is the most beautiful macro I've ever written,
// and I refuse to believe otherwise.
#define println(fmt, ...) printf(fmt "\n", __VA_ARGS__)
#define fprintln(stream, fmt, ...) fprintf(stream, fmt "\n", __VA_ARGS__)

#define kilobytes(x) (u64)((u64)x * 1024)
#define megabytes(x) (u64)(kilobytes(x) * 1024)
#define gigabytes(x) (u64)(megabytes(x) * 1024)

#define defer_loop(start, end) for(int _i_ = (start, 0); _i_ == 0; _i_ += 1, end)
#define offset_of(type, member) ((u32)&(((type *)NULL)->member))

// Basic functions forward declarations needed for the rest of the base.
styx_function void styx_message_box(char *title, char *message);
styx_function void *styx_reserve_mem(u64 size);
styx_function void styx_free_mem(void *mem, u64 size);

#endif //UTIL_H
