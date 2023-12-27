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

/**
 * \file spy_simulation.h
 *
 * \brief prototymes of function used by spy_simulation.
 */


#include "memory.h"

/**
 * \brief Creates the shared memory and truncates the correct size.
 */
memory_t *create_shared_memory();

/**
 * \brief Initializes the map of the city and the list of coordinates
 * of important locations (critical companies, supermarkets, city_hall and 
 * residential buildings).
 *
 * \param map a pointer to the map in the memory structure.
 * \param memory a pointer to the shared memory structure.
 */
void initialize_map(map_t *map, memory_t * mem);

/**
 * \brief Initializes the memory.
 *
 * \param memory a pointer to the shared memory structure.
 */
void initialize_memory(memory_t * memory);

/**
 * \brief Handler function called when spy_simulation receives a end of turn
 * signal (SIGTTIN).
 *
 */
void next_turn(int sig, siginfo_t * siginfo, void * context);

/**
 * \brief Handler function called when spy_simulation receives a end of simulation
 * signal (SIGTERM).
 *
 */
void end_simulation(int sig);

/**
 * \brief Function that initializes sigaction structures and waits for signals. 
 *
 */
void manage_spy_simulation();

/**
 * \brief Function that forks and executes ./bin/timer
 * The clone process calls manage_citizen_manager()
 *
 */
void manage_timer();

/**
 * \brief Function that forks and executes ./bin/citize_manager
 * The clone process calls manage_enemy_spy_network()
 *
 */
void manage_citizen_manager();

/**
 * \brief Function that forks and executes ./bin/enemy_spy_network
 * The clone process calls manage_counter_intelligence()
 *
 */
void manage_enemy_spy_network();

/**
 * \brief Function that forks and executes ./bin/counter_intelligence
 * The clone process calls manage_enemy_country()
 *
 */
void manage_counter_intelligence();

/**
 * \brief Function that forks and executes ./bin/enemy_country
 * The clone process calls manage_monitor()
 *
 */
void manage_enemy_country();

/**
 * \brief Function that executes ./bin/monitor
 *
 */
void manage_monitor();

/**
 * \brief Function that starts the simulation
 *
 */
void start_simulation();

#endif /* SPY_SIMULATION_H */