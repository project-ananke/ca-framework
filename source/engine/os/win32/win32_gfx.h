/* date = April 15th 2022 8:44 pm */

#ifndef WIN32_GFX_H
#define WIN32_GFX_H

typedef struct StyxRenderer StyxRenderer;
struct StyxRenderer
{
    ID3D11Device *dev;
    ID3D11DeviceContext *dev_ctx;
    
    IDXGISwapChain1 *swap_chain;
    ID3D11RenderTargetView *render_target;
    
    ID3D11VertexShader *vertex_shader;
    ID3D11PixelShader *pixel_shader;
    
    ID3D11InputLayout *input_layout;
    ID3D11Buffer *vertex_buffer;
};

styx_function StyxRenderer *win32_init_renderer(HWND hwnd);
styx_function void win32_free_renderer(StyxRenderer *renderer);

styx_function void win32_window_resize(StyxRenderer *renderer);
styx_function void win32_clear_window(StyxRenderer *renderer, vec3c col);

styx_function void win32_handle_queue(StyxRenderQueue *queue, StyxRenderer *renderer, u32 width, u32 height);
styx_function void win32_update_window(StyxRenderQueue *queue, StyxRenderer *renderer,  u32 width, u32 height, b32 vsync);

#endif //WIN32_GFX_H
