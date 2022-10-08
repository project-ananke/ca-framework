#include <math.h> // We're going to test out playing a sine wave.

#include "win32_wasapi.h"

// In C++, we will use __uuidof instead of defining each one of the GUIDs.
const styx_global GUID styx_IID_IAudioClient =
{ 0x1CB9AD4C, 0xDBFA, 0x4c32, 0xB1, 0x78, 0xC2, 0xF5, 0x68, 0xA7, 0x03, 0xB2 };
const styx_global GUID styx_IID_IAudioRenderClient =
{ 0xF294ACFC, 0x3146, 0x4483, 0xA7, 0xBF, 0xAD, 0xDC, 0xA7, 0xC2, 0x60, 0xE2 };
const styx_global GUID styx_CLSID_MMDeviceEnumerator =
{ 0xBCDE0395, 0xE52F, 0x467C, 0x8E, 0x3D, 0xC4, 0x57, 0x92, 0x91, 0x69, 0x2E };
const styx_global GUID styx_IID_IMMDeviceEnumerator =
{ 0xA95664D2, 0x9614, 0x4F35, 0xA7, 0x46, 0xDE, 0x8D, 0xB6, 0x36, 0x17, 0xE6 };
const styx_global GUID styx_PcmSubformatGuid = { STATIC_KSDATAFORMAT_SUBTYPE_PCM };

styx_function WASAPIModule
win32_load_wasapi(void)
{
	WASAPIModule wasapi_module = {
		.CoCreateInstance = co_create_instance_stub,
		.CoInitializeEx = co_initialize_ex_stub,
        .CoUninitialize = co_uninitialize_stub,
    }; 
    
	wasapi_module.lib = LoadLibraryA("ole32.dll");
	if (wasapi_module.lib) {
		wasapi_module.CoCreateInstance =
        (Co_Create_Instance *)GetProcAddress(wasapi_module.lib, "CoCreateInstance");
		wasapi_module.CoInitializeEx =
        (Co_Initialize_Ex *)GetProcAddress(wasapi_module.lib, "CoInitializeEx");
        wasapi_module.CoUninitialize =
        (Co_Uninitialize *)GetProcAddress(wasapi_module.lib, "CoUninitialize");
	} else {
		styx_os->debug_print("Failed to load ole32.dll.");
		// TODO(sir->w): Logging beyond assertions.
	}
    
	return wasapi_module;
}

styx_function void
win32_unload_wasapi(WASAPIModule *wasapi_module)
{
    wasapi_module->CoUninitialize();
	FreeLibrary(wasapi_module->lib);
    
	*wasapi_module = (WASAPIModule){0};
}

styx_function WASAPIAudio *
win32_init_wasapi()
{
	HRESULT hr = {0};
	styx_tctx *tctx = get_tctx();
	WASAPIAudio *audio = arena_push_struct(tctx->allocator, WASAPIAudio);
    
	audio->module = win32_load_wasapi();
	audio->module.CoInitializeEx(NULL, COINIT_SPEED_OVER_MEMORY);
    
	hr = audio->module.CoCreateInstance(&styx_CLSID_MMDeviceEnumerator,
										NULL, CLSCTX_ALL,
										&styx_IID_IMMDeviceEnumerator,
										(LPVOID *)&audio->enumerator);
	assert_msg_hr(hr, "CoCreateInstance error.");
    
	hr = IMMDeviceEnumerator_GetDefaultAudioEndpoint(audio->enumerator,
                                                     eRender, eConsole,
                                                     &audio->device);
	assert_msg_hr(hr, "IMMDeviceEnumerator_GetDefaultAudioEndpoint error.");
    
    hr = IMMDevice_Activate(audio->device,
                            &styx_IID_IAudioClient, CLSCTX_ALL, 0,
                            (void **)&audio->audio_client);
    assert_msg_hr(hr, "IMMDevice_Activate error.");
    
    WAVEFORMATEX wave_fmt = {0};
	wave_fmt.wFormatTag = WAVE_FORMAT_PCM;
	wave_fmt.nChannels = 2;
	wave_fmt.nSamplesPerSec = audio->samples_per_sec = 44100;
	wave_fmt.wBitsPerSample = 16;
	wave_fmt.nBlockAlign = (wave_fmt.nChannels * wave_fmt.wBitsPerSample) / 8;
	wave_fmt.nAvgBytesPerSec = wave_fmt.nSamplesPerSec * wave_fmt.nBlockAlign;
    
	REFERENCE_TIME buffer_duration = 10000000ULL * 2;
    
	// Read up more on the AUDCLNT_STREAMFLAGS.
    hr = IAudioClient_Initialize(audio->audio_client,
								 AUDCLNT_SHAREMODE_SHARED, 
                                 AUDCLNT_STREAMFLAGS_RATEADJUST | 
								 AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | 
								 AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY,
								 buffer_duration, 0, &wave_fmt, NULL);
    assert_msg_hr(hr, "IAudioClient_Initialize error.");
	
	hr = IAudioClient_GetService(audio->audio_client, 
								 &styx_IID_IAudioRenderClient, (void **)&audio->audio_render_client);
	assert_msg_hr(hr, "IAudioClient_GetService error.");
	
	hr = IAudioClient_GetBufferSize(audio->audio_client, &audio->buffer_size_in_frames);
	assert_msg_hr(hr, "IAudioClient_GetBufferSize error.");
	
	hr = IAudioClient_Start(audio->audio_client);
	assert_msg_hr(hr, "IAudioClient_Start error.");
	
	return audio;
}

styx_function void
win32_free_wasapi(WASAPIAudio *audio)
{
	IAudioClient_Stop(audio->audio_client);
	IAudioClient_Release(audio->audio_client);
	IAudioRenderClient_Release(audio->audio_render_client);
	
	win32_unload_wasapi(&audio->module);
	// Release all WASAPI pointers.
}

styx_function void 
win32_fill_sound_buffer(WASAPIAudio *audio, u32 samples_to_write, u16 *samples)
{
	if (samples) {
		s16 *buffer;
		IAudioRenderClient_GetBuffer(audio->audio_render_client, 
									 samples_to_write, (BYTE **)&buffer);
		if (buffer) {
			for (u32 frame_idx = 0; frame_idx < samples_to_write; ++frame_idx) {
				u32 left_sample_idx = 2 * frame_idx;
				u32 right_sample_idx = left_sample_idx + 1;
				u16 left_sample = samples[left_sample_idx];
				u16 right_sample = samples[right_sample_idx];
				
                *buffer++ = left_sample;
                *buffer++ = right_sample;
            }
            
            IAudioRenderClient_ReleaseBuffer(audio->audio_render_client, 
                                             samples_to_write, 0);
		}
	}
}