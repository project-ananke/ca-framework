package main

import "core:fmt"

import "engine:common"
import "engine:platform"

import "engine:core"
import "engine:core/styxlua"
import "engine:core/styx2d"
import "engine:core/styxm"

// Do not export any external libraries in this file. 
import mu "vendor:microui"

script_window :: proc(mu_ctx: ^mu.Context) -> string
{
    scripts := platform.list_files("../source/scripts", context.temp_allocator)
    @static selected_script: string

    if mu.window(mu_ctx, "Scripts Selection Menu", {40, 40, 200, 400}) {
        for file, _ in scripts {
            mu.push_id(mu_ctx, file.base_name)
            if (.SUBMIT in mu.button(mu_ctx, file.base_name)) {
                selected_script = file.abs_path
            }
            mu.pop_id(mu_ctx)
        }
    }

    return selected_script
}

main :: proc()
{
    context = core.init_engine()
    defer core.free_engine()

    window := platform.init_window(1280, 720, "Project Ananke")
    defer platform.free_window(&window)

    selected_script: string
    for window.running {
    	free_all(context.temp_allocator)

    	platform.window_process(&window)
        platform.window_clear(&window, styxm.Vec3c{0xFF, 0xFF, 0xFF})

        mu.begin(&window.mu_ctx)
        selected_script = script_window(&window.mu_ctx)
        mu.end(&window.mu_ctx)

        if selected_script != "" {
            rule := styxlua.extract_rule(selected_script, context.temp_allocator)
            styx2d.push_gridrule(&window, rule, styxm.Vec3c{0x00, 0x00, 0x00})
        }

        styx2d.mu_render(&window)
        platform.window_update(&window)
    }
}
