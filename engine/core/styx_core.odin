package styx_core

import "core:runtime"
import "core:log"
import "core:mem"
import "core:fmt"

@(private)
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