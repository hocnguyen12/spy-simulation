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
#ifndef SPY_SIMULATION_H
#define SPY_SIMULATION_H

#include "memory.h"

memory_t *create_shared_memory();

void initialize_memory(memory_t * memory);

void next_turn(int sig, siginfo_t * siginfo, void * context);

void end_simulation(int sig);

void manage_spy_simulation();

void manage_timer();

void manage_citizen_manager();

void manage_enemy_spy_network();

void manage_counter_intelligence();

void manage_enemy_country();

void manage_monitor();

void start_simulation();

#endif /* SPY_SIMULATION_H */