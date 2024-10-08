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

#include "memory.h"
#include "monitor.h"
#include "spy_simulation.h"
#include "posix_semaphore.h"

/**
 * \file main_spy_simulation.c
 *
 * \brief Creates 7 processes that each execute a program of the simulation.
 *
 * The function manage_spy_siulation() represents the role of spy_simulation,
 * which is to initialize the shared memory and all the city data.
 */
int main(int argc, char **argv)
{
    memory_t *memory;
    memory = create_shared_memory(); 
    initialize_memory(memory);

    /* FORK TO EXECUTE ALL THE PROGRAMS */
    start_simulation();
    
    exit(EXIT_SUCCESS);
}