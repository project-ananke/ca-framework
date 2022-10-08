#include "styxa_parser.h"

styx_function styx_wav_file
styxa_load_wav(memory_arena *allocator, str8 path)
{
    styx_tctx *tctx = get_tctx();
    void *file_data = styx_os->read_entire_file(allocator, path);
    if (file_data == NULL) {
        return (styx_wav_file){0};
    }
    
    styx_wav_file_header *file_header = (styx_wav_file_header *)file_data;
    
    // Making sure the file header is what we expect.
    // Check if this file is even in the right format.
    assert_soft(file_header->riff_id == 1179011410);
    assert_soft(file_header->wave_id == 1163280727);
    assert_soft(file_header->fmt_id == 544501094);
    assert_soft(file_header->data_id == 1635017060);
    
    // Check data is in format we expect.
    assert_soft(file_header->fmt_code == 1); // Only support PCM data.
    assert_soft(file_header->num_channels == 2); // Only support 2-channel data.
    assert_soft(file_header->fmt_chunk_size == 16); // This should be true for PCM data.
    assert_soft(file_header->sample_rate == 44100); // Only support 44100Hz data.
    assert_soft(file_header->bits_per_sample == 16); // Only support 16-bit samples.
    
    // This is how these fields are defined,
    // but no harm to assert that they're what we expect.
    assert_soft(file_header->block_align ==
                file_header->num_channels * file_header->bits_per_sample / 8);
    assert_soft(file_header->byte_rate == file_header->sample_rate * file_header->block_align);
    
    styx_wav_file file = {
        .header = *file_header,
        .wav_samples_num = file_header->data_chunk_size /
        (file_header->num_channels * sizeof(u16)),
        .data = (u16 *)file_data + sizeof(styx_wav_file_header),
    };
    
    return file;
}
