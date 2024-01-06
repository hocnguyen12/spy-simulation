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
#include <pthread.h>

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

void initialize_companies(memory_t * mem)
{
    int i;

    char * crucial_msg = caesar_cipher("This is a crucial message", CIPHER_KEY);
    char * strong_msg = caesar_cipher("This is a strong message", CIPHER_KEY);
    char * medium_msg = caesar_cipher("This is a medium message", CIPHER_KEY);
    char * low_msg = caesar_cipher("This is a low message", CIPHER_KEY);
    char * very_low_msg = caesar_cipher("This is a very low message", CIPHER_KEY);

    mem->companies[0].row = 3; 
    mem->companies[0].col = 5;
    mem->companies[0].size = BIG;
    mem->companies[0].nb_employee = 31;
    mem->companies[0].nb_messages = 69;
    mem->companies[0].index = 0;
    mem->companies[0].nb_of_people = 0;
    for (i = 0; i < 2; i++) {
        mem->companies[0].messages[i].importance = 10;
        strcpy(mem->companies[0].messages[i].content, crucial_msg);
    }
    for (i = 2; i < 7; i++) {
        mem->companies[0].messages[i].importance = 9;
        strcpy(mem->companies[0].messages[i].content, strong_msg);
    }
    for (i = 7; i < 21; i++) {
        mem->companies[0].messages[i].importance = 6;
        strcpy(mem->companies[0].messages[i].content, medium_msg);
    }
    for (i = 21; i < 41; i++) {
        mem->companies[0].messages[i].importance = 3;
        strcpy(mem->companies[0].messages[i].content, low_msg);
    }
    for (i = 41; i < 71; i++) {
        mem->companies[0].messages[i].importance = 2;
        strcpy(mem->companies[0].messages[i].content, very_low_msg);
    }

    mem->companies[1].row = 0;
    mem->companies[1].col = 1;
    mem->companies[1].size = SMALL;
    mem->companies[1].nb_employee = 12;
    mem->companies[1].nb_messages = 49;
    mem->companies[1].index = 1;
    mem->companies[1].nb_of_people = 0;
    for (i = 0; i < 1; i++) {
        mem->companies[1].messages[i].importance = 9;
        strcpy(mem->companies[1].messages[i].content, strong_msg);
    }
    for (i = 1; i < 8; i++) {
        mem->companies[1].messages[i].importance = 6;
        strcpy(mem->companies[1].messages[i].content, medium_msg);
    }
    for (i = 8; i < 19; i++) {
        mem->companies[1].messages[i].importance = 3;
        strcpy(mem->companies[1].messages[i].content, low_msg);    
        }
    for (i = 19; i < 49; i++) {
        mem->companies[1].messages[i].importance = 2;
        strcpy(mem->companies[1].messages[i].content, very_low_msg);
    }

    mem->companies[2].row = 5;
    mem->companies[2].col = 6;
    mem->companies[2].size = VERY_SMALL;
    mem->companies[2].nb_employee = 5;
    mem->companies[2].nb_messages = 36;
    mem->companies[2].index = 2;
    mem->companies[2].nb_of_people = 0;
    for (i = 0; i < 1; i++) {
        mem->companies[2].messages[i].importance = 9;
        strcpy(mem->companies[2].messages[i].content, strong_msg);
    }
    for (i = 1; i < 8; i++) {
        mem->companies[2].messages[i].importance = 6;
        strcpy(mem->companies[2].messages[i].content, medium_msg);
    }
    for (i = 8; i < 19; i++) {
        mem->companies[2].messages[i].importance = 3;
        strcpy(mem->companies[2].messages[i].content, low_msg);
    }
    for (i = 19; i < 36; i++) {
        mem->companies[2].messages[i].importance = 2;
        strcpy(mem->companies[2].messages[i].content, very_low_msg);
    }

    mem->companies[3].row = 4;
    mem->companies[3].col = 0;
    mem->companies[3].size = MEDIUM;
    mem->companies[3].nb_employee = 21;
    mem->companies[3].nb_messages = 63;
    mem->companies[3].index = 3;
    mem->companies[3].nb_of_people = 0;
    for (i = 0; i < 1; i++) {
        mem->companies[3].messages[i].importance = 9;
        strcpy(mem->companies[3].messages[i].content, strong_msg);
    }
    for (i = 1; i < 13; i++) {
        mem->companies[3].messages[i].importance = 6;
        strcpy(mem->companies[3].messages[i].content, medium_msg);
    }
    for (i = 13; i < 33; i++) {
        mem->companies[3].messages[i].importance = 3;
        strcpy(mem->companies[3].messages[i].content, low_msg);
    }
    for (i = 33; i < 63; i++) {
        mem->companies[3].messages[i].importance = 2;
        strcpy(mem->companies[3].messages[i].content, very_low_msg);
    }

    mem->companies[4].row = 6;
    mem->companies[4].col = 1;
    mem->companies[4].size = VERY_SMALL;
    mem->companies[4].nb_employee = 6;
    mem->companies[4].nb_messages = 36;
    mem->companies[4].index = 4;
    mem->companies[4].nb_of_people = 0;
    for (i = 0; i < 1; i++) {
        mem->companies[4].messages[i].importance = 9;
        strcpy(mem->companies[4].messages[i].content, strong_msg);
    }
    for (i = 1; i < 8; i++) {
        mem->companies[4].messages[i].importance = 6;
        strcpy(mem->companies[4].messages[i].content, medium_msg);
    }
    for (i = 8; i < 19; i++) {
        mem->companies[4].messages[i].importance = 3;
        strcpy(mem->companies[4].messages[i].content, low_msg);
    }
    for (i = 19; i < 36; i++) {
        mem->companies[4].messages[i].importance = 2;
        strcpy(mem->companies[4].messages[i].content, very_low_msg);
    }

    mem->companies[5].row = 0;
    mem->companies[5].col = 4;
    mem->companies[5].size = SMALL;
    mem->companies[5].nb_employee = 11;
    mem->companies[5].nb_messages = 49;
    mem->companies[5].index = 5;
    mem->companies[5].nb_of_people = 0;
    for (i = 0; i < 1; i++) {
        mem->companies[5].messages[i].importance = 9;
        strcpy(mem->companies[5].messages[i].content, strong_msg);
    }
    for (i = 1; i < 8; i++) {
        mem->companies[5].messages[i].importance = 6;
        strcpy(mem->companies[5].messages[i].content, medium_msg);
    }
    for (i = 8; i < 19; i++) {
        mem->companies[5].messages[i].importance = 3;
        strcpy(mem->companies[5].messages[i].content, low_msg);
    }
    for (i = 19; i < 49; i++) {
        mem->companies[5].messages[i].importance = 2;
        strcpy(mem->companies[5].messages[i].content, very_low_msg);
    }

    mem->companies[6].row = 2;
    mem->companies[6].col = 1;
    mem->companies[6].size = MEDIUM;
    mem->companies[6].nb_employee = 20;
    mem->companies[6].nb_messages = 63;
    mem->companies[6].index = 6;
    mem->companies[6].nb_of_people = 0;
    for (i = 0; i < 1; i++) {
        mem->companies[6].messages[i].importance = 9;
        strcpy(mem->companies[6].messages[i].content, strong_msg);
    }
    for (i = 1; i < 13; i++) {
        mem->companies[6].messages[i].importance = 6;
        strcpy(mem->companies[6].messages[i].content, medium_msg);
    }
    for (i = 13; i < 33; i++) {
        mem->companies[6].messages[i].importance = 3;
        strcpy(mem->companies[6].messages[i].content, low_msg);
    }
    for (i = 33; i < 63; i++) {
        mem->companies[6].messages[i].importance = 2;
        strcpy(mem->companies[6].messages[i].content, very_low_msg);
    }

    mem->companies[7].row = 4;
    mem->companies[7].col = 3;
    mem->companies[7].size = VERY_SMALL;
    mem->companies[7].nb_employee = 5;
    mem->companies[7].nb_messages = 36;
    mem->companies[7].index = 7;
    mem->companies[7].nb_of_people = 0;
    for (i = 0; i < 1; i++) {
        mem->companies[7].messages[i].importance = 9;
        strcpy(mem->companies[7].messages[i].content, strong_msg);
    }
    for (i = 1; i < 8; i++) {
        mem->companies[7].messages[i].importance = 6;
        strcpy(mem->companies[7].messages[i].content, medium_msg);
    }
    for (i = 8; i < 19; i++) {
        mem->companies[7].messages[i].importance = 3;
        strcpy(mem->companies[7].messages[i].content, low_msg);
    }
    for (i = 19; i < 36; i++) {
        mem->companies[7].messages[i].importance = 2;
        strcpy(mem->companies[7].messages[i].content, very_low_msg);
    }
}

void initialize_map(map_t *map, memory_t * mem) {

    map->rows = MAX_ROWS;
    map->columns = MAX_COLUMNS;
    
    enum cell_type_e map_plan[MAX_ROWS][MAX_COLUMNS] = {
        {WASTELAND, COMPANY, WASTELAND, RESIDENTIAL_BUILDING, COMPANY, WASTELAND, WASTELAND},
        {WASTELAND, SUPERMARKET, WASTELAND, WASTELAND, WASTELAND, RESIDENTIAL_BUILDING, WASTELAND},
        {RESIDENTIAL_BUILDING, COMPANY, WASTELAND, WASTELAND, WASTELAND, WASTELAND, WASTELAND},
        {WASTELAND, WASTELAND, CITY_HALL, WASTELAND, RESIDENTIAL_BUILDING, COMPANY, WASTELAND},
        {COMPANY, RESIDENTIAL_BUILDING, WASTELAND, COMPANY, SUPERMARKET, RESIDENTIAL_BUILDING, RESIDENTIAL_BUILDING},
        {WASTELAND, RESIDENTIAL_BUILDING, WASTELAND, WASTELAND, WASTELAND, WASTELAND, COMPANY},
        {RESIDENTIAL_BUILDING, COMPANY, WASTELAND, RESIDENTIAL_BUILDING, WASTELAND, RESIDENTIAL_BUILDING, WASTELAND}
    };

    for (int i = 0; i < MAX_COLUMNS; ++i) {
        for (int j = 0; j < MAX_ROWS; ++j) {
            map->cells[i][j].column = i;
            map->cells[i][j].row = j;
            map->cells[i][j].type = map_plan[i][j];
            switch(map_plan[i][j]) {
                case WASTELAND:
                    map->cells[i][j].nb_of_characters = NUM_CITIZEN;
                    break;
                case RESIDENTIAL_BUILDING:
                    map->cells[i][j].nb_of_characters = 15;
                    break;
                case CITY_HALL:
                    map->cells[i][j].nb_of_characters = 20;
                    break;
                case COMPANY:
                    map->cells[i][j].nb_of_characters = 50;
                    break;
                case SUPERMARKET:
                    map->cells[i][j].nb_of_characters = 30;
                    break;
            }
        }
    }
    mem->city_hall = (coordinates_t) {.x = 3, .y = 2};

    initialize_companies(mem);

    mem->supermarkets[0] = (coordinates_t) {.x = 4, .y = 4};    
    mem->supermarkets[1] = (coordinates_t) {.x = 1, .y = 1};


    mem->residential_buildings[0] = (coordinates_t) {.x = 5, .y = 1};
    mem->residential_buildings[1] = (coordinates_t) {.x = 1, .y = 5};
    mem->residential_buildings[2] = (coordinates_t) {.x = 6, .y = 3};
    mem->residential_buildings[3] = (coordinates_t) {.x = 2, .y = 0};
    mem->residential_buildings[4] = (coordinates_t) {.x = 3, .y = 4};
    mem->residential_buildings[5] = (coordinates_t) {.x = 4, .y = 5};
    mem->residential_buildings[6] = (coordinates_t) {.x = 6, .y = 5};
    mem->residential_buildings[7] = (coordinates_t) {.x = 0, .y = 3};
    mem->residential_buildings[8] = (coordinates_t) {.x = 4, .y = 1};
    mem->residential_buildings[9] = (coordinates_t) {.x = 6, .y = 0};
    mem->residential_buildings[10] = (coordinates_t) {.x = 4, .y = 6};
}

void initialize_memory(memory_t * memory) 
{    
    semaphore_t *sem;
    sem = open_semaphore("/spy_semaphore");
    P(sem);
    memory->memory_has_changed = 0;
    memory->simulation_has_ended = 0;
    memory->current_turn = 0;
    memory->hour = 0;
    memory->minute = 0;
    memory->working = 0;
    memory->at_home = 0;
    memory->walking = 0;
    memory->shopping = 0;
    // CREATE CITY INFOS
    initialize_map(&memory->map, memory);

    // MAILBOX (in a residential building)
    memory->mailbox_row = 1;
    memory->mailbox_col = 5;
    memory->nb_of_messages_in_mailbox = 0;
    for (int i = 0; i < MAILBOX_CAPACITY; i++) {
        strcpy(memory->mailbox_content[i].content, EMPTY);
        memory->mailbox_content[i].importance = -1;
    }
    for (int k = 0; k < 10; k++) {
        //memory->enemy_country_monitor[k] = malloc(MAX_LENGTH_OF_MESSAGE);
    }
    memory->nb_of_messages_received = 0;
    // CREATE
    V(sem);
    if (DISPLAY == 0) {
        printf("Initialized memory...\n");
    }
}

void next_turn(int sig) 
{
    memory_t * memory;

    semaphore_t *sem;
    sem = open_semaphore("/spy_semaphore");
    P(sem);
    memory = get_data();
    memory->current_turn++; 
    memory->minute += 10;
    if (memory->minute == 60) {
        memory->minute = 0;
        memory->hour += 1;
        if (memory->hour == 24) {
            memory->hour = 0;
            // PASSER AU JOUR SUIVANT
        }
    }

    int i;
    int citizen_working = 0;
    int citizen_at_home = 0;
    int citizen_walking = 0;
    int citizen_shopping = 0;

    for (i = 0; i < NUM_CITIZEN; i++) {
        if (memory->citizens[i].current_state == resting_at_home) {
            citizen_at_home++;
        } else if (memory->citizens[i].current_state == working) {
            citizen_working++;
        } else if (memory->citizens[i].current_state == going_to_company || memory->citizens[i].current_state == going_to_supermarket || memory->citizens[i].current_state == going_back_home) {
            citizen_walking++;
        } else if (memory->citizens[i].current_state == doing_some_shopping) {
            citizen_shopping++;
        }
    }

    memory->at_home = citizen_at_home;
    memory->working = citizen_working;
    memory->walking = citizen_walking;
    memory->shopping = citizen_shopping;

    // DISPLAY
    if (DISPLAY == 0 || DISPLAY == 2) {
        printf("\nTURN %d, HOUR : %d:%d\n", memory->current_turn, memory->hour, memory->minute);
        /*
        printf("CITIZEN : \n");
        printf("working  : %d\n", citizen_working);
        printf("at home  : %d\n", citizen_at_home);
        printf("walking  : %d\n", citizen_walking);
        printf("shopping : %d\n", citizen_shopping);
*/
        spy_t * spy;
        printf("SPIES : \n");
        for (int j = 0; j < NUM_SPIES; j++) {
            spy = &memory->spies[j];
                printf("pos : (%d, %d), id = %d, current_state = %d\n", spy->row, spy->col, spy->id, spy->current_state);
        }

        counter_int_t * counter = &memory->counter_intelligence;
        printf("COUNTER INTELIGENCE current_state = %d\n", counter->current_state);
        printf("position : (%d, %d) company : (%d, %d)\n", counter->row, counter->col, counter->company_row, counter->company_col);
        printf("Targer id = %d\n", counter->target_id);
        printf("mailbox is found = %d\n", counter->mailbox_is_found);
        printf("found suspicious act = %d\n", counter->company_is_found);
       
       /*
        printf("ENEMY COUNTRY MONITOR CONTENT : \n");
        for (int k = 0; k < memory->nb_of_messages_received; k++) {
            printf("- %s\n", memory->enemy_country_monitor[k]);         
        }
        printf("nb msg received : %d\n", memory->nb_of_messages_received);
        */
    }

    int citizen_manager_pid;
    citizen_manager_pid = memory->pids[2];
    if (kill(citizen_manager_pid, SIGTTIN) == -1) {
		perror("kill()");
		exit(EXIT_FAILURE);
	}

    int enemy_spy_network_pid;
    enemy_spy_network_pid = memory->pids[3];
    //printf("sending signal to enemy spy network pid : %d\n", enemy_spy_network_pid);
    if (kill(enemy_spy_network_pid, SIGTTIN) == -1) {
		perror("kill()");
		exit(EXIT_FAILURE);
	}

    int counter_intelligence_pid;
    counter_intelligence_pid = memory->pids[4];
    //printf("sending signal to enemy spy network pid : %d\n", enemy_spy_network_pid);
    if (kill(counter_intelligence_pid, SIGTTIN) == -1) {
		perror("kill()");
		exit(EXIT_FAILURE);
	}

    memory->memory_has_changed = 1; 

    V(sem);

    if (munmap(memory, sizeof(memory_t)) == -1) {
        perror("Error un-mapping shared memory");
    }
}

void end_simulation(int sig) 
{
    memory_t * memory;
    semaphore_t *sem;
    sem = open_semaphore("/spy_semaphore");
    P(sem);
    memory = get_data();
    memory->simulation_has_ended = 1;
    
    /*CITIZEN MANAGER*/
    if (kill(memory->pids[2], SIGTERM) == -1) {
		perror("kill()");
		exit(EXIT_FAILURE);
	}
    /*ENEMY SPY NETWORK*/
    if (kill(memory->pids[3], SIGTERM) == -1) {
		perror("kill()");
		exit(EXIT_FAILURE);
	}
    /*COUNTER INTELLIGENCE*/
    /*
    if (kill(memory->pids[4], SIGTERM) == -1) {
		perror("kill()");
		exit(EXIT_FAILURE);
	}*/
    /*ENEMY COUNTRY*/
    if (kill(memory->pids[5], SIGTERM) == -1) {
		perror("kill()");
		exit(EXIT_FAILURE);
	}
    V(sem);

    //printf("SIMULATION ENDED...\n");
    /* CALL FUNCTION TO FREE MEMORY */
    // shm_unlink, munmap
    if (munmap(memory, sizeof(memory_t)) == -1) {
        perror("Error un-mapping shared memory");
    }
    exit(EXIT_SUCCESS);
}

void manage_spy_simulation() 
{

    //printf("spy simulation process : %d\n", getpid());

    memory_t * memory;
    semaphore_t *sem;
    sem = open_semaphore("/spy_semaphore");
    P(sem);
    /* SPY SIMULATION PID */
    memory = get_data();
    memory->pids[0] = getpid();
    V(sem);

    /* INTERCEPT KILL SIG FROM TIMER*/
    struct sigaction turn_signal, end_signal;

    turn_signal.sa_handler = &next_turn;
    turn_signal.sa_flags = 0;
    if (sigaction(SIGTTIN, &turn_signal, NULL) < 0) {
        handle_fatal_error("Error using sigaction");
    }
    end_signal.sa_handler = &end_simulation;
    end_signal.sa_flags = 0;
    if (sigaction(SIGTERM, &end_signal, NULL) < 0) {
        handle_fatal_error("Error using sigaction");
    }


    if (munmap(memory, sizeof(memory_t)) == -1) {
        perror("Error un-mapping shared memory");
    }
    /* -------------------SIMULATION LOGIC-----------------------*/
    /* next_turn will be called on reception of SIGUSR1 */
    /* end_simulation will be called on reception of SIGUSR2 */     

    while(1){
        //sleep(1);
        //printf("waiting...\n");
    }
}

void manage_timer()
{
    //printf("timer process : %d\n", getpid());

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
        char *args[] = {"./bin/timer", ROUND_TIME, NULL};  

        if (execvp("./bin/timer", args) == -1) {
            handle_fatal_error("Error [execvp(timer)]");
        }
    }
}

void manage_citizen_manager()
{
    //printf("citizen_manager process : %d\n", getpid());

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
        if (execl("./bin/citizen_manager", NULL) == -1) {
            handle_fatal_error("Error [execl()]");
        }
        wait(NULL);
    }
}

void manage_enemy_spy_network()
{
    //("enemy_spy_network process : %d\n", getpid());

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
        if (execvp("./bin/enemy_spy_network", NULL) == -1) {
            handle_fatal_error("Error [execl(enemy_spy_network)]");
        }
    }
}

void manage_counter_intelligence()
{
    //printf("counter_intelligence process : %d\n", getpid());

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
        if (execl("./bin/counter_intelligence", NULL) == -1) {
            handle_fatal_error("Error [execl()]");
        }
        wait(NULL);
    }
}

void manage_enemy_country()
{
    //printf("enemy_country process : %d\n", getpid());

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
        
        if (execl("./bin/enemy_country", NULL) == -1) {
            handle_fatal_error("Error [execl()]");
        }
        wait(NULL);
    }
}

void manage_monitor()
{
    //printf("monitor process : %d\n", getpid());

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
    if (DISPLAY == 1) {
        if (execvp("./bin/monitor", NULL) == -1) {
            handle_fatal_error("Error [execl(monitor)]");
        }
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
