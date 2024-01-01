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
#include "citizen_manager.h"

/* GLOBAL VARIABLES */
pthread_mutex_t memory_mutex;
pthread_barrier_t barrier;

void define_citizen(citizen_t * citizens, memory_t *memory)
{
	int i;
	coordinates_t * companies = memory->companies;
	coordinates_t * buildings = memory->residential_buildings;
    coordinates_t * supermarkets = memory->supermarkets;

	for(i = 0 ; i < NUM_CITIZEN ; i++){
		citizens[i].id = i;
		
		// Assignation aléatoire des positions de la maison et du travail
        int immeuble_index = i % NUM_RESIDENTIAL_BUILDINGS;
        citizens[i].home_col = buildings[immeuble_index].y;
        citizens[i].home_row = buildings[immeuble_index].x;
		
		citizens[i].health = 10;
        citizens[i].current_state = resting_at_home;
		citizens[i].row = citizens[i].home_row;
		citizens[i].col = citizens[i].home_col;

		// Role
		if (i < 111) { 
            citizens[i].role = ROLE_COMPANY;
			int entreprise_index = i % NUM_COMPANIES; // Répartition uniforme
			citizens[i].work_col = companies[entreprise_index].y;
            citizens[i].work_row = companies[entreprise_index].x;
        } else if (i < 121) { 
            citizens[i].role = ROLE_CITY_HALL;
			citizens[i].work_col = memory->city_hall.y;
        	citizens[i].work_row = memory->city_hall.x;
        } else {
            citizens[i].role = ROLE_SUPERMARKET;
			int supermarche_index = (i - 121) % NUM_SUPERMARKETS; // Répartition uniforme parmi les supermarchés
            citizens[i].work_col = supermarkets[supermarche_index].y;
            citizens[i].work_row = supermarkets[supermarche_index].x;
        }
	}
}	

void switch_routine(int sig)
{
	//printf("thread n° %ld, received next turn order\n", pthread_self());
	//printf("TEST\n");


	int citizen_id;
	//pthread_mutex_lock(&memory_mutex);

	memory_t * memory;
	semaphore_t *sem;
    sem = open_semaphore("/spy_semaphore");
	//pthread_mutex_lock(&memory_mutex);
	memory = get_data();
	if (memory == NULL) {
        perror("Failed to get shared memory");
        return;
    }

	citizen_t * citizen;
	
	//printf("searching for citizen...\n");
	int i = 0;
	while (i < NUM_CITIZEN) {
		if (memory->citizen_threads[i].thread == pthread_self()) {
			citizen_id = memory->citizen_threads[i].id;
			citizen = &memory->citizens[citizen_id];
			//printf("citizen id : %d\n", citizen_id);
			break;
		}
		i++;
	}
	
	pthread_barrier_wait(&barrier);

	switch (citizen->role) {
		case ROLE_CITY_HALL:
			update_employee(citizen, memory);
			break;
		case ROLE_SUPERMARKET:
			update_supermarket_employee(citizen, memory);
			break;
		case ROLE_COMPANY:
			update_employee(citizen, memory);
			break;
	}	
	//pthread_mutex_unlock(&memory_mutex);
	if (munmap(memory, sizeof(memory_t)) == -1) {
        perror("Error un-mapping shared memory");
    }
}

void *citizen_routine()
{
	//printf("starting citizen routine, thread n° %ld\n", pthread_self());

	sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGTTIN);
    pthread_sigmask(SIG_BLOCK, &set, NULL);

	struct sigaction sa;
    sa.sa_handler = &switch_routine;
    sa.sa_flags = 0;
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        handle_fatal_error("Error using sigaction");
    }

	while(1) {}
	return NULL;
}

void update_threads(int sig)
{
	// get threads tid
	character_thread_t * thread_info;
	memory_t * memory;
	semaphore_t *sem;
    sem = open_semaphore("/spy_semaphore");
	memory = get_data();
	thread_info = memory->citizen_threads;

	int i;
	//printf("pthread_kill()\n");
	for (i = 0; i < NUM_CITIZEN; i++){
		pthread_kill(thread_info[i].thread, SIGUSR1);
	}
	if (munmap(memory, sizeof(memory_t)) == -1) {
        perror("Error un-mapping shared memory");
    }
}

void end_simulation(int sig) 
{
    character_thread_t * thread_info;
    memory_t * memory;
    memory = get_data();
    thread_info = memory->citizen_threads;

    int i;
    for (int i = 0; i < NUM_CITIZEN ; i++) {
        pthread_join(thread_info[i].thread, NULL);
    }

    if (munmap(memory, sizeof(memory_t)) == -1) {
        perror("Error un-mapping shared memory");
    }
    pthread_mutex_destroy(&memory_mutex);
    pthread_barrier_destroy(&barrier);
}

void wait_for_signal(pthread_t * threads)
{
	struct sigaction next_turn, simulation_end;

    next_turn.sa_handler = &update_threads;
    next_turn.sa_flags = 0;
    if (sigaction(SIGTTIN, &next_turn, NULL) < 0) {
        handle_fatal_error("Error using sigaction");
    }

    simulation_end.sa_handler = &end_simulation;
    simulation_end.sa_flags = 0;
    if (sigaction(SIGTTIN, &end_simulation, NULL) < 0) {
        handle_fatal_error("Error using sigaction");
    }

    //printf("waiting...\n");
    while (1) {}
}

character_thread_t * citizen_thread(memory_t * memory)
{
	pthread_mutex_init(&memory_mutex, NULL);
	pthread_barrier_init(&barrier, NULL, NUM_CITIZEN);

	semaphore_t* sem;
    sem = open_semaphore("/spy_semaphore");
  
	for(int i = 0; i < NUM_CITIZEN; i++){
		P(sem);
		int result = pthread_create(&memory->citizen_threads[i].thread, NULL, citizen_routine, NULL);
		memory->citizen_threads[i].id = i;
		V(sem);
        if (result != 0) {
            fprintf(stderr, "Failed to create thread: %s\n", strerror(result));

            for (int j = 0; j < i; j++) {
                pthread_cancel(memory->citizen_threads[j].thread); 
				pthread_join(memory->citizen_threads[j].thread, NULL); 
			}
            exit(EXIT_FAILURE);
        }
	}
	return memory->citizen_threads;
}

void update_employee(citizen_t * citizen, memory_t *memory)
{
	//printf("UPDATE :  employee thread %ld, id : %d, it is %d hour\n", pthread_self(), citizen->id, memory->hour);
	//printf("position : x = %d, y = %d\n", citizen->row, citizen->col);
	if(memory->hour >= 8 && memory->hour < 17){	
		if (citizen->col != citizen->work_col || citizen->row != citizen->work_row) {
			//printf("go_to_work()\n");
			go_to_work(citizen, memory);
		}
		else{
			//printf("work()\n");
			work(citizen, memory);
		}
	}
	if (memory->hour == 17) {
		// 25% de chance d'aller au supermarché
		int random_number = rand();
		if (random_number < RAND_MAX / 4) {
			pthread_mutex_lock(&memory_mutex);
			citizen->current_state = going_to_supermarket;
			pthread_mutex_unlock(&memory_mutex);
		} else {
			pthread_mutex_lock(&memory_mutex);
			citizen->current_state = going_back_home;
			pthread_mutex_unlock(&memory_mutex);
		}
	}
	if(memory->hour >= 17){
		if(citizen->current_state == going_to_supermarket) {
			coordinates_t closest = find_closest_supermarket(citizen->col, citizen->row, memory);
			if (citizen->col != closest.x || citizen->row != closest.y) {
				go_to_supermarket(citizen, closest, memory);
			} else {
				do_some_shopping(citizen, memory);
			}
		} else if (citizen->current_state == doing_some_shopping) {
			go_back_home(citizen, memory);
		} else {
			if (citizen->col != citizen->home_col || citizen->row != citizen->home_row) {
				go_back_home(citizen, memory);
			} else {
				rest_at_home(citizen, memory);
			}
		}
	}
	if(memory->hour < 8){
		rest_at_home(citizen, memory);
	}
}


void update_supermarket_employee(citizen_t * citizen, memory_t *memory)
{
	//printf("updating supermarket employee...it is %d hour\n", memory->hour);
	if(memory->hour >= 8 && memory->hour < 19){
		if (citizen->col != citizen->work_col || citizen->row != citizen->work_row) {
			go_to_work(citizen, memory);
		}
		else{
			work(citizen, memory);
		}
	} else if(memory->hour == 19 && memory->minute < 30){
		do_some_shopping(citizen, memory);
	} else {
		if (citizen->col != citizen->home_col || citizen->row != citizen->home_row) {
			go_back_home(citizen, memory);
		} else {
			rest_at_home(citizen, memory);
		}
	}
}

int dist(int r1, int c1, int r2, int c2) 
{
    return abs(r1 - r2) + abs(c1 - c2);
}

coordinates_t find_closest_supermarket(int current_col, int current_row, memory_t *memory) 
{
    int min_dist = INT_MAX;
    coordinates_t closest_supermarket;

    for (int i = 0; i < NUM_SUPERMARKETS; i++) {
		// Manhattan distance
		int distance = dist(memory->supermarkets[i].y, memory->supermarkets[i].x, current_col, current_row);
		//int dist = abs(memory->supermarkets[i].x - current_row) + abs(memory->supermarkets[i].y - current_column);
        //int dist = (memory->supermarkets[i].x - current_col) * (memory->supermarkets[i].x - current_col) + (memory->supermarkets[i].y - current_row) * (memory->supermarkets[i].y - current_row);
        if (distance < min_dist) {
            min_dist = distance;
            closest_supermarket = memory->supermarkets[i];
        }
    }

    return closest_supermarket;
}

void go_back_home(citizen_t * citizen, memory_t * memory)
{
    citizen_goto(citizen, citizen->home_col, citizen->home_row);
    
	pthread_mutex_lock(&memory_mutex);
	citizen->current_state = going_back_home;
	pthread_mutex_unlock(&memory_mutex);
}

void go_to_work(citizen_t * citizen, memory_t * memory)
{
	pthread_mutex_lock(&memory_mutex);
	citizen->current_state = going_to_company;
	pthread_mutex_unlock(&memory_mutex);

    citizen_goto(citizen, citizen->work_col, citizen->work_row);
}

void go_to_supermarket(citizen_t * citizen, coordinates_t closest, memory_t * memory)
{
	citizen_goto(citizen, closest.x, closest.y);

	pthread_mutex_lock(&memory_mutex);
	citizen->current_state = going_to_supermarket;
	pthread_mutex_unlock(&memory_mutex);
}

void work(citizen_t *citizen, memory_t * memory)
{
	pthread_mutex_lock(&memory_mutex);
	citizen->current_state = working;
	pthread_mutex_unlock(&memory_mutex);
}

void do_some_shopping(citizen_t * citizen, memory_t *memory)
{
	pthread_mutex_lock(&memory_mutex);
	citizen->current_state = doing_some_shopping;
	pthread_mutex_unlock(&memory_mutex);
}

void rest_at_home(citizen_t *citizen, memory_t *memory)
{
	pthread_mutex_lock(&memory_mutex);
	citizen->current_state = resting_at_home;
	pthread_mutex_unlock(&memory_mutex);
}


void citizen_goto(citizen_t * citizen, int destination_col, int destination_row) 
{
    int best_col, best_row, best_dist;
	best_col = 0;
	best_row = 0;
    //best_dist = MAX_COLUMNS * MAX_COLUMNS + MAX_ROWS * MAX_ROWS;
	best_dist = dist(0, 0, MAX_ROWS - 1, MAX_COLUMNS - 1);

	// chose best square to move to
    for (int dcol=-1; dcol<=1; dcol++) {
        for (int drow=-1; drow<=1; drow++) {
			if (citizen->col + dcol > 6 || citizen->row + drow > 6) {
				continue;
			}
            if (dist(citizen->row + drow, citizen->col + dcol, destination_row, destination_col) < best_dist) {
                best_col = citizen->col + dcol;
                best_row = citizen->row + drow;
                best_dist = dist(citizen->row + drow, citizen->col + dcol, destination_row, destination_col) ;
            }
        }
    }
	pthread_mutex_lock(&memory_mutex);
    citizen->col = best_col;
    citizen->row = best_row;
	pthread_mutex_unlock(&memory_mutex);
}
