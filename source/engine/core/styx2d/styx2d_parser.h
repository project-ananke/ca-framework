/* date = May 1st 2022 9:26 pm */

#ifndef STYX2D_PARSER_H
#define STYX2D_PARSER_H

styx_inline u32 styx_get_u32be(u8 *mem);
styx_inline u16 styx_get_u16be(u8 *mem);

//-
// TODO(sir->w): PNG file loading. Currently on hiatus to make time for work on bmp file loading.
// NOTE(sir->w): File header, signature thing.
pack_struct(styx_png_file_header);
typedef struct styx_png_file_header styx_png_file_header;
struct styx_png_file_header
{
    u32 high_bit;
    
    u8 p;
    u8 n;
    u8 g;
    
    u16 dos_ending;
    u8 dos_type_ending;
    u8 unix_ending;
};

typedef struct styx_png_chunk styx_png_chunk;
struct styx_png_chunk
{
    u32 data_len;
    u8 type[4];
    u8 *data;
    u32 crc32;
};

typedef struct styx_zlib_block styx_zlib_block;
struct styx_zlib_block
{
    u8 cmf;
    u8 extra_flags;
    u8 *data;
    u16 check_val;
};

typedef struct styx_png_file styx_png_file;
struct styx_png_file
{
    styx_png_file_header header;
};

styx_function styx_png_chunk styx2d_read_png_chunk(memory_arena *allocator, u8 *mem);
styx_function styx_zlib_block styx2d_read_zlib_block(memory_arena *allocator, u8 *mem, u32 len);
styx_function styx_png_file styx2d_load_png(memory_arena *allocator, str8 path);
//-
// NOTE(sir->w): BMP File parsing.
// TODO(sir->w): BMP file writing.
pack_struct(styx_bmp_file_header);
typedef struct styx_bmp_file_header styx_bmp_file_header;
struct styx_bmp_file_header
{
    struct {
        u8 signature[2];
        u32 file_size;
        u32 reserved;
        u32 data_offset;
    } header;
    union {
        u32 size;
        u32 width;
        u32 height;
        u16 planes;
        u16 bits_per_pixel;
        u32 compression;
        u32 image_size;
    } info_header;
};

typedef struct styx_bmp_file styx_bmp_file;
struct styx_bmp_file
{
    styx_bmp_file_header *header;
    
    u32 pixel_cnt;
    u8 *bmp_data;
};

styx_function styx_bmp_file styx2d_load_bmp(memory_arena *allocator, str8 path);

#endif //STYX2D_PARSER_H
