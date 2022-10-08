#ifndef STYXA_H
#define STYXA_H

typedef struct StyxAudioBuffer StyxAudioBuffer;
struct StyxAudioBuffer
{
    u16 *samples;
    
    u32 samples_per_sec;
    u32 sample_count;
};

styx_function StyxAudioBuffer styxa_init_audio_buffer();

#endif
