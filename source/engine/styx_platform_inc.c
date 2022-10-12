#define STYX_PLATFORM 1
#define STYX_GAME 0

// Should we consider a potential switch to C++ for operator overloading,
// defer mechanism, and just overall sanity?
#include "base/styx_base_inc.c"

#if STYX_OS_WINDOWS
#define WIN32_LEAN_AND_MEAN
#define COBJMACROS
#include <windows.h>
#include <shlwapi.h>
#include <initguid.h>

#include <d3d11.h>
#include <dxgi1_2.h>
#include <d3dcompiler.h>

#include <mmdeviceapi.h>
#include <audioclient.h>

#define assert_hr(hr) assert(SUCCEEDED(hr))
#define assert_msg_hr(hr, ...) assert_msg(SUCCEEDED(hr), __VA_ARGS__)

#include "core/common/styx_os.c"
#include "core/common/styx_renderer.c"
#include "core/common/styx_module.h"

#include "os/win32/win32_gfx.c"
#include "os/win32/win32_wasapi.c"
#include "os/win32/win32_platform.c"
//
//#define styxplat_create_window styxplat_create_window
//#define styx_destroy_window styx_destroy_window
//#define styx_process_messages styx_process_messages
//
//#define styx_load_module styx_load_styx_module
//#define styx_unload_module styx_unload_styx_module
//
//#define styx_fill_sound_buffer styx_fill_sound_buffer
//

// NOTE(sir->w): Support for multiple rendering backends.

#elif STYX_OS_MAC 

#include <sys/mman.h>

#include <SDL.h>

#include "core/common/styx_os.c"
#include "core/common/styx_renderer.c"
#include "core/common/styx_module.h"

#include "os/macos/macos_platform.c"

#else
# error "Platform not yet supported."
#endif
