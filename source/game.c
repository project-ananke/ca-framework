#include "base/styx_base_inc.c"
#include "core/common/styx_common_inc.c"

#include "core/styxa/styxa.c"
#include "core/styx2d/styx2d.c"

// TODO(sir->w): Refactor codebase for snake_case types.
MODULE_LOAD(styx_load)
{
    styx_module_init(TCTX_ALLOCATOR_SIZE, TCTX_TEMP_ALLOCATOR_SIZE);
    styx_tctx *tctx = get_tctx();
    
    // We gonna load in a .wav file because we are pros.
    /*styx_wav_file wav_file = styxa_load_wav(tctx->allocator, str8_lit("../assets/flowing-water.wav"));
    wav_file = (styx_wav_file){0};
    
    styx_png_file png_file = styx2d_load_png(tctx->allocator, str8_lit("../assets/woman_warrior.png"));
    png_file = (styx_png_file){0};*/
    
    /*styx_bmp_file bmp_file = styx2d_load_bmp(tctx->allocator, str8_lit("../assets/sample.bmp"));
    bmp_file = (styx_bmp_file){0};*/
}

MODULE_UNLOAD(styx_unload)
{
    
}

MODULE_LOOP(styx_update)
{
    styx_tctx *tctx = get_tctx();
    
    // NOTE(sir->w): For some reason, the allocator pointer becomes NULL.
    //arena_reset(tctx->temp_allocator);
    
    styx_push_clear(renderer, vec3c_lit(51, 77, 77));
    styx_push_triangle(renderer,
                       vec3f_lit(-0.5f, 0.5f, 0.0f),
                       vec3f_lit(0.5f, -0.5f, 0.0f),
                       vec3f_lit(-0.5f, -0.5f, 0.0f),
                       vec3c_lit(255, 255, 255));
}
