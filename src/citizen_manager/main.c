/*
* ENSICAEN
* 6 Boulevard Maréchal Juin
* F-14050 Caen Cedex
* Projet : A LICENSE TO KILL
*
* Copyright (C) Cécile LU (cecile.lu (at) ecole.ensicaen.fr)
*/

/**
 * \file citizen_manager.c
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "citizen_manager.h"

int main(void){
    //printf("CITIZEN MANAGER : pid = %d\n", getpid());
    memory_t* memory;
    semaphore_t* sem;
    sem = open_semaphore("/spy_semaphore");
    P(sem);
    memory = get_data();
    define_citizen(memory->citizens, memory);
    V(sem);

    pthread_t * threads = citizen_thread(memory);
    wait_for_signal(threads);
}
