/* date = April 8th 2022 8:21 pm */

#ifndef ASSERT_H
#define ASSERT_H

#define ASSERT_BUF_SIZE 512
styx_global char _assert_msg_buf[ASSERT_BUF_SIZE];

styx_function void _assert_hard_fail(char *cond, char *file, int line, char *func);
styx_function void _assert_hard_msg_fail(char *cond, char *file, int line, char *func, const char *msg);
styx_function void _assert_soft_fail(char *cond, char *file, int line, char *func);
styx_function void _assert_soft_msg_fail(char *cond, char *file, int line, char *func, const char *msg);

// Actually, these functions should be static because a large number of
// inline functions throughout the code would drastically and adversely affect
// compilation time. 
styx_function void
_assert_hard_fail(char *cond, char *file, int line, char *func)
{
	snprintf(_assert_msg_buf, ASSERT_BUF_SIZE, 
             "Assertion failed at %s:%d in %s: %s\n",
             file, line, func, cond);
    styx_message_box("Hard Assertion Failure", _assert_msg_buf);
	crash_program();
}

styx_function void
_assert_hard_msg_fail(char *cond, char *file, int line, char *func, const char *msg)
{
	snprintf(_assert_msg_buf, ASSERT_BUF_SIZE, 
             "Assertion failed at %s:%d in %s: %s\nError: %s\n",
             file, line, func, cond, msg);
    styx_message_box("Hard Assertion Failure", _assert_msg_buf);
	crash_program();
}

styx_function void
_assert_soft_fail(char *cond, char *file, int line, char *func)
{
    snprintf(_assert_msg_buf, ASSERT_BUF_SIZE, 
             "Assertion failed at %s:%d in %s: %s\n",
             file, line, func, cond);
    styx_message_box("Soft Assertion Failure", _assert_msg_buf);
}

styx_function void
_assert_soft_msg_fail(char *cond, char *file, int line, char *func, const char *msg)
{
    snprintf(_assert_msg_buf, ASSERT_BUF_SIZE, 
             "Assertion failed at %s:%d in %s: %s\nError: %s\n",
             file, line, func, cond, msg);
    styx_message_box("Soft Assertion Failure", _assert_msg_buf);
}

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
