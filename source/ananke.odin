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

    rule30 := styxlua.extract_rule("../source/scripts/rule90.lua")
    defer common.free_rule(&rule30)

    for window.running {
    	free_all(context.temp_allocator)

    	platform.window_process(&window)
        platform.window_clear(&window, styxm.Vec3c{0xFF, 0xFF, 0xFF})

        styx2d.push_gridrule(&window, rule30, styxm.Vec3c{0x00, 0x00, 0x00})

        platform.window_update(&window)
    }
}
