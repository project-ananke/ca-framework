package platform

import "core:log"
import "core:sync"

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
	color_buf: ^MTL.Buffer,
}

program_source :: `
using namespace metal;

struct ColoredVertex {
	float4 position [[position]];
	float4 color;
};

vertex ColoredVertex vertex_main(device packed_float3 *position [[buffer(0)]],
								 device float4 *color [[buffer(1)]],
								 uint vid [[vertex_id]]) {
	ColoredVertex vert;
	vert.position = float4(position[vid], 1.0);
	vert.color    = color[vid];
	return vert;
}

fragment float4 fragment_main(ColoredVertex vert [[stage_in]]) {
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
	pipeline_state_descriptor->colorAttachments()->object(0)->setPixelFormat(.BGRA8Unorm_sRGB)
	pipeline_state_descriptor->setVertexFunction(vertex_program)
	pipeline_state_descriptor->setFragmentFunction(fragment_program)

	pipeline_state = dev->newRenderPipelineStateWithDescriptor(pipeline_state_descriptor) or_return

	positions := [?][3]f32{
		{ 0.0,  1.0, 0.0},
		{-1.0, -1.0, 0.0},
		{ 1.0, -1.0, 0.0},
	}
	colors := [?][4]f32{
		{1, 0, 0, 1},
		{0, 1, 0, 1},
		{0, 0, 1, 1},
	}

	vertex_buf = dev->newBufferWithSlice(positions[:], {.StorageModeManaged})
	color_buf = dev->newBufferWithSlice(colors[:], {.StorageModeManaged})

	return
}

free_renderer :: proc(using renderer: ^MetalRenderer)
{
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
		//color_attachment->setClearColor(MTL.ClearColor{0.0, 0.0, 0.0, 1.0})
		// Defaults to loading the previous texture.
		color_attachment->setLoadAction(.Load)
		color_attachment->setStoreAction(.Store)
	}

	for i in 0..<renderer.write_at {
		switch renderer.cmd[i] {
		case .Clear:
			col := styxm.v3c_to_v3cf(renderer.data[i].(styx2d.RenderEntryClear).col)

			color_attachment->setClearColor(MTL.ClearColor{col.r, col.g, col.b, 1.0})
			color_attachment->setLoadAction(.Clear)
			color_attachment->setStoreAction(.Store)
		case .Triangle:
		}
	}
	styx2d.renderer_clear(renderer)

	render_encoder := command_buffer->renderCommandEncoderWithDescriptor(pass)
	render_encoder->setRenderPipelineState(pipeline_state)
	render_encoder->setVertexBuffer(vertex_buf, 0, 0)
	render_encoder->setVertexBuffer(color_buf, 0, 1)
	render_encoder->drawPrimitivesWithInstanceCount(.Triangle, 0, 3, 1)
	render_encoder->endEncoding()

	command_buffer->presentDrawable(drawable)
	command_buffer->commit()
}
