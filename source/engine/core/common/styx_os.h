/* date = April 8th 2022 4:49 pm */

#ifndef STYX_OS_H
#define STYX_OS_H

#define RESERVE_MEM(name) void *name(u64 size)
typedef RESERVE_MEM(StyxReserveMem);
#define FREE_MEM(name) void name(void *mem, u64 size)
typedef FREE_MEM(StyxFreeMem);
#define MSG_BOX(name) void name(char *title, char *msg)
typedef MSG_BOX(StyxMessageBox);

// Do some va_args magic and make OutputDebugStringA better than it
// originally was.
#define DEBUG_PRINT(name) void name(char *str)
typedef DEBUG_PRINT(StyxDebugPrint);
#define READ_ENTIRE_FILE(name) void *name(memory_arena *arena, str8 path)
typedef READ_ENTIRE_FILE(StyxReadEntireFile);

// Bruh, this violates the naming conventions of the project. Fix it. 
//
// This is a really nice v-table interface and all, and it really helps when 
// the developer is working on the game layer, but it complicates development
// on the platform later because of unnecessary abstractions.
// Is it then better to simply use function calls instead of using a v-table?
typedef struct StyxOS StyxOS;
struct StyxOS
{
    StyxReserveMem *reserve_mem;
    StyxFreeMem *free_mem;
    StyxMessageBox *msg_box;
    StyxDebugPrint *debug_print;
    StyxReadEntireFile *read_entire_file;
};

// How to give the base modules access to the StyxOS calls.
styx_global StyxOS *styx_os;

#define styxos_message_box styx_os->msg_box
#define styxos_reserve_mem styx_os->reserve_mem
#define styxos_free_mem styx_os->free_mem
#define styxos_read_entire_file styx_os->read_entire_file

#endif //PLATFORM_H
