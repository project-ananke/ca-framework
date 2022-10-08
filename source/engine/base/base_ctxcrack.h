/* date = April 8th 2022 8:20 pm */

#ifndef CTX_CRACK_H
#define CTX_CRACK_H

#if defined(_MSC_VER)
# define STYX_COMPILER_MSVC 1

# if defined(_WIN32)
#  define STYX_OS_WINDOWS 1
# else
#  error Unknown OS.
# endif

# if defined(_M_AMD64)
#  define STYX_ARCH_X64 1
# elif defined(_M_I86)
#  define STYX_ARCH_X86 1
# elif defined(_M_AMD64)
#  define STYX_ARCH_ARM
# else
#  error Unknown arch.
# endif

#elif defined(__clang__)
# define STYX_COMPILER_CLANG 1

# if defined(_WIN32)
#  define STYX_OS_WINDOWS 1
# elif defined(__gnu_linux__)
#  define STYX_OS_LINUX 1
# elif defined(__APPLE__) && defined(__MACH__)
#  define STYX_OS_MAC 1
# else
#  error Unknown OS.
# endif

# if defined(__amd64__)
#  define STYX_ARCH_X64 1
# elif defined(__i386__)
#  define STYX_ARCH_X86 1
# elif defined(__arm__)
#  define STYX_ARCH_ARM 1
# else
#  error Unknown arch.
# endif

#elif defined(__GNUC__)
# define STYX_COMPILER_GCC 1

# if defined(_WIN32)
#  define STYX_OS_WINDOWS 1
# elif defined(__gnu_linux__)
#  define STYX_OS_LINUX 1
# elif defined(__APPLE__) && defined(__MACH__)
#  define STYX_OS_MAC 1
# else
#  error Unknown OS.
# endif

# if defined(__amd64__)
#  define STYX_ARCH_X64 1
# elif defined(__i386__)
#  define STYX_ARCH_X86 1
# elif defined(__arm__)
#  define STYX_ARCH_ARM
# else
#  error Unknown arch.
# endif

#else
# error Unknown compiler.
#endif

#if defined(__cplusplus)
# define STYX_LANG_CPP 1
#else
# define STYX_LANG_C 1
#endif

#if !defined(STYX_COMPILER_MSVC)
# define STYX_COMPILER_MSVC 0
#endif

#if !defined(STYX_COMPILER_GCC)
# define STYX_COMPILER_GCC 0
#endif

#if !defined(STYX_COMPILER_CLANG)
# define STYX_COMPILER_CLANG 0
#endif

#if !defined(STYX_OS_WINDOWS)
# define STYX_OS_WINDOWS 0
#endif

#if !defined(STYX_OS_LINUX)
# define STYX_OS_LINUX 0
#endif

#if !defined(STYX_OS_MAC)
# define STYX_OS_MAC 0
#endif

#if !defined(STYX_ARCH_X64)
# define STYX_ARCH_X64 0
#endif

#if !defined(STYX_ARCH_X86)
# define STYX_ARCH_X86 0
#endif

#if !defined(STYX_ARCH_ARM)
# define STYX_ARCH_ARM 0
#endif

#if !defined(STYX_LANG_CPP)
# define STYX_LANG_CPP 0
#endif

#if !defined(STYX_LANG_C)
# define STYX_LANG_C 0
#endif

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

#if defined(NDEBUG)
# define STYX_RELEASE 1 
#else
# define STYX_DEBUG 1
#endif

#if !defined(STYX_RELEASE)
# define STYX_RELEASE 0
#endif

#if !defined(STYX_DEBUG)
# define STYX_DEBUG 0
#endif 

#endif //CTX_CRACK_H
