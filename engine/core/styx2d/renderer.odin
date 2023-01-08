package styx2d

import "engine:common"
import "engine:core/styxm"

// NOTE(sir->w7): This is the classic sir->w7 implementation of the FIFO stack-based
// rendering pipeline, and it surely isn't the most effective solution to this. Maybe
// look for better ways to implement this system.
RenderEntryCmd :: enum
{
	Clear,
	Triangle,
}

RenderEntryClear :: struct
{
	col: styxm.Vec3c,
}

// Maybe not use a union for this.
RenderData :: union
{
	RenderEntryClear,
}

// TODO(sir->w7): Work on a resizable command buffer.
RENDERER_COMMANDS_MAX :: 64
Renderer :: struct
{
	cmd: [RENDERER_COMMANDS_MAX]RenderEntryCmd,
	data: [RENDERER_COMMANDS_MAX]RenderData,

	write_at: u32,
}

// What will I use this function for?
init_renderer :: proc() -> (renderer: Renderer)
{
	return
}

free_renderer :: proc(using renderer: ^Renderer)
{
	return
}

renderer_clear :: proc(using renderer: ^Renderer)
{
	write_at = 0
}

push_clear :: proc(using renderer: ^Renderer, col: styxm.Vec3c)
{
	assert(write_at < RENDERER_COMMANDS_MAX)
	
	cmd[write_at] = .Clear
	data[write_at] = RenderEntryClear{ col }

	write_at += 1
}
