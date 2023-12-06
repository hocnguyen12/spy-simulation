#ifndef COMMON_H
#define COMMON_H
#include "memory.h"

void handle_fatal_error(const char *message);
memory_t *create_shared_memory();

#endif