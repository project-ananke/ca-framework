package main

import "core:fmt"
import "core:log"
import "core:mem"
import "core:runtime"
import "core:strings"

import "ext:odin-lua/lua"
import "ext:odin-lua/luaL"

import SDL "vendor:sdl2"

import "engine:sys"
import "engine:core"
import "engine:core/styxlua"

main :: proc()
{
    context = core.init_styx_context()
    defer core.free_styx_context()

    L := luaL.newstate()
    defer lua.close(L)
  
    luaL.openlibs(L)
    styxlua.init(L)

    SDL.Init({ .VIDEO })
  	defer SDL.Quit()

	window := SDL.CreateWindow("Project Ananke",
                               SDL.WINDOWPOS_CENTERED, SDL.WINDOWPOS_CENTERED,
                               1280, 720, nil)
	defer SDL.DestroyWindow(window)

    rule30 := styxlua.extract_rule(L, "../source/scripts/rule30.lua")
    defer styxlua.free_rule(&rule30)

	for running := true; running; {
        free_all(context.temp_allocator)

		for e: SDL.Event; SDL.PollEvent(&e); {
			#partial switch e.type {
			case .QUIT:
				running = false
			case .KEYDOWN:
				if e.key.keysym.sym == .ESCAPE {
					running = false
				}
			}
		}
	}
}
