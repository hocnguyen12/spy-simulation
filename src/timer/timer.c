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
 * \file timer.c
 *
 * \brief Sends a posix signal to the spy_simulation at the end of a turn.
 */

#include "timer.h"

#define TURN_NUMBER 2016

/**
 * @file microseconds_sleep.c
 *
 * A simple program to provide microsecond sleeping.
 *
 * @author Alain Lebret
 * @version	1.0
 * @date 2023-07-13
 */

/**
 * Sleeps for a number of microseconds.
 */
void us_sleep(int nb_usec)
{
	struct timeval waiting;
	
	waiting.tv_sec = nb_usec / 1000000; 
	waiting.tv_usec = nb_usec % 1000000; 
	select(0, NULL, NULL, NULL, &waiting);
}

double get_elapsed_time(struct timeval start, struct timeval end) {
    return (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_usec - start.tv_usec) / 1000000.0;
}

void start_simulation_timer(double turn_duration)
{
    memory_t * memory;
	int spy_simulation_pid, turn;
	turn = 0;

    /* reading spy_simulation pid from shm */
	semaphore_t *sem;
    sem = open_semaphore("/spy_semaphore");
    P(sem);
	memory = get_data();
   	spy_simulation_pid = memory->pids[0];
    V(sem);

	for (;;) {
		if (turn == TURN_NUMBER) {
			/* END SIMULATION */
			if (kill(spy_simulation_pid, SIGTERM) == -1) {
				perror("kill()");
				exit(EXIT_FAILURE);
			}

			if (munmap(memory, sizeof(memory)) == -1) {
        		perror("munmap");
    		}
			break;
		}

		/* CHECK FOR END OF SIMULATION */
		sem = open_semaphore("/spy_semaphore");
		P(sem);
		memory = get_data();
		if (memory->simulation_has_ended) {
			//printf("timer process sending SIGUSR2 to spy_sim...\n");
			if (kill(spy_simulation_pid, SIGTERM) == -1) {
				perror("kill()");
				exit(EXIT_FAILURE);
			}
			if (munmap(memory, sizeof(memory)) == -1) {
        		perror("munmap");
    		}
			break;
		}
		V(sem);

		/* INDICATE END OF TURN */
		if (kill(spy_simulation_pid, SIGTTIN) == -1) {
			perror("kill()");
			exit(EXIT_FAILURE);
    	}
		turn++;
		us_sleep(turn_duration);
	}
}