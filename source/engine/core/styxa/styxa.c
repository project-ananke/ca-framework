#include "styxa_parser.c"

#include "styxa.h"

// TODO(sir->w): Base this off the vsync fps cap.
#define STYXA_FPS 60

styx_function StyxAudioBuffer
styxa_init_audio_buffer()
{
    styx_tctx *tctx = get_tctx();
    StyxAudioBuffer audio_buffer = {0};
    audio_buffer.samples_per_sec = 44100;
    audio_buffer.sample_count = audio_buffer.samples_per_sec / STYXA_FPS;
    audio_buffer.samples = arena_push_array(tctx->allocator,
                                            sizeof(u16), audio_buffer.sample_count);
    
    return audio_buffer;
}
