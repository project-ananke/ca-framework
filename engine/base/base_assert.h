/* date = April 8th 2022 8:21 pm */

#ifndef ASSERT_H
#define ASSERT_H

void _assert_hard_fail(char *cond, char *file, int line, char *func);
void _assert_hard_msg_fail(char *cond, char *file, int line, char *func, const char *msg);
void _assert_soft_fail(char *cond, char *file, int line, char *func);
void _assert_soft_msg_fail(char *cond, char *file, int line, char *func, const char *msg);

#define assert_hard(cond) \
    do { if (!(cond)) _assert_hard_fail(#cond, __FILE__, __LINE__, __func__); } while (0)
#define assert_hard_msg(cond, msg) \
    do { if (!(cond)) _assert_hard_msg_fail(#cond, __FILE__, __LINE__, __func__, msg); } while(0)

#define assert_soft(cond) \
    do { if (!(cond)) _assert_soft_fail(#cond, __FILE__, __LINE__, __func__); } while (0)
#define assert_soft_msg(cond, msg) \
    do { if (!(cond)) _assert_soft_msg_fail(#cond, __FILE__, __LINE__, __func__, msg); } while(0)

#ifndef assert
#define assert assert_hard
#endif

#ifndef assert_msg
#define assert_msg assert_hard_msg
#endif

#define assert_static(cond) \
    typedef char concat(static_assertion_, __LINE__)[(!!(cond)) * 2 - 1]

/////////////////////////////////////////////////
#if STYX_RELEASE
#undef assert_hard
#undef assert_hard_msg
#undef assert_soft
#undef assert_soft_msg

#define assert_hard(cond)
#define assert_hard_msg(cond, msg)
#define assert_soft(cond)
#define assert_soft_msg(cond, msg)
#endif

#endif //ASSERT_H
