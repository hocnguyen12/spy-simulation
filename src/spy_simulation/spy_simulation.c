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
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include "posix_semaphore.h"
#include "spy_simulation.h"
#include "memory.h"
#include "monitor.h"
#include "timer.h"

memory_t* create_shared_memory() 
{
    int shm_fd; 
    memory_t* ptr;

    shm_fd = shm_open("/spy_memory", O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        handle_fatal_error("shm_open()");
    }

    if (ftruncate(shm_fd, sizeof(memory_t)) == -1) {
        handle_fatal_error("ftruncate()");
    }

    ptr = mmap(0, sizeof(memory_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        handle_fatal_error("mmap()");
    }

    return ptr;
}

void initialize_memory(memory_t * memory) 
{    
    semaphore_t *sem;
    sem = open_semaphore("/spy_semaphore");
    P(sem);
    //memory->pids = malloc(7 * sizeof(int));
    memory->memory_has_changed = 0;
    memory->simulation_has_ended = 0;
    memory->current_turn = 0;
    // CREATE CITY INFOS



    
    V(sem);
    printf("Initialized memory...\n");
}

void next_turn(int sig, siginfo_t * siginfo, void * context) 
{
    memory_t * memory = (memory_t *)siginfo->si_value.sival_ptr;

    semaphore_t *sem;
    sem = open_semaphore("/spy_semaphore");
    P(sem);
    memory = get_data();
    printf("TURN %d\n", memory->current_turn);
    memory->current_turn++; 
    V(sem);
}

void end_simulation(int sig) 
{
    printf("SIMULATION ENDED...\n");
    /* CALL FUNCTION TO FREE MEMORY */
    exit(EXIT_SUCCESS);
}

void manage_spy_simulation() 
{

    printf("spy simulation process : %d\n", getpid());

    memory_t * memory;
    semaphore_t *sem;
    sem = open_semaphore("/spy_semaphore");
    P(sem);
    /* SPY SIMULATION PID */
    memory = get_data();
    memory->pids[0] = getpid();
    printf("Wrote spy pid in memory : %d\n", memory->pids[0]);
    V(sem);

    /* INTERCEPT KILL SIG FROM TIMER*/
    struct sigaction turn_signal, end_signal;

    memset(&turn_signal, 0, sizeof(turn_signal));
    turn_signal.sa_sigaction = &next_turn;
    turn_signal.sa_flags = SA_SIGINFO;
    if (sigaction(SIGUSR1, &turn_signal, NULL) < 0) {
        handle_fatal_error("Error using sigaction");
    }

    end_signal.sa_handler = &end_simulation;
    end_signal.sa_flags = 0;
    if (sigaction(SIGUSR2, &end_signal, NULL) < 0) {
        handle_fatal_error("Error using sigaction");
    }

    /* -------------------SIMULATION LOGIC-----------------------*/
    /* next_turn will be called on reception of SIGUSR1 */
    /* end_simulation will be called on reception of SIGUSR2 */     

    //wait(NULL);
    printf("spy simulation is waiting for signals...\n");


    while(1){}
}

void manage_timer()
{
    printf("timer process : %d\n", getpid());

    memory_t * memory;
    semaphore_t *sem;
    sem = open_semaphore("/spy_semaphore");
    P(sem);
    memory = get_data();
    /* TIMER PID */
    memory->pids[1] = getpid(); 
    V(sem);

    if (munmap(memory, sizeof(memory)) == -1) {
        perror("munmap");
    }

    /* CALLS MANAGE_CITIZEN_MANAGER*/
    pid_t pid = fork();
    if (pid == -1) {
        handle_fatal_error("Error [fork()]: ");
    } else if (pid == 0) {
        manage_citizen_manager();
    } else {
        /* EXEC TIMER */
        /* Arguments pour execvp(), "900000" donne 0.9 seconde "1000000" donne 1 seconde*/
        char *args[] = {"./bin/timer", "900000", NULL};  

        if (execvp("./bin/timer", args) == -1) {
            handle_fatal_error("Error [execvp(timer)]");
        }
    }
    
   /*
    char *args[] = {"./bin/timer", "900000", NULL};  
    if (execvp("./bin/timer", args) == -1) {
        handle_fatal_error("Error [execvp(timer)]");
    }*/
}

void manage_citizen_manager()
{
    printf("citizen_manager process : %d\n", getpid());

    memory_t * memory;
    semaphore_t *sem;
    sem = open_semaphore("/spy_semaphore");
    P(sem);
    memory = get_data();
    /* CITIZEN_MANAGER PID */
    memory->pids[2] = getpid(); 
    V(sem);

    if (munmap(memory, sizeof(memory)) == -1) {
        perror("munmap");
    }

    /* CALLS MANAGE_ENEMY_SPY_NETWOTK */
    pid_t pid = fork();
    if (pid == -1) {
        handle_fatal_error("Error [fork()]: ");
    } else if (pid == 0) {
        manage_enemy_spy_network();
    } else {
        /* EXEC CITIZEN_MANAGER */
        /*
        if (execl("./bin/citizen_manager") == -1) {
            handle_fatal_error("Error [execl()]");
        }
    */
        wait(NULL);
    }
}

void manage_enemy_spy_network()
{
    printf("enemy_spy_network process : %d\n", getpid());

    memory_t * memory;
    semaphore_t *sem;
    sem = open_semaphore("/spy_semaphore");
    P(sem);
    memory = get_data();
    /* ENEMY_SPY_NETWORK PID */
    memory->pids[3] = getpid(); 
    V(sem);

    if (munmap(memory, sizeof(memory)) == -1) {
        perror("munmap");
    }

    /* CALLS MANAGE_COUNTER_INTELLIGENCE */
    pid_t pid = fork();
    if (pid == -1) {
        handle_fatal_error("Error [fork()]: ");
    } else if (pid == 0) {
        manage_counter_intelligence();
    } else {
        /* EXEC ENEMY_SPY_NETWORK */
        /*
        if (execl("./bin/enemy_spy_network") == -1) {
            handle_fatal_error("Error [execl()]");
        }
    */
        wait(NULL);
    }
}

void manage_counter_intelligence()
{
    printf("counter_intelligence process : %d\n", getpid());

    memory_t * memory;
    semaphore_t *sem;
    sem = open_semaphore("/spy_semaphore");
    P(sem);
    memory = get_data();
    /* COUNTER_INTELLIGENCE PID */
    memory->pids[4] = getpid(); 
    V(sem);

    if (munmap(memory, sizeof(memory)) == -1) {
        perror("munmap");
    }

    /* CALLS MANAGE_ENEMY_COUTNRY */
    pid_t pid = fork();
    if (pid == -1) {
        handle_fatal_error("Error [fork()]: ");
    } else if (pid == 0) {
        manage_enemy_country();
    } else {
        /* EXEC COUNTER_INTELLIGENCE */
        /*
        if (execl("./bin/counter_intelligence") == -1) {
            handle_fatal_error("Error [execl()]");
        }
    */
        wait(NULL);
    }
}

void manage_enemy_country()
{
    printf("enemy_country process : %d\n", getpid());

    memory_t * memory;
    semaphore_t *sem;
    sem = open_semaphore("/spy_semaphore");
    P(sem);
    memory = get_data();
    /* ENEMY_COUNTRY PID */
    memory->pids[5] = getpid(); 
    V(sem);

    if (munmap(memory, sizeof(memory)) == -1) {
        perror("munmap");
    }

    /* CALLS MANAGE_MONITOR */
    pid_t pid = fork();
    if (pid == -1) {
        handle_fatal_error("Error [fork()]: ");
    } else if (pid == 0) {
        manage_monitor();
    } else {
        /* EXEC ENEMY_COUNTRY */
        /*
        if (execl("./bin/enemy_country") == -1) {
            handle_fatal_error("Error [execl()]");
        }
    */
        wait(NULL);
    }
}

void manage_monitor()
{
    printf("monitor process : %d\n", getpid());

    memory_t * memory;
    semaphore_t *sem;
    sem = open_semaphore("/spy_semaphore");
    P(sem);
    memory = get_data();
    /* MONITOR PID */
    memory->pids[6] = getpid(); 
    V(sem);

    if (munmap(memory, sizeof(memory)) == -1) {
        perror("munmap");
    }

    /* EXEC MONITOR */
    if (execvp("./bin/monitor", NULL) == -1) {
        handle_fatal_error("Error [execl(monitor)]");
    }
}

void start_simulation()
{
    pid_t pid = fork();
    if (pid == -1) {
        handle_fatal_error("Error [fork()]: ");
    } else if (pid == 0) {
        /* EXEC 6 OTHER PROGRAMS */
        manage_timer();
    } else {
        /* PROCESSUS PARENT SPY_SIMULATION */
        manage_spy_simulation();
    }
}