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



void *citizen_routine(citizen_t * citizen)
{
	printf("thread id = %ld\n", pthread_self());

	struct sigaction turn_signal, end_signal;
/*
	memset(&turn_signal, 0, sizeof(turn_signal));
    turn_signal.sa_sigaction = &switch_routine;
    turn_signal.sa_flags = SA_SIGINFO;
    if (sigaction(SIGTTIN, &turn_signal, NULL) < 0) {
        handle_fatal_error("Error using sigaction");
    }*/

	turn_signal.sa_handler = &switch_routine;
    turn_signal.sa_flags = 0;
    if (sigaction(SIGTTIN, &turn_signal, NULL) < 0) {
        handle_fatal_error("Error using sigaction");
    }

    end_signal.sa_handler = &free_thread_resources;
    end_signal.sa_flags = 0;
    if (sigaction(SIGTERM, &end_signal, NULL) < 0) {
        handle_fatal_error("Error using sigaction");
    }

	while(1) {}
}

void switch_routine()
{
	int thread_id = pthread_self();

	memory_t * memory;
	citizen_t * citizen;
	semaphore_t *sem;
    sem = open_semaphore("/spy_semaphore");
    P(sem);
	memory = get_data();
	citizen = &memory->citizens[thread_id];
	V(sem);

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
}

void update_employee(citizen_t * citizen, memory_t *memory)
{

	if(memory->hour >= 8 && memory->hour < 17){
		if (citizen->col != citizen->work_col || citizen->row != citizen->work_row) {
			go_to_work(citizen, memory);
		}
		else{
			work(citizen, memory);
		}
	}
	if(memory->hour >= 17){
		int random_number = rand();
		if(random_number < RAND_MAX / 4) { //25% de chance d'aller au supermarché
			coordinates_t closest = find_closest_supermarket(citizen->col, citizen->row, memory);
			if (citizen->col != closest.x || citizen->row != closest.y) {
				go_to_supermarket(citizen, closest, memory);
			} else {
				do_some_shopping(citizen, memory);
			}

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

	if(memory->hour >= 8 && memory->hour < 19){
		if (citizen->col != citizen->work_col || citizen->row != citizen->work_row) {
			go_to_work(citizen, memory);
		}
		else{
			work(citizen, memory);
		}
	}
	if(memory->hour >= 19 && memory->minute < 30 && memory->hour < 20){
		do_some_shopping(citizen, memory);

	} 
	if(memory->hour >20) {
		if (citizen->col != citizen->home_col || citizen->row != citizen->home_row) {
			go_back_home(citizen, memory);
		} else {
			rest_at_home(citizen, memory);
		}

	}

	if(memory->hour < 8){
		rest_at_home(citizen, memory);
	}

}

void define_citizen(citizen_t * citizens, memory_t *memory)
{
	int i;

	coordinates_t *companies = memory->companies;
	coordinates_t *buildings = memory->residential_buildings;
    coordinates_t *supermarkets = memory->supermarkets;

	for(i = 0 ; i < NUM_CITIZEN ; i++){
		citizens[i].id = i;
		
		// Assignation aléatoire des positions de la maison et du travail
        int immeuble_index = i % NUM_RESIDENTIAL_BUILDINGS;
        citizens[i].home_col = buildings[immeuble_index].x;
        citizens[i].home_row = buildings[immeuble_index].y;
		
		citizens[i].health = 10;
        citizens[i].current_state = resting_at_home; // État initial

		// Role
		if (i < 111) { 
            citizens[i].role = ROLE_COMPANY;
			int entreprise_index = i % NUM_COMPANIES; // Répartition uniforme
			citizens[i].work_col = companies[entreprise_index].x;
            citizens[i].work_row = companies[entreprise_index].y;
        } else if (i < 121) { 
            citizens[i].role = ROLE_CITY_HALL;
			citizens[i].work_col = memory->city_hall.x;
        	citizens[i].work_row = memory->city_hall.y;
        } else {
            citizens[i].role = ROLE_SUPERMARKET;
			int supermarche_index = (i - 121) % NUM_SUPERMARKETS; // Répartition uniforme parmi les supermarchés
            citizens[i].work_col = supermarkets[supermarche_index].x;
            citizens[i].work_row = supermarkets[supermarche_index].y;
        }
	}
}	

pthread_t * citizen_thread(memory_t * memory)
{
	pthread_t * threads = (pthread_t*) malloc(NUM_CITIZEN * sizeof(pthread_t));

	if (threads == NULL){
		perror("Failed to allocate memory for threads");
		exit(EXIT_FAILURE);
	}

	for(int i = 0; i < NUM_CITIZEN ; i++){
		citizen_t citizen = memory->citizens[i];
		int result = pthread_create(&threads[i], NULL, citizen_routine, &citizen);
        if (result != 0) {
            fprintf(stderr, "Failed to create thread: %s\n", strerror(result));

            for (int j = 0; j < i; j++) {
                pthread_cancel(threads[j]); 
				pthread_join(threads[j], NULL); 
			}
			free(threads);
            exit(EXIT_FAILURE);
        }
	}
	return threads;
}

void free_thread_resources(pthread_t *threads) 
{
    for (int i = 0; i < NUM_CITIZEN ; i++) {
        pthread_join(threads[i], NULL);
    }
    free(threads);
}

int dist(int c1, int r1, int c2, int r2) 
{
    return (c1-c2) * (c1-c2) + (r1-r2) * (r1-r2);
}

coordinates_t find_closest_supermarket(int current_col, int current_row, memory_t *memory) 
{
    int min_dist = INT_MAX;
    coordinates_t closest_supermarket;
    int num_supermarkets = 2; 

    for (int i = 0; i < num_supermarkets; i++) {
        int dist = (memory->supermarkets[i].x - current_col) * (memory->supermarkets[i].x - current_col) + (memory->supermarkets[i].y - current_row) * (memory->supermarkets[i].y - current_row);
        if (dist < min_dist) {
            min_dist = dist;
            closest_supermarket = memory->supermarkets[i];
        }
    }

    return closest_supermarket;
}

void go_back_home(citizen_t * citizen, memory_t * memory)
{
 	if (citizen->col != citizen->home_col || citizen->row != citizen->home_row) {
     citizen_goto(citizen, citizen->home_col, citizen->home_row);
    }
	citizen->current_state = going_back_home;
}

void go_to_work(citizen_t * citizen, memory_t * memory)
{
	if (citizen->col != citizen->work_col || citizen->row != citizen->work_row) {
     citizen_goto(citizen, citizen->work_col, citizen->work_row);
    }
	citizen->current_state = going_to_company;
}

void go_to_supermarket(citizen_t * citizen, coordinates_t closest, memory_t * memory)
{
	if (citizen->col != closest.x || citizen->row != closest.y) {
		citizen_goto(citizen, closest.x, closest.y);
	}
	citizen->current_state = going_to_supermarket;
}

void work(citizen_t *citizen, memory_t * memory)
{
	citizen->current_state = working;
}

void do_some_shopping(citizen_t * citizen, memory_t *memory)
{
	citizen->current_state = doing_some_shopping;
}

void rest_at_home(citizen_t *citizen, memory_t *memory)
{
	citizen->current_state = resting_at_home;
}


void citizen_goto(citizen_t * citizen, int destination_col, int destination_row) 
{
    int best_col, best_row, best_dist;
    best_dist = MAX_COLUMNS * MAX_COLUMNS + MAX_ROWS * MAX_ROWS;

    for (int dcol=-1; dcol<=1; dcol++) {
        for (int drow=-1; drow<=1; drow++) {
            if (dist(citizen->col+dcol, citizen->row+drow, destination_col, destination_row) < best_dist) {
                best_col = citizen->col+dcol;
                best_row = citizen->row+drow;
                best_dist = dist(citizen->col+dcol, citizen->row+drow, destination_col, destination_row);
            }
        }
    }
    citizen->col = best_col;
    citizen->row = best_row;
}
