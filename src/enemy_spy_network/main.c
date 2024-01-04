/*
 * The License to Kill Project
 *
 * Copyright (C) 2021 Alain Lebret <alain.lebret [at] ensicaen [dot] fr>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include "enemy_spy_network.h"

int main(void) 
{
    //printf("ENEMY SPY NETWORK : pid = %d\n", getpid());
    memory_t* memory;
    semaphore_t* sem;
    sem = open_semaphore("/spy_semaphore");
    P(sem);
    memory = get_data();
    define_spy(memory->spies, memory);
    send_message(&memory->spies[0],memory);
    V(sem);
    pthread_t * threads = spy_thread(memory);
    wait_for_signal(threads);
}