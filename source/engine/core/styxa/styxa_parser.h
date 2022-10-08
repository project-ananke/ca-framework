#ifndef IMG_PARSER
#define IMG_PARSER

pack_struct(styx_wav_file_header);
typedef struct styx_wav_file_header styx_wav_file_header;
struct styx_wav_file_header
{
    // RIFF chunk
    u32 riff_id;
    u32 riff_chunk_size;
    u32 wave_id;
    
    // fmt chunk
    u32 fmt_id;
    u32 fmt_chunk_size;
    u16 fmt_code;
    u16 num_channels;
    u32 sample_rate;
    u32 byte_rate;
    u16 block_align;
    u16 bits_per_sample;
    
    u32 data_id;
    u32 data_chunk_size;
};

typedef struct styx_wav_file styx_wav_file;
struct styx_wav_file
{
    styx_wav_file_header header;
    u32 wav_samples_num;
    u16 *data;
};

// Dynamic allocation because why not?
styx_function styx_wav_file styxa_load_wav(memory_arena *allocator, str8 path);

#endif
