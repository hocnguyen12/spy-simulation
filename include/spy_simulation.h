#ifndef COMMON_H
#define COMMON_H

#include "memory.h"

memory_t *create_shared_memory();

void exec_timer(void);

void manage_timer(memory_t * memory);

void manage_spy_simulation(memory_t * memory);


#endif