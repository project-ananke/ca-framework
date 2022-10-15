#ifndef STYX_ENGINE
#define STYX_ENGINE

#include <SDL.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "base/styx_base.h"
#include "os/styx_window.h"
#include "os/styx_renderer.h"

#include "core/styxlua/styxlua.h"

void init_styx_engine();
void quit_styx_engine();

#endif
