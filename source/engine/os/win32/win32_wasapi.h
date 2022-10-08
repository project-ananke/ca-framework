/* date = April 18th 2022 7:47 am */

#ifndef WIN32_WASAPI_H
#define WIN32_WASAPI_H

#define CO_CREATE_INSTANCE(name) \
HRESULT name(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID *ppv)
#define CO_INITIALIZE_EX(name) \
HRESULT name(LPVOID pvReserved, DWORD dwCoInit)
#define CO_UNINITIALIZE(name) \
void name()

typedef CO_CREATE_INSTANCE(Co_Create_Instance);
typedef CO_INITIALIZE_EX(Co_Initialize_Ex);
typedef CO_UNINITIALIZE(Co_Uninitialize);

CO_CREATE_INSTANCE(co_create_instance_stub)
{
    return 1;
}

CO_INITIALIZE_EX(co_initialize_ex_stub)
{
    return 1;
}

CO_UNINITIALIZE(co_uninitialize_stub)
{
    
}

// Pointer type to be consistent with other platform layer types?
typedef struct WASAPIModule WASAPIModule;
struct WASAPIModule
{
    HMODULE lib;
    
    Co_Create_Instance *CoCreateInstance;
    Co_Initialize_Ex *CoInitializeEx;
    Co_Uninitialize *CoUninitialize;
};

typedef struct WASAPIAudio WASAPIAudio;
struct WASAPIAudio
{
    WASAPIModule module;
    
    IMMDeviceEnumerator *enumerator;
    IMMDevice *device;
    IAudioClient *audio_client;
    IAudioRenderClient *audio_render_client;
    
    u32 samples_per_sec;
    u32 buffer_size_in_frames;
};

styx_function WASAPIModule win32_load_wasapi(void);
styx_function void win32_unload_wasapi(WASAPIModule *wasapi_module);

styx_function WASAPIAudio *win32_init_wasapi();
styx_function void win32_free_wasapi(WASAPIAudio *audio);

styx_function void win32_fill_sound_buffer(WASAPIAudio *audio, u32 samples_to_write, u16 *samples);

#endif //WIN32_WASAPI_H
