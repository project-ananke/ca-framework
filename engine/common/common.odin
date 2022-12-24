package common

import "core:runtime"
import "core:mem"
import "core:log"
import "core:fmt"

Grid :: struct 
{
    grid_width, grid_height: u32,
    grid: []u8,
}

free_grid :: proc(rule: ^Grid, allocator := context.allocator)
{
	if rule.grid_width == 0 || rule.grid_height == 0 do return
    delete(rule.grid, allocator)

    rule.grid = nil
    rule.grid_width = 0
    rule.grid_height = 0
}

init_grid :: proc(grid_width, grid_height: u32, allocator := context.allocator) -> (grid: Grid)
{
    grid.grid_width = grid_width
    grid.grid_height = grid_height

    grid.grid = make([]u8, grid_width * grid_height, allocator)

    return
}

grid_update :: proc(grid1: ^Grid, grid2: ^Grid)
{
	if grid1.grid_width != grid2.grid_width || 
	   grid1.grid_height != grid2.grid_height {
		free_grid(grid1)
		grid1^ = init_grid(grid2.grid_width, grid2.grid_height)
		return
	}

	for y in 0..<grid1.grid_height {
		for x in 0..<grid1.grid_width {
			grid1.grid[(y * grid1.grid_width) + x] = 
				grid2.grid[(y * grid1.grid_width) + x]
		}
	}
}

@private
_styx_context: struct{
	tracking_allocator: mem.Tracking_Allocator,

	using runtime_ctx: runtime.Context,
}

init_styx_context :: proc() -> runtime.Context
{
	_styx_context.runtime_ctx = runtime.default_context()

    mem.tracking_allocator_init(&_styx_context.tracking_allocator, _styx_context.allocator)

    _styx_context.allocator = mem.tracking_allocator(&_styx_context.tracking_allocator)
    _styx_context.logger = log.create_console_logger()

    return _styx_context;
}

free_styx_context :: proc()
{
	for _, leak in _styx_context.tracking_allocator.allocation_map {
		fmt.printf("%v leaked %v bytes.\n", leak.location, leak.size)
	}
	for bad_free in _styx_context.tracking_allocator.bad_free_array {
		fmt.printf("%v allocation %p was freed badly.\n", bad_free.location, bad_free.memory)
	}

	// Check whether we should actually free this stuff. 
	log.destroy_console_logger(_styx_context.logger)
	mem.tracking_allocator_destroy(&_styx_context.tracking_allocator)
}

styx_context :: proc "contextless" () -> runtime.Context
{
	return _styx_context
}
