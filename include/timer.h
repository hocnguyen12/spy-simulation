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
 
#ifndef TIMER_H
#define TIMER_H

/**
 * \file memory.h
 *
 * Defines functions used to manage the timer and indicate the next turn or the
 * end of the simulatiojn.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include "memory.h"
#include "posix_semaphore.h"

#define TURN_NUMBER 2016


/**
 * \brief Sleeps for a number of microseconds.
 *
 * \param nb_usec The number of microseconds of a real turn.
 */
void us_sleep(int nb_usec);

double get_elapsed_time(struct timeval start, struct timeval end);

/**
 * \brief Sends a SIGTTIN posix signal to spy_simulation at the end of each turn.
 * Sends a SIGTERM posix signal to spy_simulation in case of simulation end.
 *
 * \param turn_duration The number of microseconds of a real turn.
 */
void start_simulation_timer(double turn_duration);

#endif /* TIMER_H */