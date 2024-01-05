#ifndef ENEMY__H
#define ENEMY_SPY_NETWORK_H

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <error.h>
#include <errno.h>
#include <limits.h>
#include <mqueue.h>

#include "common.h"
#include "posix_semaphore.h"
#include "memory.h"

void receive_message_from_enemy_spy_network_and_update(memory_t *mem);


#endif /* ENEMY_COUNTRY_H */
