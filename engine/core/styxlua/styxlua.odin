package styxlua

import "core:log"
import "core:fmt"
import "core:strings"

import "ext:odin-lua/lua"
import "ext:odin-lua/luaL"

import "engine:common"
import "engine:core/styxlua/luasys"

// NOTE(sir->w7): Rendering abstraction in the lua framework would 
// make sense to queue jobs to the platform renderer. Hmm...

@(private)
L: ^lua.State

// Move the rules to a different file. The current situation does not work well with this because
// anything that has to do with lua has to be included in this one file. That means rules extracted
// from lua scripts should be in this file. Or we can write simple wrappers.

base_script: cstring = 
	`xpcall(function()
    	exe_path = styxsys.get_exe_path()
        exe_dir = exe_path:match("^(.+)[/\\].*$")
        -- styxsys.chdir(exe_dir)
        package.path = exe_dir .. "/../source/?/init.lua;" .. package.path
        package.path = exe_dir .. "/../source/?.lua;" .. package.path
        package.path = exe_dir .. "/../source/scripts/?.lua;" .. package.path
        --local styxengine = require('styxengine')
        --styxengine.run()
    end, function(err)
   		print('Error: ' .. tostring(err))
    end)`

libs := [?]luaL.Reg{
	{ "styxsys", luasys.luaopen },
	{ nil, nil }, 
}

init :: proc()
{
    L = luaL.newstate()
    luaL.openlibs(L)

	for i := 0; libs[i].func != nil; i += 1 {
		luaL.requiref(L, libs[i].name, libs[i].func, 1)
	}

    if luaL.dostring(L, base_script) != lua.OK {
        log.errorf("Lua error: %s.\n", lua.tostring(L, -1))
        lua.pop(L, 1)
    }
}

free :: proc()
{
    lua.close(L)
}

// Error handling
module_index_int :: proc(index: string) -> i64
{
    lua.pushstring(L, index, context.temp_allocator)
    lua.gettable(L, -2)
    
    result := luaL.checkinteger(L, -1)

    lua.pop(L, 1)

    return result
}

// Do some cool error checking. 
@private
rule_extract_grid :: proc(grid_width: u32, grid_height: u32, allocator := context.allocator) -> (grid: []u8)
{
    grid = make([]u8, grid_width * grid_height, allocator)

    lua.pushstring(L, "grid", context.temp_allocator)
    lua.gettable(L, -2)

    if !lua.istable(L, -1) {
        return
    }

    for y in 1..=grid_height {
        lua.pushinteger(L, i64(y))
        
        lua.gettable(L, -2)
        defer lua.pop(L, 1)

        for x in 1..=grid_width {
            lua.rawgeti(L, -1, i64(x))
            defer lua.pop(L, 1)

            grid[((y - 1) * grid_width) + (x - 1)] = u8(luaL.checkinteger(L, -1))
        }
    }

    return
}

// Change context allocator, I don't know.
extract_rule :: proc(_rule_path: string, allocator := context.allocator) -> (rule: common.Grid)
{
    rule_path := strings.clone_to_cstring(_rule_path, allocator)
    if luaL.dofile(L, rule_path) != lua.OK {
        log.errorf("Lua error: %s.", lua.tostring(L, -1))
        lua.pop(L, 1)

        return
    }

    // Check if there is a return. 
    rule.grid_width = u32(module_index_int("grid_width"))
    rule.grid_height = u32(module_index_int("grid_height"))

    rule.grid = rule_extract_grid(rule.grid_width, rule.grid_height, allocator)

    lua.settop(L, 0)
    return
}

set_global_grid :: proc(grid: common.Grid, var_name: cstring)
{
    if grid.grid_width == 0 || grid.grid_height == 0 {
        return
    }

    lua.newtable(L)

    for y in 1..=grid.grid_height {
        lua.newtable(L)

        for x in 1..=grid.grid_width {
            lua.pushinteger(L, i64(grid.grid[((y - 1) * grid.grid_width) + (x - 1)]))
            lua.rawseti(L, -2, i64(x))
        }
        lua.rawseti(L, -2, i64(y))
    }

    lua.setglobal(L, var_name)
}
