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
#include "posix_semaphore.h"

/**
 * \file posix_semaphore.c
 * \brief A simple program used to implement a POSIX semaphore and read data from the shared memory
 */

/**
 * Handles a fatal error. It displays a message, then exits.
 */
void handle_fatal_error(const char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

/**
 * Creates a POSIX semaphore and returns it.
 * \param name The name of the semaphore on the Unix system.
 * \return A pointer on the created POSIX semaphore.
 */
semaphore_t *open_semaphore(char *name)
{
    semaphore_t *sem = NULL;

    sem = sem_open(name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 0);
    sem_init(sem,1,1);
    if (sem == SEM_FAILED) {
        sem_unlink(name); /* Try to unlink it */
        handle_fatal_error("Error [sem_unlink()]: ");
    }
    return sem;
}

/**
 * Destroys the specifier POSIX semaphore.
 * \param sem The identifier of the semaphore to destroy
 */
void destroy_semaphore(semaphore_t *sem, char *name)
{
    int r = 0;

    r = sem_close(sem);
    if (r < 0) {
        handle_fatal_error("Error [sem_close()]: ");
    }
    r = sem_unlink(name);
    if (r < 0) {
        perror("Error [sem_unlink()]: ");
    }
}

/**
 * Performs a P() operation ("wait") on a semaphore.
 * \param sem Pointer on the semaphore.
 */
void P(semaphore_t *sem)
{
    int r = 0;
    
    r = sem_wait(sem);
    if (r < 0) {
        handle_fatal_error("Error [P()]: ");
    }
}

/**
 * Performs a V() operation ("signal") on a semaphore.
 * \param sem Pointer on the semaphore.
 */
void V(semaphore_t *sem)
{
    int r = 0;
    
    r = sem_post(sem);
    if (r < 0) {
        handle_fatal_error("Error [V()]: ");
    }
}

/**
 * Returns a pointer to the simulation memory structure in the shared memory
 */
memory_t* get_data(){
    int fd;
    memory_t*ptr=malloc(sizeof(memory_t));
    fd = shm_open("/spy_memory", O_RDWR, 0666);
    if(fd==-1){
        printf("error");
    }
    ptr = (memory_t*) mmap(NULL, sizeof(memory_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    return ptr;
}