package platform

import "core:log"
import "core:sync"
import "core:math"
import "core:fmt"
import SDL "vendor:sdl2"

import NS "vendor:darwin/Foundation"
import MTL "vendor:darwin/Metal"
import CA "vendor:darwin/QuartzCore"

import "engine:core/styx2d"
import "engine:core/styxm"

// TODO(sir->w7): Implement error handling for Darwin system calls.
MetalRenderer :: struct
{
	native_window: ^NS.Window,

	dev: ^MTL.Device,
	swapchain: ^CA.MetalLayer,

	cmd_queue: ^MTL.CommandQueue,

	shader_program: ^MTL.Library,

	vertex_program: ^MTL.Function,
	fragment_program: ^MTL.Function,

	pipeline_state: ^MTL.RenderPipelineState,

	vertex_buf: ^MTL.Buffer,
	index_buf: ^MTL.Buffer,
	uniform_buf: ^MTL.Buffer,
}

// We can ponder future uses of instancing for this. For simplicity's sake,
// we will not use instancing. 
// InstanceData :: struct #align 16
// {
// 	transform: matrix[4, 4]f32, 
// 	color: [4]f32,
// }

// INSTANCES_MAX :: 64

Uniforms :: struct
{
	screen2d_ortho: matrix[4, 4]f32,
}


program_source :: `
#include <metal_stdlib>

using namespace metal;

struct VertexInput
{
	packed_float3 position;
	packed_float4 color;
};

struct ColoredVertex
{
	float4 position [[position]];
	float4 color;
};

struct Uniforms
{
	float4x4 screen2d_ortho; // Temporary, crappy orthographic projection. 
};

vertex ColoredVertex vertex_main(device const VertexInput *vertex_data    [[buffer(0)]],
								 device const Uniforms& uniforms          [[buffer(1)]],
								 uint vid                                 [[vertex_id]])
{
	ColoredVertex vert;
	
	float4 pos = float4(vertex_data[vid].position, 1.0);

	vert.position = uniforms.screen2d_ortho * pos;
	vert.color = vertex_data[vid].color;

	return vert;
}

fragment float4 fragment_main(ColoredVertex vert [[stage_in]])
{
	return vert.color;
}
`

// Plagiarized from GingerBill's Metal Github Gist.
init_renderer :: proc(window: ^SDL.Window, w, h: u32, vertex_count: u32) -> (renderer: MetalRenderer, err: ^NS.Error)
{
	using renderer

	window_system_info: SDL.SysWMinfo
	SDL.GetVersion(&window_system_info.version)
	SDL.GetWindowWMInfo(window, &window_system_info)
	assert(window_system_info.subsystem == .COCOA)

	native_window = cast(^NS.Window)window_system_info.info.cocoa.window
	dev = MTL.CreateSystemDefaultDevice()

	swapchain = CA.MetalLayer.layer()
	swapchain->setDevice(renderer.dev)
	swapchain->setPixelFormat(.BGRA8Unorm_sRGB)
	swapchain->setFramebufferOnly(true)
	swapchain->setFrame(native_window->frame())

	native_window->contentView()->setLayer(swapchain)
	native_window->setOpaque(true)
	native_window->setBackgroundColor(nil)

	cmd_queue = dev->newCommandQueue()

	compile_options := NS.new(MTL.CompileOptions)
	defer compile_options->release()

	shader_program = dev->newLibraryWithSource(NS.AT(program_source), compile_options) or_return

	vertex_program = shader_program->newFunctionWithName(NS.AT("vertex_main"))
	fragment_program = shader_program->newFunctionWithName(NS.AT("fragment_main"))

	assert(vertex_program != nil)
	assert(fragment_program != nil)

	pipeline_state_descriptor := NS.new(MTL.RenderPipelineDescriptor)
	defer pipeline_state_descriptor->release()

	pipeline_state_descriptor->colorAttachments()->object(0)->setPixelFormat(.BGRA8Unorm_sRGB)

	pipeline_state_descriptor->setVertexFunction(vertex_program)
	pipeline_state_descriptor->setFragmentFunction(fragment_program)

	pipeline_state = dev->newRenderPipelineStateWithDescriptor(pipeline_state_descriptor) or_return

	// 0, 1, 2, 2, 3, 0,
	// 4, 5, 6, 6, 7, 4,
	// ...
	indices: [6 * styx2d.MAX_VERTEX_COUNT]u16
	for i in 0..<styx2d.MAX_VERTEX_COUNT {
		indices[(6 * i)]     = u16((4 * i))
		indices[(6 * i) + 1] = u16((4 * i) + 1)
		indices[(6 * i) + 2] = u16((4 * i) + 2)
		indices[(6 * i) + 3] = u16((4 * i) + 2)
		indices[(6 * i) + 4] = u16((4 * i) + 3)
		indices[(6 * i) + 5] = u16((4 * i))
	}

	vertex_buf = dev->newBuffer(NS.UInteger(styx2d.MAX_VERTEX_COUNT * size_of(styx2d.Vertex)), { .StorageModeManaged })
	index_buf = dev->newBufferWithSlice(indices[:], { .StorageModeManaged })

	uniform_buf = dev->newBuffer(size_of(Uniforms), { .StorageModeManaged })

	{
		uniforms_data := uniform_buf->contentsAsType(Uniforms)
		uniforms_data.screen2d_ortho = styxm.screen2d_orthoproj(f32(w), f32(h))

		sz := NS.UInteger(size_of(Uniforms))
		uniform_buf->didModifyRange(NS.Range_Make(0, sz))
	}
	
	return
}

free_renderer :: proc(using renderer: ^MetalRenderer)
{
	vertex_buf->release()
	index_buf->release()
	uniform_buf->release()

	cmd_queue->release()
	swapchain->release()
	dev->release()
}

renderer_update :: proc(window: ^Window, renderer: ^styx2d.Renderer)
{
	using window._native_renderer

	NS.scoped_autoreleasepool()

	drawable := swapchain->nextDrawable()
	assert(drawable != nil)

	pass := MTL.RenderPassDescriptor.renderPassDescriptor()
	command_buffer := cmd_queue->commandBuffer()

	color_attachment := pass->colorAttachments()->object(0)
	assert(color_attachment != nil)

	{
		color_attachment->setTexture(drawable->texture())
		color_attachment->setClearColor(MTL.ClearColor{0.0, 0.0, 0.0, 1.0})
		color_attachment->setLoadAction(.Clear)
		color_attachment->setStoreAction(.Store)
	}

	render_encoder := command_buffer->renderCommandEncoderWithDescriptor(pass)

	vertex_count := renderer.write_at
	{
		vertex_data := vertex_buf->contentsAsSlice([]styx2d.Vertex)[:styx2d.MAX_VERTEX_COUNT]
		for idx := 0; idx < int(vertex_count); idx += 1{
			vertex_data[idx] = renderer.data[idx]
		}

		sz := NS.UInteger(vertex_count * size_of(styx2d.Vertex))
		vertex_buf->didModifyRange(NS.Range_Make(0, sz))

		styx2d.renderer_clear(renderer)
	}

	if vertex_count != 0 {
		render_encoder->setRenderPipelineState(pipeline_state)
		render_encoder->setVertexBuffer(vertex_buf, 0, 0)
		render_encoder->setVertexBuffer(uniform_buf, 0, 1)

		render_encoder->drawIndexedPrimitives(.Triangle, 
		                                      NS.UInteger(6 * (vertex_count / 4)), .UInt16, 
		                                      index_buf, 0)
	}

	render_encoder->endEncoding()

	command_buffer->presentDrawable(drawable)
	command_buffer->commit()
}
