#include "win32_gfx.h"

styx_function StyxRenderer * 
win32_init_renderer(HWND hwnd)
{
    styx_tctx *tctx = get_tctx();
    StyxRenderer *renderer = arena_push_struct(tctx->allocator, StyxRenderer);
    HRESULT hr = {0};
    
    // Creating the device and the device context.
    {
        UINT flags = 0;
#if STYX_DEBUG
        flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
        
        D3D_FEATURE_LEVEL lvl[] = { D3D_FEATURE_LEVEL_11_0 };
        hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL,
                               flags,
                               lvl, array_count(lvl),
                               D3D11_SDK_VERSION,
                               &renderer->dev, NULL, &renderer->dev_ctx);
        assert_hr(hr);
    }
    
    // Getting the program to debugger break at an error, so we don't need
    // to individually assert each HRESULT.
#if STYX_DEBUG
    {
        ID3D11InfoQueue *info;
        ID3D11Device_QueryInterface(renderer->dev, &IID_ID3D11InfoQueue, (void **)&info);
        ID3D11InfoQueue_SetBreakOnSeverity(info, D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        ID3D11InfoQueue_SetBreakOnSeverity(info, D3D11_MESSAGE_SEVERITY_ERROR, TRUE);
        ID3D11InfoQueue_Release(info);
    }
#endif
    
    {
        IDXGIFactory2 *factory;
        hr = CreateDXGIFactory(&IID_IDXGIFactory2, &factory);
        assert_hr(hr);
        
        DXGI_SWAP_CHAIN_DESC1 desc = {
            .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
            .SampleDesc = { 1, 0 },
            .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
            .BufferCount = 2,
            .Scaling = DXGI_SCALING_NONE,
            .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
        };
        
        hr = IDXGIFactory2_CreateSwapChainForHwnd(factory, (IUnknown *)renderer->dev, hwnd,
                                                  &desc, NULL, NULL,
                                                  &renderer->swap_chain);
        assert_hr(hr);
        
        IDXGIFactory_Release(factory);
    }
    
    {
        ID3D11Texture2D *back_buffer;
        IDXGISwapChain1_GetBuffer(renderer->swap_chain, 0, &IID_ID3D11Texture2D, (void **)&back_buffer);
        ID3D11Device_CreateRenderTargetView(renderer->dev, 
                                            (ID3D11Resource *)back_buffer, NULL,
                                            &renderer->render_target);
        ID3D11Texture2D_Release(back_buffer);
    }
    
    {
#if STYX_DEBUG
        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;
        ID3DBlob *vs_blob = NULL, *ps_blob = NULL, *err_blob = NULL;
        
        // TODO(sir->w): Copy shaders to assets during compilation, so we can easily ship the game without needing to ship the engine code as well.
        // TODO(sir->w): Do our own engine reading, so we can read the shaders into our arena -> full engine memory management.
        hr = D3DCompileFromFile(L"../source/engine/shaders/shader.hlsl",
                                NULL,
                                D3D_COMPILE_STANDARD_FILE_INCLUDE,
                                "vs_main", "vs_5_0",
                                flags, 0,
                                &vs_blob, &err_blob);
        if (FAILED(hr)) {
            if (err_blob) {
                OutputDebugStringA((char *)ID3D10Blob_GetBufferPointer(err_blob));
                ID3D10Blob_Release(err_blob);
                // TODO(sir->w): Free the D3D11 rendering devices and such because if rendering is broken, a game engine is basically missing a limb.
            }
            if (vs_blob) ID3D10Blob_Release(vs_blob);
            assert_hr(hr); // Already going to fail. Just for better debugging.
        }
        
        hr = D3DCompileFromFile(L"../source/engine/shaders/shader.hlsl",
                                NULL,
                                D3D_COMPILE_STANDARD_FILE_INCLUDE,
                                "ps_main", "ps_5_0",
                                flags, 0,
                                &ps_blob, &err_blob);
        if (FAILED(hr)) {
            if (err_blob) {
                OutputDebugStringA((char *)ID3D10Blob_GetBufferPointer(err_blob));
                ID3D10Blob_Release(err_blob);
                // TODO(sir->w): Free the D3D11 rendering devices and such because if rendering is broken, a game engine is basically missing a limb.
            }
            if (vs_blob) ID3D10Blob_Release(vs_blob);
            assert_hr(hr); // Already going to fail. Just for better debugging.
        }
        
        hr = ID3D11Device_CreateVertexShader(renderer->dev,
                                             ID3D10Blob_GetBufferPointer(vs_blob),
                                             ID3D10Blob_GetBufferSize(vs_blob),
                                             NULL, 
                                             &renderer->vertex_shader);
        assert_hr(hr);
        
        hr = ID3D11Device_CreatePixelShader(renderer->dev,
                                            ID3D10Blob_GetBufferPointer(ps_blob),
                                            ID3D10Blob_GetBufferSize(ps_blob),
                                            NULL, 
                                            &renderer->pixel_shader);
        assert_hr(hr);
        
        
        D3D11_INPUT_ELEMENT_DESC element_desc[] = {
            { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        
        hr = ID3D11Device_CreateInputLayout(renderer->dev,
                                            element_desc, array_count(element_desc),
                                            ID3D10Blob_GetBufferPointer(vs_blob),
                                            ID3D10Blob_GetBufferSize(vs_blob),
                                            &renderer->input_layout);
        assert_hr(hr);
        
        f32 vertex_data_arr[] = {
            0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f
        };
        
        D3D11_BUFFER_DESC buf_desc = {
            .ByteWidth = sizeof(vertex_data_arr),
            .Usage = D3D11_USAGE_DYNAMIC,
            .BindFlags = D3D11_BIND_VERTEX_BUFFER,
            .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
        };
        
        D3D11_SUBRESOURCE_DATA sub_rsrc = {
            .pSysMem = vertex_data_arr,
        };
        
        hr = ID3D11Device_CreateBuffer(renderer->dev,
                                       &buf_desc, &sub_rsrc,
                                       &renderer->vertex_buffer);
        
        assert_hr(hr);
        
        ID3D10Blob_Release(vs_blob);
        ID3D10Blob_Release(ps_blob);
#else
        // TODO(sir->w): We do some cool precompiled shader loading here.
#endif
    }
    
    return renderer;
}

styx_function void 
win32_free_renderer(StyxRenderer *renderer)
{
    if (renderer->dev) ID3D11Device_Release(renderer->dev);
    if (renderer->dev_ctx) ID3D11DeviceContext_Release(renderer->dev_ctx);
    if (renderer->swap_chain) IDXGISwapChain1_Release(renderer->swap_chain);
    if (renderer->render_target) ID3D11RenderTargetView_Release(renderer->render_target);
    
    if (renderer->vertex_shader) ID3D11VertexShader_Release(renderer->vertex_shader);
    if (renderer->pixel_shader) ID3D11PixelShader_Release(renderer->pixel_shader);
    
    if (renderer->input_layout) ID3D11InputLayout_Release(renderer->input_layout);
    if (renderer->vertex_buffer) ID3D11Buffer_Release(renderer->vertex_buffer);
    
    *renderer = (StyxRenderer){0};
}

styx_function void 
win32_window_resize(StyxRenderer *renderer)
{
    ID3D11DeviceContext_ClearState(renderer->dev_ctx);
    ID3D11RenderTargetView_Release(renderer->render_target);
    renderer->render_target = NULL;
    
    IDXGISwapChain1_ResizeBuffers(renderer->swap_chain,
                                  2, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
    
    ID3D11Texture2D *back_buffer;
    IDXGISwapChain1_GetBuffer(renderer->swap_chain, 0, &IID_ID3D11Texture2D, (void **)&back_buffer);
    ID3D11Device_CreateRenderTargetView(renderer->dev, 
                                        (ID3D11Resource *)back_buffer, NULL,
                                        &renderer->render_target);
    ID3D11Texture2D_Release(back_buffer);
}

styx_function void 
win32_clear_window(StyxRenderer *renderer, vec3c col)
{
    f32 f32_col[] = { (f32)col.r / 255.0f, (f32)col.g / 255.0f, (f32)col.b / 255.0f, 1.0f };
    ID3D11DeviceContext_ClearRenderTargetView(renderer->dev_ctx, renderer->render_target,
                                              f32_col);
}

styx_function void 
win32_handle_queue(StyxRenderQueue *queue, StyxRenderer *renderer,
                   u32 width, u32 height)
{
    for (int read_idx = 0; read_idx < QUEUE_MAX_JOBS; ++read_idx) {
        if (queue->jobs[read_idx] == QueueJob_None) {
            queue->write_idx = 0;
            break;
        }
        
        switch (queue->jobs[read_idx]) {
            case QueueJob_Clear: {
                win32_clear_window(renderer, queue->job_data[read_idx].clear_data.color);
            } break;
            case QueueJob_Triangle: {
                // TODO(sir->w): Padding and aligning to make best use of CPU cache.
#define get_triangle_var(var) queue->job_data[read_idx].triangle_data.var
                vec3f coord1 = get_triangle_var(coord1);
                vec3f coord2 = get_triangle_var(coord2);
                vec3f coord3 = get_triangle_var(coord3);
                
                vec3f col1 = vec3f_lit(((f32)get_triangle_var(col1).r) / 255.0f,
                                       ((f32)get_triangle_var(col1).g) / 255.0f,
                                       ((f32)get_triangle_var(col1).b) / 255.0f);
                vec3f col2 = vec3f_lit(((f32)get_triangle_var(col2).r) / 255.0f,
                                       ((f32)get_triangle_var(col2).g) / 255.0f,
                                       ((f32)get_triangle_var(col2).b) / 255.0f);
                vec3f col3 = vec3f_lit(((f32)get_triangle_var(col3).r) / 255.0f,
                                       ((f32)get_triangle_var(col3).g) / 255.0f,
                                       ((f32)get_triangle_var(col3).b) / 255.0f);
#undef get_triangle_var
                
                f32 vertices[] = {
                    coord1.x, coord1.y, coord1.z, col1.x, col1.y, col1.z,
                    coord2.x, coord2.y, coord2.z, col2.x, col2.y, col2.z,
                    coord3.x, coord3.y, coord3.z, col3.x, col3.y, col3.z,
                };
                
                u32 vertex_stride = 2 * sizeof(vec3f);
                u32 vertex_offset = 0;
                u32 vertex_count = 3;
                
                D3D11_MAPPED_SUBRESOURCE mapped_sbres = {0};
                ID3D11DeviceContext_Map(renderer->dev_ctx,
                                        (ID3D11Resource *)renderer->vertex_buffer, 0,
                                        D3D11_MAP_WRITE_DISCARD, 0, &mapped_sbres);
                memory_copy(mapped_sbres.pData, vertices, sizeof(vertices));
                
                ID3D11DeviceContext_Unmap(renderer->dev_ctx, 
                                          (ID3D11Resource *)renderer->vertex_buffer, 0);
                
                // NOTE(sir->w): Input assembler.
                ID3D11DeviceContext_IASetPrimitiveTopology(renderer->dev_ctx, 
                                                           D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                ID3D11DeviceContext_IASetInputLayout(renderer->dev_ctx,
                                                     renderer->input_layout);
                ID3D11DeviceContext_IASetVertexBuffers(renderer->dev_ctx, 
                                                       0, 1, &renderer->vertex_buffer, 
                                                       &vertex_stride, &vertex_offset);
                
                // NOTE(sir->w): Vertex shader stage.
                ID3D11DeviceContext_VSSetShader(renderer->dev_ctx,
                                                renderer->vertex_shader, NULL, 0);
                // NOTE(sir->w): Rasterizer stage.
                D3D11_VIEWPORT viewport = { .Width = (FLOAT)width, .Height = (FLOAT)height };
                ID3D11DeviceContext_RSSetViewports(renderer->dev_ctx, 1, &viewport);
                
                // NOTE(sir->w): Pixel shader.
                ID3D11DeviceContext_PSSetShader(renderer->dev_ctx,
                                                renderer->pixel_shader, NULL, 0);
                
                // NOTE(sir->w): Output merger.
                ID3D11DeviceContext_OMSetRenderTargets(renderer->dev_ctx, 
                                                       1, &renderer->render_target, NULL);
                
                ID3D11DeviceContext_Draw(renderer->dev_ctx, vertex_count, 0);
            } break;
            default: break;
        }
        
        queue->jobs[read_idx] = QueueJob_None;
        queue->job_data[read_idx] = (StyxQueueJobData){0};
    }
}

styx_function void
win32_update_window(StyxRenderQueue *queue, 
                    StyxRenderer *renderer, 
                    u32 width, u32 height, b32 vsync)
{
    win32_handle_queue(queue, renderer, width, height);
    
    // TODO(sir->w): Cleanup the D3D11 rendering pipeline to make it more logical and true to documentation canon.
    IDXGISwapChain1_Present(renderer->swap_chain, vsync ? 1 : 0, 0);
}