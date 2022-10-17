#ifndef STYXLUA_H
#define STYXLUA_H

typedef struct styx_settings styx_settings;
struct styx_settings
{
    int width, height;
};

extern lua_State *L;

void styx_init_lua();
void styx_quit_lua();

int styx_get_globalint(char *var);

styx_settings styx_load_settings(char *filename);

#endif
