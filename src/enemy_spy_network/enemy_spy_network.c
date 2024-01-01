/*
* ENSICAEN
* 6 Boulevard Maréchal Juin
* F-14050 Caen Cedex
* Projet : A LICENSE TO KILL
*
* Copyright (C) Cécile LU (cecile.lu (at) ecole.ensicaen.fr)
*/

/**
 * \file enemy_spy_network.c
 */
#include "enemy_spy_network.h"

/* GLOBAL VARIABLES */
pthread_mutex_t memory_mutex;
pthread_barrier_t barrier;

void define_spy(spy_t *spy, memory_t * memory)
{
    int i;
	coordinates_t *buildings = memory->residential_buildings;
    coordinates_t mailbox = {memory->mailbox_row, memory->mailbox_col};

    for(i = 0 ; i < NUM_SPIES ; i++){
		spy[i].id = NUM_CITIZEN + i;
        if(i < 3){
            spy[i].role = NORMAL_SPY;
            spy[i].find_company_to_steal = 0 ;
            spy[i].nb_company_stolen = 0;
            spy[i].is_spotted = 0;
            spy[i].number_round_spotting = 0;
            spy[i].number_round_stealing = 0;
            if(i < 2){
                spy[i].license = 0;
            } else {
                spy[i].license = 1;
            }
        } else {
            spy[i].role = SPY_OFFICER;
            spy[i].license = 0;
            spy[i].first_message_time = 8 + (rand() % 9); 
            spy[i].second_message_time = 8 + (rand() % 9); 
            spy[i].shopping_time = 17 + (rand() % 2); 
        }
        int chosen = 0;
        while (!chosen) {
            int immeuble_index = rand() % NUM_RESIDENTIAL_BUILDINGS;
            int mailbox_dist = dist(buildings[immeuble_index].y, buildings[immeuble_index].x, mailbox.y, mailbox.x);

            if (mailbox_dist != 0 && mailbox_dist <= 4) {
                spy[i].home_col = buildings[immeuble_index].y;
                spy[i].home_row = buildings[immeuble_index].x;
                chosen = 1;
            }
        }
        spy[i].col = spy[i].home_col;
        spy[i].row = spy[i].home_row;
        spy[i].health = 10;
        spy[i].current_state = resting_at_home;
	}
}

void switch_routine(int sig)
{
	//printf("thread n° %ld, received next turn order\n", pthread_self());

	int spy_id;
	//pthread_mutex_lock(&memory_mutex);

	memory_t * memory;
	memory = get_data();
    if (memory == NULL) {
        perror("Failed to get shared memory");
        return;
    }
	spy_t * spy;
	
	int i = 0;
	while (i < NUM_SPIES) {
		if (memory->spy_threads[i].thread == pthread_self()) {
			spy_id = memory->spy_threads[i].id;
			spy = &memory->spies[spy_id];
			break;
		}
		i++;
	}
    pthread_barrier_wait(&barrier);

	switch (spy->role) {
		case NORMAL_SPY:
			update_spy(spy, memory);
			break;
		case SPY_OFFICER:
            update_case_officer(spy, memory);
			break;
	}	
    if (munmap(memory, sizeof(memory_t)) == -1) {
        perror("Error un-mapping shared memory");
    }
}

void * spy_routine()
{
    //printf("starting spy routine, thread n° %ld\n", pthread_self());

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
	memory = get_data();
	thread_info = memory->spy_threads;

	int i;
	//printf("pthread_kill()\n");
	for (i = 0; i < NUM_SPIES; i++){
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
    thread_info = memory->spy_threads;

    int i;
    for (i = 0; i < NUM_SPIES ; i++) {
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
    if (sigaction(SIGTERM, &end_simulation, NULL) < 0) {
        handle_fatal_error("Error using sigaction");
    }

    printf("waiting...\n");
    while (1) {}
}

pthread_t * spy_thread(memory_t * memory)
{
    pthread_mutex_init(&memory_mutex, NULL);
	pthread_barrier_init(&barrier, NULL, NUM_SPIES);

    semaphore_t* sem;
    sem = open_semaphore("/spy_semaphore");

    for(int i = 0; i < NUM_SPIES ; i++){
        P(sem);
		int result = pthread_create(&memory->spy_threads[i].thread, NULL, spy_routine, NULL);
        memory->spy_threads[i].id = i;
		V(sem);
        if (result != 0) {
        		fprintf(stderr, "Failed to create thread: %s\n", strerror(result));
            	for (int j = 0; j < i; j++) {
            		pthread_cancel(memory->spy_threads[j].thread); 
					pthread_join(memory->spy_threads[j].thread, NULL); 
				}
            	exit(EXIT_FAILURE);
        }
	}
	return memory->spy_threads;
}


void update_spy(spy_t * spy, memory_t * memory)
{
    printf("current_state : %d, position : (%d, %d)\n", spy->current_state, spy->row, spy->col);
    if (memory->hour == 8 && memory->minute == 0) {
        printf("house : x = %d, y = %d\n", spy->home_row, spy->home_col);
        printf("mailbox : x = %d, y = %d\n", memory->mailbox_row, memory->mailbox_col);

        pthread_mutex_lock(&memory_mutex);
        spy->is_spotted = 0;
        spy->is_stolen = 0;
        pthread_mutex_unlock(&memory_mutex);
        coordinates_t company = search_for_company_to_steal(spy, memory);

        int num = rand();
        if ((num % 100) < 60) {
            pthread_mutex_lock(&memory_mutex);
            spy->current_state = going_to_spot;
            pthread_mutex_unlock(&memory_mutex);
        } else if ((num % 100) < 30) {
            pthread_mutex_lock(&memory_mutex);
            spy->current_state = going_back_home;
            pthread_mutex_unlock(&memory_mutex);
        } else {
            pthread_mutex_lock(&memory_mutex);
            spy->current_state = going_to_supermarket;
            pthread_mutex_unlock(&memory_mutex);
        }
        
    } else if (memory->hour >= 8 && memory->hour < 17) {
        if(spy->current_state == going_to_spot || spy->current_state == spotting){
            //coordinates_t company = spy->companies_stolen[spy->nb_company_stolen];
            if(spy->col != spy->company_col || spy->row != spy->company_row){
                spy_goto(spy, spy->company_col, spy->company_row);
            } else {
                if(spy->number_round_spotting != 12){
                    printf("nb round spotting = %d\n", spy->number_round_spotting);
                    spot_company(spy, memory);
                }else{
                    pthread_mutex_lock(&memory_mutex);
                    spy->number_round_spotting = 0;
                    printf("IS SPOTTED = 1\n");
                    spy->is_spotted = 1;
                    pthread_mutex_unlock(&memory_mutex);
                    go_back_home(spy, memory);
                }
            }
        }
        if(spy->current_state == going_back_home){
            if (spy->col != spy->home_col || spy->row != spy->home_row) {
                go_back_home(spy, memory);
            } else {
                rest_at_home(spy, memory);
            }
        } else if(spy->current_state == going_to_supermarket){
            coordinates_t closest = find_closest_supermarket(spy->col, spy->row, memory);
		    if (spy->col != closest.y || spy->row != closest.x) {
			    go_to_supermarket(spy, closest, memory);
		    } else {
			    do_some_shopping(spy, memory);
		    }
        } else if(spy->current_state == doing_some_shopping){
            go_back_home(spy, memory);
        }
    } else if ((memory->hour >= 17 || memory->hour < 8) && spy->is_spotted == 1) {
        printf("STEAL or go to STEAL (%d, %d)\n", spy->company_row, spy->company_col);

        if (spy->col != spy->company_col || spy->row != spy->company_row) {
            pthread_mutex_lock(&memory_mutex);
            spy->current_state = going_to_steal;
            pthread_mutex_unlock(&memory_mutex);
            spy_goto(spy, spy->company_col, spy->company_row);
        } else {
            if(spy->number_round_stealing != 6) {
                steal_information(spy, memory);
            } else {   
                pthread_mutex_lock(&memory_mutex);
                printf("IS_SPOTTED = 0\n");
                spy->is_spotted = 0;
                spy->is_stolen = 1;
                spy->current_state = going_to_mail_box;
                spy->find_company_to_steal = 0;
                pthread_mutex_unlock(&memory_mutex);
            }
        }
    } else if ((memory->hour >= 17 || memory->hour < 8) && spy->is_stolen == 1) {
        if (spy->current_state == going_to_mail_box){
            printf("GO to MAILBOX\n");
            if(spy->row != memory->mailbox_row || spy->col != memory->mailbox_col){
                go_to_mailbox(spy, memory);
            }else {
                send_message(spy, memory);
                pthread_mutex_lock(&memory_mutex);
                spy->current_state = going_back_home;
                pthread_mutex_unlock(&memory_mutex);
            }
        } else if (spy->current_state == going_back_home){ 
            printf("GO HOME\n");
            if (spy->col != spy->home_col || spy->row != spy->home_row) {
                go_back_home(spy, memory);
            } else {
                pthread_mutex_lock(&memory_mutex);
                spy->is_stolen = 0;
                pthread_mutex_unlock(&memory_mutex);
                rest_at_home(spy, memory);
            }
        }
    }
    // Logique pour l'espion avec un permis de tuer
    if (spy->license == 1) {
        handle_encounter_with_counter_intelligence(spy, memory);
    }
}

void update_case_officer(spy_t *officer, memory_t *memory) 
{
    int current_hour = memory->hour;

    if (current_hour == officer->first_message_time || current_hour == officer->second_message_time) {
        go_to_mailbox(officer, memory);
        get_message(officer, memory);
    } else if (current_hour == officer->shopping_time) {
        coordinates_t closest;
        closest = find_closest_supermarket(officer->col, officer->row, memory);
        go_to_supermarket(officer, closest, memory);
        get_message(officer, memory);
    } else {
        rest_at_home(officer, memory);
        send_message_to_enemy_country(officer, memory);
        randomize_time(officer, memory);
    }
}

void randomize_time(spy_t *officer, memory_t * memory)
{
    pthread_mutex_lock(&memory_mutex);
    officer->first_message_time = 8 + (rand() % 9); 
    officer->second_message_time = 8 + (rand() % 9); 
    officer->shopping_time = 17 + (rand() % 2); 
    pthread_mutex_unlock(&memory_mutex);
}

void get_message(spy_t *officer, memory_t * memory)
{
}

coordinates_t search_for_company_to_steal(spy_t *spy, memory_t * memory)
{
    printf("searching for company to steal\n");
    // if company not found
    if(spy->find_company_to_steal == 0) {
        pthread_mutex_lock(&memory_mutex);
        spy->find_company_to_steal = 1;
        pthread_mutex_unlock(&memory_mutex);
        int random_index;
        int is_already_stolen;
        coordinates_t company;

        do {
            random_index = rand() % NUM_COMPANIES;
            company = memory->companies[random_index];

            is_already_stolen = 0;
            for (int i = 0; i < spy->nb_company_stolen; i++) {
                if (spy->companies_stolen[i].x == company.x && spy->companies_stolen[i].y == company.y) {
                    is_already_stolen = 1;
                    break;
                }
            }
        } while (is_already_stolen);

        if (!is_already_stolen) {
            pthread_mutex_lock(&memory_mutex);
            spy->company_col = memory->companies[random_index].y;
            spy->company_row = memory->companies[random_index].x;
            spy->companies_stolen[spy->nb_company_stolen] = memory->companies[random_index];
            pthread_mutex_unlock(&memory_mutex);
        }
        printf("found company to steal x = %d, y = %d\n", spy->company_row, spy->company_col);
        return company;
    } else {
        printf("company in list x = %d, y = %d\n", spy->company_row, spy->company_col);
        return spy->companies_stolen[spy->nb_company_stolen];     
    }
}

void steal_information(spy_t * spy, memory_t * memory)
{
    printf("stealing information\n");
    // rester ici genre 6 tour 
    //et en meme temps voir la probabilité du message volé
    //ajouter l'entreprise das les entreprise volé
    //mettre spy->finded_company_to_steal a 0
    pthread_mutex_lock(&memory_mutex);

    spy->number_round_stealing += 1;
    if(spy->number_round_stealing == 6){
        spy->nb_company_stolen += 1;
        int random_number = rand() % 100;
        int stolen_priority;

        if (random_number < 1) { // 1% pour crucial
            stolen_priority = 10;
        } else if (random_number < 6) { // 5% pour strong
            stolen_priority = 9;
        } else if (random_number < 20) { // 14% pour medium
            stolen_priority = 6;
        } else if (random_number < 50) { // 30% pour low
            stolen_priority = 3;
        } else { // 50% pour verylow
            stolen_priority = 2;
        }
        printf("priorité du message volé : %d\n", stolen_priority);
        spy->message = stolen_priority;
        spy->current_state = going_to_mail_box;
    } else {
        spy->current_state = stealing;
    }

    pthread_mutex_unlock(&memory_mutex);
}

void send_message(spy_t * spy, memory_t * memory)
{
    printf("SENDING MESSAGE\n");
    pthread_mutex_lock(&memory_mutex);
    spy->current_state = sending_message;
    pthread_mutex_unlock(&memory_mutex);
}

void spot_company(spy_t * spy, memory_t * memory)
{
    printf("Starting spotting a company\n");
    int max_distance = 1;

    int entreprise_col = spy->company_col;
    int entreprise_row = spy->company_row;

    int random_col_offset = (rand() % (2 * max_distance + 1)) - max_distance;
    int random_row_offset = (rand() % (2 * max_distance + 1)) - max_distance;

    int new_col = entreprise_col + random_col_offset;
    int new_row = entreprise_row + random_row_offset;

    pthread_mutex_lock(&memory_mutex);
    spy->number_round_spotting += 1;
    spy->current_state = spotting;
    spy->col = new_col;
    spy->row = new_row;
    pthread_mutex_unlock(&memory_mutex);

    // se deplacer autour de l'entreprise de maniere random a chaque tour
    //ajouter un tour a chaque tour au number_round_spotting
    // tant que le nombre de round=!12 recommencer, 
    // recevoir les signaux
    //quand le spotting est fini, le remettre à 0
    // Mettre l'etat a spotting
}

void go_back_home(spy_t * spy, memory_t * memory)
{
    printf("go back home\n");
    spy_goto(spy, spy->home_col, spy->home_row);
	pthread_mutex_lock(&memory_mutex);
	spy->current_state = going_back_home;
    pthread_mutex_unlock(&memory_mutex);

}

void go_to_supermarket(spy_t * spy, coordinates_t closest, memory_t * memory)
{
    printf("go to supermarket\n");
    spy_goto(spy, closest.y, closest.x);
    pthread_mutex_lock(&memory_mutex);
	spy->current_state = going_to_supermarket;
    pthread_mutex_unlock(&memory_mutex);
}


void do_some_shopping(spy_t * spy, memory_t *memory)
{
    printf("do some shopping\n");
	pthread_mutex_lock(&memory_mutex);
	spy->current_state = doing_some_shopping;
    pthread_mutex_unlock(&memory_mutex);
}

void rest_at_home(spy_t *spy, memory_t *memory)
{
    printf("rest at home\n");
    pthread_mutex_lock(&memory_mutex);
	spy->current_state = resting_at_home;
	pthread_mutex_unlock(&memory_mutex);
}

void go_to_mailbox(spy_t * spy, memory_t * memory)
{
    printf("go to mailbox (%d, %d)\n", memory->mailbox_row, memory->mailbox_col);
    spy_goto(spy, memory->mailbox_col, memory->mailbox_row);
	pthread_mutex_lock(&memory_mutex);
    spy->current_state = going_to_mail_box;
    pthread_mutex_unlock(&memory_mutex);
}

int dist(int r1, int c1, int r2, int c2) 
{
    return abs(r1 - r2) + abs(c1 - c2);
}

void spy_goto(spy_t * spy, int destination_col, int destination_row) 
{
    if (spy->row == destination_row && spy->col == destination_col) {
        return;
    }
    int best_col, best_row, best_dist;
    best_col = 0;
	best_row = 0;
	best_dist = dist(0, 0, MAX_ROWS - 1, MAX_COLUMNS - 1);
    for (int dcol=-1; dcol<=1; dcol++) {
        for (int drow=-1; drow<=1; drow++) {
            if (spy->col + dcol >= MAX_COLUMNS || spy->row + drow >= MAX_ROWS || spy->col + dcol < 0 || spy->col + dcol < 0) {
				continue;
			}
            if (dist(spy->row + drow, spy->col + dcol, destination_row, destination_col) < best_dist) {
                best_col = spy->col + dcol;
                best_row = spy->row + drow;
                best_dist = dist(spy->row + drow, spy->col + dcol, destination_row, destination_col);
            }
        }
    }

    pthread_mutex_lock(&memory_mutex);
    spy->col = best_col;
    spy->row = best_row;
    pthread_mutex_unlock(&memory_mutex);
}

coordinates_t find_closest_supermarket(int current_col, int current_row, memory_t *memory) 
{
    int min_dist = INT_MAX;
    coordinates_t closest_supermarket;

    for (int i = 0; i < NUM_SUPERMARKETS; i++) {
        int dist = (memory->supermarkets[i].y - current_col) * (memory->supermarkets[i].y - current_col) + (memory->supermarkets[i].x - current_row) * (memory->supermarkets[i].x - current_row);
        if (dist < min_dist) {
            min_dist = dist;
            closest_supermarket = memory->supermarkets[i];
        }
    }

    return closest_supermarket;
}

void send_message_to_enemy_country(spy_t * spy, memory_t * memory)
{

}

void handle_encounter_with_counter_intelligence(spy_t * spy, memory_t * memory)
{

}