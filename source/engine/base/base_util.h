/* date = April 8th 2022 8:24 pm */

#ifndef UTIL_H
#define UTIL_H

#define concat1(a, b) a##b
#define concat(a, b) concat1(a, b)

#define cast(type) (type)
#define array_count(array) (sizeof(array) / sizeof(*array))

#define disable_buffering(stream) setvbuf(stream, 0, _IONBF, 0)

// NOTE(sir->w): This is the most beautiful macro I've ever written,
// and I refuse to believe otherwise.
#define println(fmt, ...) printf(fmt "\n", __VA_ARGS__)
#define fprintln(stream, fmt, ...) printf(stream, fmt "\n", __VA_ARGS__)

#define kilobytes(x) (u64)((u64)x * 1024)
#define megabytes(x) (u64)(kilobytes(x) * 1024)
#define gigabytes(x) (u64)(megabytes(x) * 1024)

#define defer_loop(start, end) for(int _i_ = (start, 0); _i_ == 0; _i_ += 1, end)
#define offset_of(type, member) ((u32)&(((type *)NULL)->member))

//- NOTE(winston): For dynamic exporting of functions.
#if STYX_COMPILER_MSVC
# if STYX_LANG_CPP
#  define styx_export extern "C" _declspec(dllexport)
# else
#  define styx_export _declspec(dllexport)
# endif
#else
# define styx_export
#endif

#if STYX_COMPILER_CLANG || STYX_COMPILER_GCC
# define crash_program() __builtin_trap()
#elif STYX_COMPILER_MSVC
# define crash_program() __debugbreak();
#else
# error No crashing method implemented.
#endif

#if STYX_COMPILER_MSVC
# define local_perthread __declspec(thread)
#else
# define local_perthread
#endif

#if STYX_COMPILER_MSVC
# define pack_struct(name) __pragma(pack(push, 1)) struct name __pragma(pack(pop))
#endif

// Basic functions forward declarations needed for the rest of the base.
extern void styx_message_box(char *title, char *message);

#endif //UTIL_H
