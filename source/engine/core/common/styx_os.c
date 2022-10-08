#include "styx_os.h"

// Remove the layers of abstraction and simplify interface.
// These wrappers should be unnecessary.
inline void
styx_message_box(char *title, char *message)
{
    styx_os->msg_box(title, message);
}

inline void *
styx_reserve_mem(u64 size)
{
    return styx_os->reserve_mem(size);
}

inline void
styx_free_mem(void *mem)
{
    styx_os->free_mem(mem);
}