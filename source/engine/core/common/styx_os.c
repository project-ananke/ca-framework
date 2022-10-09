#include "styx_os.h"

// Remove the layers of abstraction and simplify interface.
// These wrappers should be unnecessary.
styx_inline void
styx_message_box(char *title, char *message)
{
    styx_os->msg_box(title, message);
}

styx_inline void *
styx_reserve_mem(u64 size)
{
    return styx_os->reserve_mem(size);
}

styx_inline void
styx_free_mem(void *mem, u64 size)
{
    styx_os->free_mem(mem, size);
}
