package main

import "core:fmt"

import "engine:common"
import "engine:platform"

import "engine:core"
import "engine:core/styxlua"
import "engine:core/styx2d"
import "engine:core/styxm"

// Do not import any external libraries in this file. 
import mu "vendor:microui"

dev_window :: proc(mu_ctx: ^mu.Context) -> string
{
    scripts := platform.list_dir("../source/scripts", context.temp_allocator)
    @static selected_script: string
    @static selected_script_name: string

    info_script := fmt.aprintf("Selected script: %s", selected_script_name)
    defer delete(info_script)

    if mu.window(mu_ctx, "Dev Window", {920, 120, 200, 400}) {
        if (.ACTIVE in mu.header(mu_ctx, "Scripts")) { 
            for file, _ in scripts {
               mu.push_id(mu_ctx, file.base_name)
               if (.SUBMIT in mu.button(mu_ctx, file.base_name)) {
                   selected_script = file.abs_path
                   selected_script_name = file.base_name
               }
               mu.pop_id(mu_ctx)
            }

            mu.label(mu_ctx, info_script)
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

    core_grid: common.Grid
    defer common.free_grid(&core_grid)

    selected_script: string
    for window.running {
    	free_all(context.temp_allocator)

    	platform.window_process(&window)
        platform.window_clear(&window, styxm.Vec3c{0xFF, 0xFF, 0xFF})

        mu.begin(&window.mu_ctx)
        selected_script = dev_window(&window.mu_ctx)
        mu.end(&window.mu_ctx)

        if selected_script != "" {
            styxlua.set_global_grid(core_grid, "styx_grid")
            rule_grid := styxlua.extract_rule(selected_script, context.temp_allocator)
            common.grid_update(&core_grid, &rule_grid)

            styx2d.push_gridrule(&window, rule_grid, styxm.Vec3c{0x00, 0x00, 0x00})
        }

        styx2d.mu_render(&window)

        platform.window_update(&window)
        platform.window_cap_fps(60)
    }
}
