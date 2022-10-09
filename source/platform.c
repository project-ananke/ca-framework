// NOTE(sir->w): Seriously only works for 4coder internal cmd prompt.
// Replace with OutputDebugStringA with proper formatting for more general
// debugging purposes.

// Do we really want to pollute global namespace with typedefs?
// Would it be a better option to preface every struct declaration with
// "struct"?
#include "engine/styx_platform_inc.c"

styx_function int 
styx_main(void)
{
	styx_window *window = styxplat_create_window(str8_lit("Project Styx"), 1280, 720);

	while (window->running) {
		styxplat_process_messages(window);
	}

	styxplat_destroy_window(window);
	return 0;
}
