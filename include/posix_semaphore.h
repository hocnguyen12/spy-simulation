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
#ifndef POSIX_SEMAPHORE_H
#define POSIX_SEMAPHORE_H

/**
 * \file posix_semaphore.h
 * \brief A simple program used to implement a POSIX semaphore and read data from the shared memory
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <sys/sem.h>
#include <semaphore.h>

#include "memory.h"

typedef sem_t semaphore_t;
typedef struct memory_s memory_t;

/**
 * Handles a fatal error. It displays a message, then exits.
 */
void handle_fatal_error(const char *message);

/**
 * Creates a POSIX semaphore and returns it.
 * \param name The name of the semaphore on the Unix system.
 * \return A pointer on the created POSIX semaphore.
 */
semaphore_t *open_semaphore(char *name);

/**
 * Destroys the specifier POSIX semaphore.
 * \param sem The identifier of the semaphore to destroy
 */
void destroy_semaphore(semaphore_t *sem, char *name);

/**
 * Performs a P() operation ("wait") on a semaphore.
 * \param sem Pointer on the semaphore.
 */
void P(semaphore_t *sem);

/**
 * Performs a V() operation ("signal") on a semaphore.
 * \param sem Pointer on the semaphore.
 */
void V(semaphore_t *sem);

/**
 * Returns a pointer to the simulation memory structure in the shared memory
 */
memory_t* get_data();

#endif /* POSIX_SEMAPHORE_H */