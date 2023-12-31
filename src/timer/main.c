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

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include "timer.h"
#include "memory.h"
#include "posix_semaphore.h"

/**
 * \file main_timer.c
 *
 * \brief Indicates to spy_simuation the end of a turn. Sends an end signal after 2016 turns.
 * 
 * \param argv The real duration of a turn.
 */
int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: %s time_in_microseconds(in interval [100000, 1000000])\n", argv[0]);
        return EXIT_FAILURE;
    }

    double turn_duration = atof(argv[1]);

    /* ---------------------------------------------------------------------- */ 
    /* Test that prints the time elapsed in one turn.           */ 
    /*
    struct timeval begin_time, end_time;
    double duration;

    gettimeofday(&begin_time, NULL); 
    us_sleep(turn_duration);
    gettimeofday(&end_time, NULL); 

    duration = get_elapsed_time(begin_time, end_time);
    printf("Temps écoulé : %f secondes\n", duration);
    */
    /* ---------------------------------------------------------------------- */ 
    printf("Initializing simulation...\n");
    us_sleep(2000000);
    //printf("Starting simulation\n");
    start_simulation_timer(turn_duration);

    return EXIT_SUCCESS;
}