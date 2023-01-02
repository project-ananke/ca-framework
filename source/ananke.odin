package main

import "core:fmt"

import "engine:common"
import "engine:platform"

import "engine:core"
import "engine:core/styxlua"
import "engine:core/styx2d"
import "engine:core/styxm"

main :: proc()
{
    context = core.init_engine()
    defer core.free_engine()

    window := platform.init_window(1280, 720, "Project Ananke")
    defer platform.free_window(&window)

    core_grid: common.Grid
    defer common.free_grid(&core_grid)

	selected_script := "../source/scripts/rule90.lua"
    for window.running {
    	free_all(context.temp_allocator)

    	platform.window_process(&window)
        platform.window_clear(&window, styxm.Vec3c{0xFF, 0xFF, 0xFF})

		rule_grid := styxlua.extract_rule(selected_script, context.temp_allocator)
		styx2d.push_gridrule(&window, rule_grid, styxm.Vec3c{0x00, 0x00, 0x00})

        platform.window_update(&window)
        platform.window_cap_fps(60)
    }
}
