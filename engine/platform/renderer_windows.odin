package platform

import win32 "core:sys/windows"

import D3D11 "vendor:directx/d3d11"
import DXGI "vendor:directx/dxgi"
import D3D "vendor:directx/d3d_compiler"

import "engine:core/styx2d"
import "engine:core/styxm"

D3D11Renderer :: struct
{
	dev: ^D3D11.IDevice,
	dev_ctx: ^D3D11.IDeviceContext,
	
	swapchain: ^DXGI.ISwapChain1,
	ren_target_view: ^D3D11.IRenderTargetView,

	vertex_shader: ^D3D11.IVertexShader,
	pixel_shader: ^D3D11.IPixelShader,

	input_layout: ^D3D11.IInputLayout,

	vertex_buffer: ^D3D11.IBuffer,
}

init_renderer :: proc(hwnd: win32.HWND, w: u32, h: u32) -> (renderer: D3D11Renderer)
{
	feature_levels := [?]D3D11.FEATURE_LEVEL{._11_0}
	
	base_dev: ^D3D11.IDevice
	base_dev_ctx: ^D3D11.IDeviceContext
	
	D3D11.CreateDevice(nil, .HARDWARE,
					   nil, {.BGRA_SUPPORT, .DEBUG},
					   &feature_levels[0], len(feature_levels), 
					   D3D11.SDK_VERSION,
					   &base_dev, nil, &base_dev_ctx)
	
	base_dev->QueryInterface(D3D11.IDevice_UUID, (^rawptr)(&renderer.dev))
	base_dev_ctx->QueryInterface(D3D11.IDeviceContext_UUID, (^rawptr)(&renderer.dev_ctx))
	
	dxgi_device: ^DXGI.IDevice
	renderer.dev->QueryInterface(DXGI.IDevice_UUID, (^rawptr)(&dxgi_device))

	dxgi_adapter: ^DXGI.IAdapter
	dxgi_device->GetAdapter(&dxgi_adapter)

	dxgi_factory: ^DXGI.IFactory2
	dxgi_adapter->GetParent(DXGI.IFactory2_UUID, (^rawptr)(&dxgi_factory))
	
	swapchain_desc := DXGI.SWAP_CHAIN_DESC1{
		Width  = 0,
		Height = 0,
		Format = .B8G8R8A8_UNORM_SRGB,
		Stereo = false,
		SampleDesc = {
			Count   = 1,
			Quality = 0,
		},
		BufferUsage = {.RENDER_TARGET_OUTPUT},
		BufferCount = 2,
		Scaling     = .STRETCH,
		SwapEffect  = .DISCARD,
		AlphaMode   = .UNSPECIFIED,
		Flags       = 0,
	}

	dxgi_factory->CreateSwapChainForHwnd(renderer.dev, hwnd,
										 &swapchain_desc, nil, nil, 
										 &renderer.swapchain)
	framebuffer: ^D3D11.ITexture2D
	renderer.swapchain->GetBuffer(0, D3D11.ITexture2D_UUID, (^rawptr)(&framebuffer))
	defer framebuffer->Release()
	
	renderer.dev->CreateRenderTargetView(framebuffer, nil, &renderer.ren_target_view)
	
	// TODO (sir-w7): Have the engine know where the executable is, so we can have 
	// absolute paths and not have to deal with relative path bullcrap.
	vs_blob, ps_blob, err_blob: ^D3D11.IBlob
	hr := D3D.CompileFromFile(L("shaders/shader.hlsl"),
	                          nil, nil,
	                          "vs_main", "vs_5_0",
	                          u32(D3D.D3DCOMPILE_FLAG.ENABLE_STRICTNESS | D3D.D3DCOMPILE_FLAG.DEBUG), 0, 
	                          &vs_blob, &err_blob)
	if !win32.SUCCEEDED(hr) {
		if err_blob != nil {
        	win32.OutputDebugStringA(cstring(err_blob->GetBufferPointer()))
        	err_blob->Release()
      	}
      	if vs_blob != nil do vs_blob->Release()
      	assert(false)
	}

	hr = D3D.CompileFromFile(win32.L("shaders/shader.hlsl"),
	                         nil, nil,
	                         "ps_main", "ps_5_0",
	                         u32(D3D.D3DCOMPILE_FLAG.ENABLE_STRICTNESS | D3D.D3DCOMPILE_FLAG.DEBUG), 0, 
	                         &ps_blob, &err_blob)
	if !win32.SUCCEEDED(hr) {
		if err_blob != nil {
        	win32.OutputDebugStringA(cstring(err_blob->GetBufferPointer()))
        	err_blob->Release()
      	}
      	if ps_blob != nil do ps_blob->Release()
      	assert(false)
	}

	hr = renderer.dev->CreateVertexShader(vs_blob->GetBufferPointer(),
	                                      vs_blob->GetBufferSize(),
	                                      nil, &renderer.vertex_shader)
	hr = renderer.dev->CreatePixelShader(ps_blob->GetBufferPointer(),
	                                     ps_blob->GetBufferSize(),
	                                     nil, &renderer.pixel_shader)

	input_element_desc := [?]D3D11.INPUT_ELEMENT_DESC{
		{ "POS", 0, .R32G32B32_FLOAT, 0, 0, .VERTEX_DATA, 0 },
	}

	hr = renderer.dev->CreateInputLayout(&input_element_desc[0], len(input_element_desc),
	                                	 vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(),
	                                	 &renderer.input_layout)

	vertex_data_array := [?]f32 {
		0.0, 0.5, 0.0,
		0.5, -0.5, 0.0,
		-0.5, -0.5, 0.0,
	}

	buffer_desc := D3D11.BUFFER_DESC {
		ByteWidth = size_of(vertex_data_array),
		Usage = .DEFAULT,
		BindFlags = { .VERTEX_BUFFER },
	}

	sr_data := D3D11.SUBRESOURCE_DATA {
		pSysMem = &vertex_data_array[0],
	}

	hr = renderer.dev->CreateBuffer(&buffer_desc, &sr_data, &renderer.vertex_buffer)
	assert(win32.SUCCEEDED(hr))

	return
}

free_renderer :: proc(using renderer: ^D3D11Renderer)
{
	vertex_shader->Release()
	pixel_shader->Release()

	dev->Release()
	dev_ctx->Release()
	swapchain->Release()
}

window_clear :: proc(window: ^Window, col: styxm.Vec3c)
{
	using window._native_renderer

	clear_col := [4]f32{f32(col.r) / 255.0, f32(col.g) / 255.0, f32(col.b) / 255.0, 1.0}
	dev_ctx->ClearRenderTargetView(ren_target_view, &clear_col)		
}

renderer_update :: proc(window: ^Window, renderer: ^styx2d.Renderer)
{
	using window._native_renderer

	stride: u32 = 3 * size_of(f32)
	offset: u32 = 0
	count:  u32 = 3
	
	viewport: D3D11.VIEWPORT = {
		0.0, 0.0,
		f32(window.width), f32(window.height),
  		0.0, 1.0,
  	}

	dev_ctx->RSSetViewports(1, &viewport)


	dev_ctx->OMSetRenderTargets(1, &ren_target_view, nil)

	dev_ctx->IASetPrimitiveTopology(.TRIANGLELIST)
	dev_ctx->IASetInputLayout(input_layout)
	dev_ctx->IASetVertexBuffers(0, 1, 
	                            &vertex_buffer, 
	                            &stride, &offset)

	dev_ctx->VSSetShader(vertex_shader, nil, 0)
	dev_ctx->PSSetShader(pixel_shader, nil, 0)

	dev_ctx->Draw(count, 0)

	swapchain->Present(0, 0)
}