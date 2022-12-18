package styxlua

import "core:log"
import "core:fmt"

import "ext:odin-lua/lua"
import "ext:odin-lua/luaL"

import "engine:sys"

Rule :: struct 
{
    grid_width, grid_height: u32,
    grid: []u8,
}

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
	{ "styxsys", sys.luaopen },
	{ nil, nil }, 
}

init :: proc(L: ^lua.State)
{
	for i := 0; libs[i].func != nil; i += 1 {
		luaL.requiref(L, libs[i].name, libs[i].func, 1)
	}

    if luaL.dostring(L, base_script) != lua.OK {
        log.errorf("Lua error: %s.\n", lua.tostring(L, -1))
        lua.pop(L, 1)
    }
}

// Error handling
module_index_int :: proc(L: ^lua.State, index: string) -> i64
{
    lua.pushstring(L, index, context.temp_allocator)
    lua.gettable(L, -2)
    
    result := luaL.checkinteger(L, -1)

    lua.pop(L, 1)

    return result
}


// Do some cool error checking. 
@(private)
rule_extract_grid :: proc(L: ^lua.State, grid_width: u32, grid_height: u32) -> (grid: []u8)
{
    grid = make([]u8, grid_width * grid_height)

    lua.pushstring(L, "grid", context.temp_allocator)
    lua.gettable(L, -2)

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
extract_rule :: proc(L: ^lua.State, rule_path: cstring) -> (rule: Rule)
{
    if luaL.dofile(L, rule_path) != lua.OK {
        log.errorf("Lua error: %s.", lua.tostring(L, -1))
        lua.pop(L, 1)
    }


    rule.grid_width = u32(module_index_int(L, "grid_width"))
    rule.grid_height = u32(module_index_int(L, "grid_height"))

    rule.grid = rule_extract_grid(L, rule.grid_width, rule.grid_height)

    return
}

free_rule :: proc(rule: ^Rule)
{
    delete(rule.grid)
}