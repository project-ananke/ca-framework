package core

import "core:runtime"
import "core:log"
import "core:mem"
import "core:fmt"

import "ext:odin-lua/lua"
import "ext:odin-lua/luaL"

import SDL "vendor:sdl2"

import "engine:common"
import "engine:core/styxlua"

init_engine :: proc() -> runtime.Context
{
	context = common.init_styx_context()

    styxlua.init()
    SDL.Init({ .VIDEO })

    return context
}

free_engine :: proc()
{
	styxlua.free()
	SDL.Quit()

	common.free_styx_context()
}