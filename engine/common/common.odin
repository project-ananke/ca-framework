package common

import "core:runtime"
import "core:mem"
import "core:log"
import "core:fmt"

Rule :: struct 
{
    grid_width, grid_height: u32,
    grid: []u8,
}

free_rule :: proc(rule: ^Rule)
{
    delete(rule.grid)
}

@(private)
_styx_context: struct{
	tracking_allocator: mem.Tracking_Allocator,

	using runtime_ctx: runtime.Context,
}

@private
Logging_Allocator_Data :: struct {
	backing: mem.Allocator,
}

// @private
// logging_allocator :: proc(data: ^Logging_Allocator_Data) -> mem.Allocator {
// 	// `package log` uses context.temp_allocator and temp_allocator is lazily initialized.  Dummy allocate here
// 	// to make sure it doesn't use context.allocator within our custom allocator procedure.
// 	dummy := new(int, context.temp_allocator);
  
// 	return mem.Allocator{
// 		data = data,
// 		procedure = proc(allocator_data: rawptr, mode: mem.Allocator_Mode, size, alignment: int, old_memory: rawptr, old_size: int, flags: u64 = 0, loc := #caller_location) -> (result: rawptr) {
// 			data := cast(^Logging_Allocator_Data) allocator_data;
// 			log.infof("{}: size={} alignment={} old_memory={} old_size={}, flags={}, loc={}",
// 			           mode, size, alignment, old_memory, old_size, flags, loc);
// 			result = data.backing.procedure(data.backing.data, mode, size, alignment, old_memory, old_size, flags, loc);
// 			log.infof(" = %p", result);
// 			return;
// 		},
// 	};
// }

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
