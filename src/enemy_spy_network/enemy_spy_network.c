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

void * spy_routine(spy_t *spy, memory_t * memory){

    printf("starting citizen routine, thread n° %ld\n", pthread_self());

	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
    sa.sa_handler = switch_routine;
    //sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);
	pthread_sigmask(SIG_UNBLOCK, &set, NULL);

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        handle_fatal_error("Error using sigaction");
    }
	while(1) {
		//switch_routine(citizen);
		us_sleep(900000);
		//printf("waiting\n");
	}
	return NULL;

}

void switch_routine(int sig)
{
	printf("thread n° %ld, received next turn order\n", pthread_self());

	int spy_id;
	pthread_mutex_lock(&memory_mutex);

	memory_t * memory;
	semaphore_t *sem;
    sem = open_semaphore("/spy_semaphore");
    P(sem);
	memory = get_data();
	spy_t * spy;
	
	int i = 0;
	while (i < 3) {
		if (memory->spy_threads[i].thread == pthread_self()) {
			spy_id = memory->spy_threads[i].id;
			spy = &memory->spies[spy_id];
			break;
		}
		i++;
	}
	V(sem);


	switch (spy->role) {
		case NORMAL_SPY:
			update_spy(spy, memory);
			break;
		case SPY_OFFICER:
            update_case_officer(spy, memory)
			break;
	}	
	pthread_mutex_unlock(&memory_mutex);
}

void update_spy(spy_t * spy, memory_t * memory){
    if (memory->hour == 8 && memory->minute == 0) {
        spy->is_spotted = 0;
        spy->is_stealed = 0
        coordinates_t company = search_for_company_to_steal(spy, memory);

        int num = rand();
        if ((num % 100) < 60) {
            pthread_mutex_lock(&memory_mutex);
            spy->current_state = spotting;
            pthread_mutex_unlock(&memory_mutex);

            }
        else if ((num % 100) < 30) {
                pthread_mutex_lock(&memory_mutex);
                spy->current_state = going_back_home;
                pthread_mutex_unlock(&memory_mutex);

        }
        else {
            pthread_mutex_lock(&memory_mutex);
                spy->current_state = going_to_supermarket;
                pthread_mutex_unlock(&memory_mutex);
        }
    }

    if (memory->hour >= 8 && memory->hour <17 ) {
        if(spy->current_state == spotting){
            if(spy->col != company.y && spy->row != company.x){
                spy_goto(spy, company.y, company.x);
            } else {
                if(number_round_spotting != 12){
                    spot_company(spy, memory);
                }else{
                    spy->number_round_spotting = 0;
                    spy->is_spotted = 1;
                    go_back_home(spy, memory);
                }
            }
        }

        if(spy->current_state == going_back_home){
            if (spy->col != spy->home_col && spy->row != spy->home_row) {
                go_back_home(spy, memory);
            } else {
                rest_at_home(spy, memory);
            }
        }

        else if(spy->current_state == going_to_supermarket){
            coordinates_t closest = find_closest_supermarket(spy->col, spy->row, memory);
		    if (spy->col != closest.y || spy->row != closest.x) {
			    go_to_supermarket(spy, closest, memory);
		    } else {
			    do_some_shopping(spy, memory);
		    }

        }

        else if(spy->current_state == doing_some_shopping){
            go_back_home(spy, memory);
        }

    if (memory->hour >= 17 && memory->hour < 8 && spy->is_spotted == 1) {
        if(spy->number_round_stealing != 6){
            steal_information(spy, memory);
  
        } else {
            
            spy->number_round_stealing = 0;
            spy->is_stealed = 1;
            if(spy->row != memory->mailbox_row && spy->col != memory->mailbox_col){
                spy_goto_mail_box(spy, memory);
            }else {
                send_message(spy, memory);
            }
        }

    }
    // Logique pour l'espion avec un permis de tuer
    if (spy->license == 1) {
        handle_encounter_with_counter_intelligence(spy, memory);
    }
}


void update_case_officer(spy_t *officer, memory_t *memory) {
    int current_hour = memory->hour;

    if (current_hour == officer->first_message_time || current_hour == officer->second_message_time) {
        go_to_mailbox(officer, memory);
        get_message();
        
    } else if (current_hour == officer->shopping_time) {
        go_to_supermarket(officer, memory);
        get_message();

    } else {
        rest_at_home(officer, memory);
        send_message_to_enemy_contry();
        random_time(officer, memory);
    }
}

void random_time(spy_t *officer, memory_t * memory){
    pthread_mutex_lock(&memory_mutex);
    officer->first_message_time = 8 + (rand() % 9); 
    officer->second_message_time = 8 + (rand() % 9); 

    officer->shopping_time = 17 + (rand() % 2); 
    pthread_mutex_unlock(&memory_mutex);

}

void get_message(spy_t *officer, memory_t * memory){
    

}
void define_spy(spy_t *spy, memory_t * memory){
    int i;

	coordinates_t *buildings = memory->residential_buildings;
    coordinates_t mailbox = {memory->mailbox_row, memory->mailbox_col};


    for(i = 0 ; i < NUM_SPY ; i++){
		spy[i].id = 128 + i;

        if(i<3){
            spy[i].role = NORMAL_SPY;
            spy[i].find_company_to_steel = 0 ;
            spy[i].nb_company_stolen = 0
            spi[i].is_spotted = 0;
            spy[i].number_round_spotting = 0;
            spy[i].number_round_stealing = 0;
            if( i < 2){
                spy[i].license = 0;
            } else {
                spy[i].license = 1;
            }
        } else {
            spy[i].role = SPY_OFFICER;
            spy[i].license = 0;
            spy[i].first_message_time = 8 + (rand() % 9); 
            spy[i].second_message_time = 8 + (rand() % 9); 
            spy[i]->shopping_time = 17 + (rand() % 2); 

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


void update_threads(int sig)
{
	// get threads tid
	citizen_thread_t * thread_info;
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


pthread * spy_thread(memory_t * memory){

    pthread_mutex_init(&memory_mutex, NULL);
	pthread_barrier_init(&barrier, NULL, NUM_CITIZEN);

    semaphore_t* sem;
    sem = open_semaphore("/spy_semaphore");

    for(int i = 0; i < NUM_SPY ; i++){
        p(sem);

		int result = pthread_create(&memory->spy_threads[i].thread, NULL, spy_routine, NULL);
        memory->spy_threads[i].id = i;
		V(sem);

        if (result != 0) {
        		fprintf(stderr, "Failed to create thread: %s\n", strerror(result));
            	for (int j = 0; j < i; j++) {
            		pthread_cancel(memory->spy_threads[j].thread)); 
					pthread_join(memory->spy_threads[j].thread, NULL); 
				}
            	exit(EXIT_FAILURE);
        }
	}
	return memory->spy_threads;

}


void end_simulation(int sig) 
{
    citizen_thread_t * thread_info;
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



coordinates_t search_for_company_to_steal(spy_t *spy, memory_t * memory){
    if(spy>find_company_to_steal == 0){
        coordinates_t company = find_closest_company(spy->col, spy->row, memory);
        int already_stolen = 0;

        for (int i = 0; i < NUM_COMPANIES; i++) {
            if (spy->companies_stoled[i].x == company.x && spy->companies_stoled[i].y == company.y) {
                already_stolen = 1;
                break;
            }
        }

        if (!already_stolen) {
            spy->company_col = company.y;
            spy->company_row = company.x;
        }

        spy->companies_stoled[nb_company_stolen] = company;
        return company;

}


void steal_information(spy_t * spy, memory_t * memory){
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

        spy->message = stolen_priority;
    }


    spy->current_state = stealing;
    pthread_mutex_unlock(&memory_mutex);

}

void send_message(spy_t * spy, memory_t * memory){



    pthread_mutex_lock(&memory_mutex);
    spy->current_state = sending_message;
    pthread_mutex_unlock(&memory_mutex);


}

void spot_company(spy_t * spy, memorry_t * memory){
    printf("Starting spotting a company");
    spy->number_round_spotting += 1;

    pthread_mutex_lock(&memory_mutex);
    spy->current_state = spotting;
    pthread_mutex_unlock(&memory_mutex);

    int entreprise_col = spy->company_col;
    int entreprise_row = spy->company_row;

    int random_col_offset = (rand() % (2 * max_distance + 1)) - max_distance;
    int random_row_offset = (rand() % (2 * max_distance + 1)) - max_distance;

    int new_col = entreprise_col + random_col_offset;
    int new_row = entreprise_row + random_row_offset;

    pthread_mutex_lock(&memory_mutex);
    spy->current_state = spotting;
    spy->col = new_col;
    spy->row = new row;
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
        
    spy_goto(spy, spy->home_col, spy->home_row);

	pthread_mutex_lock(&memory_mutex);
	spy->current_state = going_back_home;
    pthread_mutex_unlock(&memory_mutex);

}

void go_to_supermarket(spy_t * spy, coordinates_t closest, memory_t * memory)
{
    spy_goto(spy, closest.y, closest.);

    pthread_mutex_lock(&memory_mutex);
	spy->current_state = going_to_supermarket;
    pthread_mutex_unlock(&memory_mutex);


}


void do_some_shopping(spy_t * spy, memory_t *memory)
{
	pthread_mutex_lock(&memory_mutex);
	spy->current_state = doing_some_shopping;
    pthread_mutex_unlock(&memory_mutex);


}

void rest_at_home(spy_t *spy, memory_t *memory)
{
    pthread_mutex_lock(&memory_mutex);
	spy->current_state = resting_at_home;
	pthread_mutex_unlock(&memory_mutex);


}

void spy_goto_mail_box(spy_t * spy, memory_t * memory){
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
    int best_col, best_row, best_dist;
    best_dist = MAX_COLUMNS * MAX_COLUMNS + MAX_ROWS * MAX_ROWS;

    for (int dcol=-1; dcol<=1; dcol++) {
        for (int drow=-1; drow<=1; drow++) {
            if (dist(spy->col+dcol, spy->row+drow, destination_col, destination_row) < best_dist) {
                best_col = spy->col+dcol;
                best_row = spy->row+drow;
                best_dist = dist(spy->col+dcol, spy->row+drow, destination_col, destination_row);
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
    int num_supermarkets = 2; 

    for (int i = 0; i < num_supermarkets; i++) {
        int dist = (memory->supermarkets[i].y - current_col) * (memory->supermarkets[i].y - current_col) + (memory->supermarkets[i].x - current_row) * (memory->supermarkets[i].x - current_row);
        if (dist < min_dist) {
            min_dist = dist;
            closest_supermarket = memory->supermarkets[i];
        }
    }

    return closest_supermarket;
}

coordinates_t find_closest_company(int current_col, int current_row, memory_t *memory) 
{
    int min_dist = INT_MAX;
    coordinates_t closest_company;
    int num_company = 8; 

    for (int i = 0; i < num_company; i++) {
        int dist = (memory->companies[i].y - current_col) * (memory->companies[i].y - current_col) + (memory->supermarkets[i].x - current_row) * (memory->supermarkets[i].x - current_row);
        if (dist < min_dist) {
            min_dist = dist;
            closest_company = memory->companies[i];
        }
    }

    return closest_company;
}
