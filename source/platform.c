// NOTE(sir->w): Seriously only works for 4coder internal cmd prompt.
// Replace with OutputDebugStringA with proper formatting for more general
// debugging purposes.

// Do we really want to pollute global namespace with typedefs?
// Would it be a better option to preface every struct declaration with
// "struct"?
#include "engine/styx_platform_inc.c"
#include "engine/core/styxa/styxa.c"

styx_function int 
styx_main(void)
{
	styx_window *window = styxplat_create_window(str8_lit("Project Styx"), 1280, 720);
	styx_module *game_mod = styxplat_load_module(str8_lit("game.dll"),
												 str8_lit("styx_load"), 
												 str8_lit("styx_unload"), 
												 str8_lit("styx_update"),
												 window);

	while (window->running) {
		if (styxplat_module_should_reload(game_mod)) {
			styxplat_reload_module(game_mod, window);
		}
		styxplat_process_messages(window);

		styxplat_module_update(game_mod, window);
		styxplat_update_window(window, TRUE);
	}

	styxplat_unload_module(game_mod, window);
	styxplat_destroy_window(window);

	return 0;
}
