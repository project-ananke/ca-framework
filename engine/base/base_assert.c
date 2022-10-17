#include <stdio.h>

#include "base_ctxcrack.h"
#include "base_util.h"
#include "base_assert.h"

#define ASSERT_BUF_SIZE 1024
char _assert_msg_buf[ASSERT_BUF_SIZE];

void _assert_hard_fail(char *cond, char *file, int line, char *func)
{
	snprintf(_assert_msg_buf, ASSERT_BUF_SIZE, 
             "Assertion failed at %s:%d in %s: %s\n",
             file, line, func, cond);
    styx_message_box("Hard Assertion Failure", _assert_msg_buf);
	crash_program();
}

void _assert_hard_msg_fail(char *cond, char *file, int line, char *func, const char *msg)
{
	snprintf(_assert_msg_buf, ASSERT_BUF_SIZE, 
             "Assertion failed at %s:%d in %s: %s\nError: %s\n",
             file, line, func, cond, msg);
    styx_message_box("Hard Assertion Failure", _assert_msg_buf);
	crash_program();
}

void _assert_soft_fail(char *cond, char *file, int line, char *func)
{
    snprintf(_assert_msg_buf, ASSERT_BUF_SIZE, 
             "Assertion failed at %s:%d in %s: %s\n",
             file, line, func, cond);
    styx_message_box("Soft Assertion Failure", _assert_msg_buf);
}

void _assert_soft_msg_fail(char *cond, char *file, int line, char *func, const char *msg)
{
    snprintf(_assert_msg_buf, ASSERT_BUF_SIZE, 
             "Assertion failed at %s:%d in %s: %s\nError: %s\n",
             file, line, func, cond, msg);
    styx_message_box("Soft Assertion Failure", _assert_msg_buf);
}
