#include "styx2d_parser.h"

#define skip_bytes(mem, byte_num) (mem += byte_num)

styx_inline u32 
styx_get_u32be(u8 *mem)
{
    return mem[0] << 24 | mem[1] << 16 | mem[2] << 8 | mem[3];
}

styx_inline u16 
styx_get_u16be(u8 *mem)
{
    return mem[0] << 8 | mem[1];
}

styx_function styx_png_chunk
styx2d_read_png_chunk(memory_arena *allocator, u8 *mem)
{
    styx_png_chunk chunk = {0};
    
    chunk.data_len = styx_get_u32be(mem);
    *((u32 *)&chunk.type) = *((u32 *)mem + offset_of(styx_png_chunk, type));
    chunk.data = arena_push_array(allocator, sizeof(u8), chunk.data_len);
    memory_copy(chunk.data, mem + offset_of(styx_png_chunk, data), chunk.data_len);
    chunk.crc32 = styx_get_u32be(mem + offset_of(styx_png_chunk, data) + chunk.data_len);
    
    return chunk;
}

styx_function styx_zlib_block 
styx2d_read_zlib_block(memory_arena *allocator, u8 *mem, u32 len)
{
    styx_zlib_block block = {0};
    block.cmf = *mem;
    skip_bytes(mem, 1);
    
    block.extra_flags = *mem;
    skip_bytes(mem, 1);
    
    // NOTE(sir->w): 4 bytes for the cmf, extra_flags, and check_val.
    block.data = arena_push(allocator, len - 4);
    memory_copy(block.data, mem, len - 2);
    skip_bytes(mem, len - 2);
    
    block.check_val = styx_get_u16be(mem);
    
    return block;
}

styx_function styx_png_file
styx2d_load_png(memory_arena *allocator, str8 path)
{
    void *file_data = styxos_read_entire_file(allocator, path);
    styx_png_file_header *header = (styx_png_file_header *)file_data;
    
    // NOTE(sir->w): Basic assertions to verify this is PNG file.
    assert_soft(header->high_bit == 0x89);
    
    assert_soft(header->p == 0x50);
    assert_soft(header->n == 0x4E);
    assert_soft(header->g == 0x47);
    
    styx_png_file file = {
        .header = *header,
    };
    
    return file;
}

styx_function styx_bmp_file 
styx2d_load_bmp(memory_arena *allocator, str8 path)
{
    void *file_data = styxos_read_entire_file(allocator, path);
    styx_bmp_file_header *header = (styx_bmp_file_header *)file_data;
    
    assert_soft(header->header.signature[0] == 'B');
    assert_soft(header->header.signature[1] == 'M');
    
    styx_bmp_file file = {
        .header = header,
        .pixel_cnt = header->info_header.width * header->info_header.height,
        .bmp_data = (u8 *)file_data + header->header.data_offset,
    };
    
    return file;
}
