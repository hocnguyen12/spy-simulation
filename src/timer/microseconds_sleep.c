/*
 * Unix System Programming Examples / Exemplier de programmation système Unix
 *
 * Copyright (C) 1995-2023 Alain Lebret <alain.lebret [at] ensicaen [dot] fr>
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

#include "timer.h"

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

void start_simulation_timer(double turn_duration, memory_t *memory)
{
	int spy_simulation_pid, turn;
	turn = 0;

	printf("start_simulation_timer() called...\n");

	semaphore_t *sem;
    sem = open_semaphore("/spy_semaphore");
    P(sem);
	memory = get_data();
   	spy_simulation_pid = memory->pids[0];
    V(sem);

	printf("Starting to count...\n");

	for (;;) {
		printf("TURN : %d\n", turn);
		if (turn == 5) {
			/* END SIMULATION */
			printf("timer process sending SIGUSR2 to spy_sim...\n");
			if (kill(spy_simulation_pid, SIGUSR2) == -1) {
				perror("kill()");
				exit(EXIT_FAILURE);
			}
			break;
		}

		/* CHECK FOR END OF SIMULATION */
		sem = open_semaphore("/spy_semaphore");
		P(sem);
		memory = get_data();
		if (memory->simulation_has_ended) {
			printf("timer process sending SIGUSR2 to spy_sim...\n");
			if (kill(spy_simulation_pid, SIGUSR2) == -1) {
				perror("kill()");
				exit(EXIT_FAILURE);
			}
			break;
		}
		V(sem);

		/* INDICATE END OF TURN */
		printf("timer process sending SIGUSR1 to spy_sim... pid = %d\n", spy_simulation_pid	);
		if (kill(spy_simulation_pid, SIGUSR1) == -1) {
			perror("kill()");
			exit(EXIT_FAILURE);
    	}
		turn++;
		us_sleep(turn_duration);
	}
}