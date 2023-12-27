/*
* ENSICAEN
* 6 Boulevard Maréchal Juin
* F-14050 Caen Cedex
*
* Projet : A LICENSE TO KILL
* Copyright (C) Cécile LU (cecile.lu (at) ecole.ensicaen.fr)
*/
#ifndef CITIZEN_MANAGER_H
#define CITIZEN_MANAGER_H

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

#include "memory.h"
#include "posix_semaphore.h"

/**
* \file citizen_manager.h
* 
* \brief Constants and prototypes of citizen_manager
*/



/**
 * \brief Calculate the square distance between two points.
 * 
 * \param c1 Column of the first point.
 * \param r1 Row of the first point.
 * \param c2 Column of the second point.
 * \param r2 Row of the second point.
 * \return The square distance between the two points.
 */
int dist(int c1, int r1, int c2, int r2);

/**
 * \brief Function to update a city hall employee's or a company employee state based on the current simulation time.
 * 
 * \param citizen Pointer to the citizen structure.
 * \param memory Pointer to the shared memory structure.
 */
void update_employee(citizen_t * citizen, memory_t *memory);


/**
 * \brief Function to update a supermarket employee's state based on the current simulation time.
 * 
 * \param citizen Pointer to the citizen structure.
 * \param memory Pointer to the shared memory structure.
 */
void update_supermarket_employee(citizen_t * citizen, memory_t *memory);


/**
 * \brief Routine for a citizen thread. Executed everytime there is an signal.
 * 
 * \param citizen Pointer to the citizen structure.
 * \param memory Pointer to the shared memory structure.
 * \return Returns NULL.
 */
void *citizen_routine();

/**
 * \brief Switches routine based on the citizen's role.
 * 
 * \param citizen Pointer to the citizen structure.
 * \param memory Pointer to the shared memory structure.
 */
void switch_routine();



/**
 * \brief Function to define citizens and assign them roles, work, and home locations.
 * 
 * \param citizens Pointer to an array of citizen structures.
 * \param memory Pointer to the shared memory structure.
 */
void define_citizen(citizen_t * citizens, memory_t *memory);


/**
 * \brief Function to create threads for each citizen.
 * 
 * \param citizens Pointer to an array of citizen structures.
 * \return Array of pthread_t representing the threads for each citizen.
 */
pthread_t * citizen_thread();

/**
 * \brief Function to free resources used by citizen threads.
 * 
 * \param threads Array of pthread_t representing the threads for each citizen.
 */
void free_thread_resources(pthread_t *threads);

/**
 * \brief Function to find the closest supermarket to the given coordinates.
 * 
 * \param current_col Current column of the citizen.
 * \param current_row Current row of the citizen.
 * \param memory Pointer to the shared memory structure.
 * \return Coordinates of the closest supermarket.
 */
coordinates_t find_closest_supermarket(int current_col, int current_row, memory_t *memory);

/**
 * \brief Function to move a citizen towards their home. 
 * 
 * \param citizen Pointer to the citizen structure.
 */
void go_back_home(citizen_t * citizen, memory_t * memory);

/**
 * \brief Function to move a citizen towards their workplace.
 * 
 * \param citizen Pointer to the citizen structure.
 */
void go_to_work(citizen_t * citizen, memory_t * memory);

/**
 * \brief Function to move a citizen towards the closest supermarket.
 * 
 * \param citizen Pointer to the citizen structure.
 * \param closest Coordinates of the closest supermarket.
 */
void go_to_supermarket(citizen_t * citizen, coordinates_t closest, memory_t * memory);

/**
 * \brief Function to simulate a citizen working.
 * 
 * \param citizen Pointer to the citizen structure.
 * \param memory Pointer to the shared memory structure.
 */
void work(citizen_t *citizen, memory_t * memory);

/**
 * Function to simulate a citizen shopping.
 * \param citizen Pointer to the citizen structure.
 * \param memory Pointer to the shared memory structure.
 */
void do_some_shopping(citizen_t * citizen, memory_t *memory);

/**
 * \brief Function to simulate a citizen resting at home.
 * 
 * \param citizen Pointer to the citizen structure.
 * \param memory Pointer to the shared memory structure.
 */
void rest_at_home(citizen_t *citizen, memory_t *memory);

/**
 * \brief Move a citizen to the specified destination.
 * 
 * \param citizen Pointer to the citizen structure.
 * \param destination_col Destination column.
 * \param destination_row Destination row.
 */
void citizen_goto(citizen_t *citizen, int destination_col, int destination_row);

#endif /* CITIZEN_MANAGER_H*/
