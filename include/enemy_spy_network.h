/*
* ENSICAEN
* 6 Boulevard Maréchal Juin
* F-14050 Caen Cedex
*
* Projet : A LICENSE TO KILL
* Copyright (C) Cécile LU (cecile.lu (at) ecole.ensicaen.fr)
*/

#ifndef ENEMY_SPY_NETWORK_H
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

#include "common.h"
#include "posix_semaphore.h"
#include "memory.h"

/**
* \file enemy_spy_network.h
* 
* \brief Constants and prototypes of enemy_spy_network
*/

/**
 * \brief Function to define spy and initialize their attributs and home locations.
 * 
 * \param spys Pointer to an array of spy structures.
 * \param memory Pointer to the shared memory structure.
 */
void define_spy(spy_t *spy, memory_t * memory);

void switch_routine(int sig);

/**
 * \brief Routine for a spy thread. Executed everytime there is an signal.
 * 
 * \param spy Pointer to the spy structure.
 * \param memory Pointer to the shared memory structure.
 * \return Returns NULL.
 */
void * spy_routine();

void update_threads(int sig);

void end_simulation(int sig);

void wait_for_signal(pthread_t * threads);

/**
 * \brief Function to create threads for each spy.
 * 
 * \param spys Pointer to an array of spy structures.
 * \return Array of pthread_t representing the threads for each spy.
 */
pthread_t * spy_thread(memory_t * memory);

/**
 * \brief Function to update a spy state based on the current simulation time.
 * 
 * \param spy Pointer to the spy structure.
 * \param memory Pointer to the shared memory structure.
 */
void update_spy(spy_t * spy, memory_t * memory);

void update_case_officer(spy_t *officer, memory_t *memory);

void randomize_time(spy_t *officer, memory_t * memory);

void get_message(spy_t *officer, memory_t * memory);

/**
 * \brief Function of repering company to steal for a spy
 * 
 * \param spy Pointer to the spy structure.
 * \param memory Pointer to the shared memory structure.
 */
coordinates_t search_for_company_to_steal(spy_t *spy, memory_t * memory);

void steal_information(spy_t * spy, memory_t * memory);

void send_message(spy_t * spy, memory_t * memory);

void spot_company(spy_t * spy, memory_t * memory);

/**
 * \brief Function to move a spy towards their home. 
 * 
 * \param spy Pointer to the spy structure.
 */
void go_back_home(spy_t * spy, memory_t * memory);

/**
 * \brief Function to move a spy towards the closest supermarket.
 * 
 * \param spy Pointer to the spy structure.
 * \param closest Coordinates of the closest supermarket.
 */
void go_to_supermarket(spy_t * spy, coordinates_t closest, memory_t * memory);


/**
 * \brief Function to simulate a spy shopping.
 * 
 * \param spy Pointer to the spy structure.
 * \param memory Pointer to the shared memory structure.
 */
void do_some_shopping(spy_t * spy, memory_t *memory);

/**
 * \brief Function to simulate a spy resting at home.
 * 
 * \param spy Pointer to the spy structure.
 * \param memory Pointer to the shared memory structure.
 */
void rest_at_home(spy_t *spy, memory_t *memory);

void go_to_mailbox(spy_t * spy, memory_t * memory);

/**
 * \brief Calculate the manhattan distance between two points.
 * 
 * \param c1 Column of the first point.
 * \param r1 Row of the first point.
 * \param c2 Column of the second point.
 * \param r2 Row of the second point.
 * \return The manhattan distance between the two points.
 */
int dist(int c1, int r1, int c2, int r2);

/**
 * \brief Move a spy to the specified destination.
 * 
 * \param spy Pointer to the spy structure.
 * \param destination_col Destination column.
 * \param destination_row Destination row.
 */
void spy_goto(spy_t * spy, int destination_col, int destination_row);

/**
 * \brief Function to find the closest supermarket to the given coordinates.
 * 
 * \param current_col Current column of the spy.
 * \param current_row Current row of the spy.
 * \param memory Pointer to the shared memory structure.
 * \return Coordinates of the closest supermarket.
 */
coordinates_t find_closest_supermarket(int current_col, int current_row, memory_t *memory);

void send_message_to_enemy_country(spy_t * spy, memory_t * memory);

void handle_encounter_with_counter_intelligence(spy_t * spy, memory_t * memory);

#endif /* ENEMY_SPY_NETWORK_H */
