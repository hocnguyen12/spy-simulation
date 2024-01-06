/*
 * The License to Kill Project
 *
 * Copyright (C) 1995-2022 Alain Lebret <alain.lebret [at] ensicaen [dot] fr>
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
#ifndef COUNTER_INTELLIGENCE_H
#define COUNTER_INTELLIGENCE_H

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

#include "common.h"
#include "posix_semaphore.h"
#include "memory.h"

void initialize_counter_intelligence(counter_int_t * counter, memory_t * memory);

void counter_routine(int sig);

void begin_counter_intelligence_routine();

void update_counter(counter_int_t * counter, memory_t * memory);

void find_suspiscious_act(counter_int_t * counter, memory_t *memory);

void follow_counter(counter_int_t *counter, memory_t *memory);

void find_mailbox(counter_int_t * counter, memory_t * memory);

void altercation_with_spy(counter_int_t * counter, memory_t * memory, spy_t* spy);

void fight_with_spy(counter_int_t* counter, memory_t * memory, spy_t* spy);

void get_message(counter_int_t* counter, memory_t* memory);

void go_back_home(counter_int_t * counter, memory_t * memory);

void rest_at_home(counter_int_t *counter, memory_t *memory);

void go_to_mailbox(counter_int_t * counter, memory_t * memory);

int dist(int r1, int c1, int r2, int c2);

void counter_goto(counter_int_t * counter, int destination_col, int destination_row);

void go_to_company(counter_int_t * counter, memory_t * memory);

#endif /* COUNTER_INTELLIGENCE_H */