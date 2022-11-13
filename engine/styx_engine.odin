package main

import "core:fmt"
import "core:log"

import "ext:odin-lua/lua"
import "ext:odin-lua/luaL"

import SDL "vendor:sdl2"
import "sys"

base_script: cstring = 
	`xpcall(function()
    	exe_path = styxsys.get_exe_path()
        exe_dir = exe_path:match("^(.+)[/\\].*$")
        styxsys.chdir(exe_dir)
        package.path = exe_dir .. "/../source/?/init.lua;" .. package.path
        package.path = exe_dir .. "/../source/?.lua;" .. package.path
        package.path = exe_dir .. "/../source/scripts/?.lua;" .. package.path
        local styxengine = require('styxengine')
        styxengine.run()
    end, function(err)
   		print('Error: ' .. tostring(err))
    end)`
  
main :: proc()
{
    L := luaL.newstate()
    defer lua.close(L)
  
    luaL.openlibs(L)

    SDL.Init({ .VIDEO })
  	defer SDL.Quit()
	window := SDL.CreateWindow("Project Ananke",
                               SDL.WINDOWPOS_CENTERED, SDL.WINDOWPOS_CENTERED,
                               1280, 720, nil)
	defer SDL.DestroyWindow(window)

	for running := true; running; {
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

    if luaL.dostring(L, base_script) != lua.OK {
        fmt.printf("Lua error: %s.\n", lua.tostring(L, -1))
        lua.pop(L, 1)
    }
}
