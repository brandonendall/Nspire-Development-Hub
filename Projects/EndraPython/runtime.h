#pragma once
#include <stddef.h>
/* Set stack_top to a live marker in the outer application event loop. */
void runtime_start(void *stack_top);
void runtime_end(void);
int runtime_exec(const char *name, const char *src, int interactive);
void console_write(const char *data, size_t size);
int cancel_pressed(void);

int runtime_error_line(void);

void platform_wait_input(void);
int runtime_catalog(const char *module);
