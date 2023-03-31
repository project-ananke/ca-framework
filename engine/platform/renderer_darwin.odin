package platform

import "core:log"
import "core:sync"
import "core:math"

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
	instance_buf: ^MTL.Buffer,

	clear_col: styxm.Vec3cf,
}

InstanceData :: struct #align 16
{
	transform: matrix[4, 4]f32, 
	color: [4]f32,
}

INSTANCES_MAX :: 64

program_source :: `
#include <metal_stdlib>

using namespace metal;

struct InstanceData
{
	float4x4 transform;
	float4 color;
};

struct VertexInput
{
	packed_float3 position;
};

struct ColoredVertex
{
	float4 position [[position]];
	float4 color;
};

vertex ColoredVertex vertex_main(device const VertexInput *vertex_data    [[buffer(0)]],
								 device const InstanceData *instance_data [[buffer(1)]],
								 uint vid                                 [[vertex_id]],
								 uint iid                                 [[instance_id]])
{
	ColoredVertex vert;
	
	float4 pos = float4(vertex_data[vid].position, 1.0);

	vert.position = instance_data[iid].transform * pos;
	vert.color = instance_data[iid].color;

	return vert;
}

fragment float4 fragment_main(ColoredVertex vert [[stage_in]])
{
	return vert.color;
}
`

// Plagiarized from GingerBill's Metal Github Gist.
init_renderer :: proc(window: ^SDL.Window) -> (renderer: MetalRenderer, err: ^NS.Error)
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

	positions := [?][3]f32{
		{ -0.5, -0.5, 0.5 },
		{  0.5, -0.5, 0.5 },
		{  0.5,  0.5, 0.5 },
		{ -0.5,  0.5, 0.5 },
	}
	indices := [?]u16{
		0, 1, 2,
		2, 3, 0,
	}

	vertex_buf = dev->newBufferWithSlice(positions[:], { .StorageModeManaged })
	index_buf = dev->newBufferWithSlice(indices[:], { .StorageModeManaged })
	instance_buf = dev->newBuffer(INSTANCES_MAX * size_of(InstanceData), { .StorageModeManaged })

	return
}

free_renderer :: proc(using renderer: ^MetalRenderer)
{
	vertex_buf->release()
	index_buf->release()
	instance_buf->release()

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
		color_attachment->setClearColor(MTL.ClearColor{0.25, 0.5, 1.0, 1.0})
		// color_attachment->setLoadAction(.Load)
		color_attachment->setLoadAction(.Clear)
		color_attachment->setStoreAction(.Store)
	}

	{
		@static angle: f32
		angle += 0.01
		instance_data := instance_buf->contentsAsSlice([]InstanceData)[:INSTANCES_MAX]
		for instance, idx in &instance_data {
			scl :: 0.1

			i := f32(idx) / INSTANCES_MAX
			xoff := (i*2 - 1) + (1.0/INSTANCES_MAX)
			yoff := math.sin((i + angle) * math.TAU)
			instance.transform = matrix[4, 4]f32{
				scl * math.sin(angle),  scl * math.cos(angle), 0, xoff,
				scl * math.cos(angle), -scl * math.sin(angle), 0, yoff,
				                    0,                      0, 0,    0,
				                    0,                      0, 0,    1,
			}
			instance.color = {i, 1-i, math.sin(math.TAU * i), 1}
		}
		sz := NS.UInteger(len(instance_data)*size_of(instance_data[0]))
		instance_buf->didModifyRange(NS.Range_Make(0, sz))
	}
	// for i in 0..<renderer.write_at {
	// 	switch renderer.cmd[i] {
	// 	case .Clear:
	// 		clear_col = styxm.v3c_to_v3cf(renderer.data[i].(styx2d.RenderEntryClear).col)

	// 		color_attachment->setClearColor(MTL.ClearColor{clear_col.r, clear_col.g, clear_col.b, 1.0})
	// 		color_attachment->setLoadAction(.Clear)
	// 		color_attachment->setStoreAction(.Store)
	// 	case .Triangle:
	// 	}
	// }

	renderer.write_at = 0

	render_encoder := command_buffer->renderCommandEncoderWithDescriptor(pass)
	defer render_encoder->release()

	render_encoder->setRenderPipelineState(pipeline_state)
	render_encoder->setVertexBuffer(vertex_buf, 0, 0)
	render_encoder->setVertexBuffer(instance_buf, 0, 1)
	
	render_encoder->drawIndexedPrimitivesWithInstanceCount(.Triangle, 6, .UInt16, index_buf, 0, INSTANCES_MAX)

	render_encoder->endEncoding()

	command_buffer->presentDrawable(drawable)
	command_buffer->commit()
}
