/*
ENSICAEN
6 Boulevard Maréchal Juin
F-14050 Caen Cedex
Projet : A LICENSE TO KILL
*
Copyright (C) Cécile LU (cecile.lu (at) ecole.ensicaen.fr)
*/

/**
 
\file counter_intelligence.c*/
#include "counter_intelligence.h"

void initialize_counter_intelligence(counter_int_t * counter, memory_t * memory)
{
    counter->id = 131;
    counter->home_row = memory->city_hall.x;
    counter->home_col = memory->city_hall.y;
    counter->health = 10;
    counter->target_id = -1;

    counter->row = counter->home_row;
    counter->col = counter->home_col;
    counter->current_state = resting_at_home;
}

void routine(counter_int_t * counter, memory_t * memory){
    if(memory->hour > 17 && memory->hour < 8 && counter->company_is_founded != 1){
        find_suspiscious_act(counter, memory);
    } else {
        if(memory->hour == 8){
            int num = (rand()%(17 - 8 + 1)) + 8;
        }
        if(counter->mailbox_is_founded == 1 && counter->hour == memory->hour){
            if(counter->row != memory->mailbox_row || counter->col != memory->mailbox_col){
                go_to_mailbox(counter, memory);
            } else {
                int spy_present = 0;
                spy_t *spy
                for (int i = 0; i < NUM_SPIES; i++) {
                    spy = &memory->spies[i];
                        if (spy->row == counter->row && spy->col == counter->col) {
                            spy_present = 1;
                            break;
                        }
                }   

                if (spy_present) {
                    altercation_with_spy(counter, memory, spy);
                }
                else {
                    get_message(counter, memory);
                }

            }
        }
    }
    if(counter->current_state == founded_suspiscious_act){
        if(counter->row != counter->company_row || counter->col != counter->company_col){
            go_to_company(counter, memory);
        } else {
            counter->current_state = hide;
            hide(counter, memory);
        }
        if(counter->counter_is_spotted == 1){
            counter->current_state = following_counter;
            follow_counter(counter,memory);
        }
        if(counter->find_a_residential_building == 1){
            counter->current_state = searching_mailbox;
            find_mailbox()
            if(counter->mailbox_is_founded == 1){
                get_message(counter, memory)
            }
        }
    }
}

void find_suspiscious_act(counter_int_t * counter, memory_t *memory){
    for (int i = 0; i < NUM_COMPANIES; i++) {
        company_t *company = &memory->companies[i];

        if (company->nb_of_people != 0) {
            pthread_mutex_lock(&memory_mutex);
            counter->company_col = company->col;
            counter->company_row = company->row;
            counter->company_is_founded = 1;
            counter->current_state = founded_suspiscious_act;
            pthread_mutex_unlock(&memory_mutex);

            break;  
        }
    }
}


void hide(counter_int_t *counter, memory_t *memory) {
    int max_distance = 1; 
    int entreprise_col = counter->company_col;
    int entreprise_row = counter->company_row;

    int random_col_offset = (rand() % (2 * max_distance + 1)) - max_distance;
    int random_row_offset = (rand() % (2 * max_distance + 1)) - max_distance;

    pthread_mutex_lock(&memory_mutex);
    counter->col = entreprise_col + random_col_offset;
    counter->row = entreprise_row + random_row_offset;
    pthread_mutex_unlock(&memory_mutex);

    for (int i = 0; i < NUM_SPIES; i++) {
        spy_t *spy = &memory->spies[i];
        if (spy->col == counter->col && spy->row == counter->row) {
            pthread_mutex_lock(&memory_mutex);
            counter->target_id = spy->id;
            pthread_mutex_unlock(&memory_mutex);
            break;
        }
    }
}


void follow_counter(counter_int_t *counter, memory_t *memory) {
    int target_id = counter->target_id; // L'ID de l'espion à suivre
    spy_t *target_spy = &memory->spies[target_id];

    pthread_mutex_lock(&memory_mutex);
    int destination_col = target_spy->col;
    int destination_row = target_spy->row;
    pthread_mutex_unlock(&memory_mutex);

    // Utilisez la fonction counter_goto pour se déplacer vers l'espion
    if(target_spy->col != memory->mailbox_col || target_spy->row != memory->mailbox_row){
        counter_goto(counter, destination_col, destination_row);
    }
}


void find_mailbox(counter_int_t * counter, memory_t * memory){
    int chance_to_find = 70;
    int num;

    chance_to_find += 10 * counter->days_without_finding;

    num =rand()% 100;
    if(memory->mailbox_col == counter->col && memory->mailbox_row == counter->row){
        if (num < chance_to_find) {
            counter->mailbox_is_founded = 1;
            counter->days_without_finding = 0;
            counter->mailbox_col = counter->col;
            counter->mailbox_row = counter->row;
            get_message(counter, memory);
        } else {
            printf("Mailbox non trouvée, réessayera demain.\n");
            counter->mailbox_is_founded = 0;
            counter->days_without_finding++;
            counter->current_state = going_back_home;
        }
    } else {
        counter->current_state = going_back_home;
    }
}

void altercation_with_spy(counter_int_t * counter, memory_t * memory, spy_t* spy){
    int chance = rand() % 100;

    if(chance < 10) {
        fight_with_spy(counter, memory, spy);
    } else {
        counter->current_state = run_away;
        go_back_home(counter,memory);
    }

}

void fight_with_spy(counter_int_t* counter, memory_t * memory, spy_t* spy){
    int spy_tid;
    for(int i = 0; i< NUM_SPIES; i++){
        
        if(spy->id == memory->spy_threads[i].id){
            spy_tid = memory->spy_threads[i].thread;

        }
    }
    pthread_kill(spy_tid, SIGUSR2);
    

}

void get_message(counter_int_t* counter, memory_t* memory){

}

void go_back_home(counter_int_t * counter, memory_t * memory){
    if (DISPLAY == 0) {
        printf("go back home\n");
    }
    counter_goto(counter, counter->home_col, counter->home_row);
	pthread_mutex_lock(&memory_mutex);
	counter->current_state = going_back_home;
    pthread_mutex_unlock(&memory_mutex);

}

void rest_at_home(counter_int_t *counter, memory_t *memory)
{
    if (DISPLAY == 0) {
        printf("rest at home\n");
    }
    pthread_mutex_lock(&memory_mutex);
	counter->current_state = resting_at_home;
	pthread_mutex_unlock(&memory_mutex);
}

void go_to_mailbox(counter_int_t * counter, memory_t * memory)
{   
    if (DISPLAY == 0) {
        printf("go to mailbox (%d, %d)\n", memory->mailbox_row, memory->mailbox_col);
    }
    counter_goto(counter, memory->mailbox_col, memory->mailbox_row);
	pthread_mutex_lock(&memory_mutex);
    counter->current_state = going_to_mail_box;
    pthread_mutex_unlock(&memory_mutex);
}

int dist(int r1, int c1, int r2, int c2) 
{
    return abs(r1 - r2) + abs(c1 - c2);
}

void counter_goto(counter_int_t * counter, int destination_col, int destination_row) 
{
    if (counter->row == destination_row && counter->col == destination_col) {
        return;
    }
    int best_col, best_row, best_dist;
    best_col = 0;
	best_row = 0;
	best_dist = dist(0, 0, MAX_ROWS - 1, MAX_COLUMNS - 1);
    for (int dcol=-1; dcol<=1; dcol++) {
        for (int drow=-1; drow<=1; drow++) {
            if (counter->col + dcol >= MAX_COLUMNS || counter->row + drow >= MAX_ROWS || counter->col + dcol < 0 || counter->col + dcol < 0) {
				continue;
			}
            if (dist(counter->row + drow, counter->col + dcol, destination_row, destination_col) < best_dist) {
                best_col = counter->col + dcol;
                best_row = counter->row + drow;
                best_dist = dist(counter->row + drow, counter->col + dcol, destination_row, destination_col);
            }
        }
    }

    pthread_mutex_lock(&memory_mutex);
    counter->col = best_col;
    counter->row = best_row;
    pthread_mutex_unlock(&memory_mutex);
}

void go_to_company(counter_int_t * counter, memory_t * memory)
{
	pthread_mutex_lock(&memory_mutex);
	counter->current_state = going_to_company;
	pthread_mutex_unlock(&memory_mutex);

    for (int step = 0; step < 3; ++step) {
        if (counter->row == counter->company_row && counter->col == counter->company_col) {
            break;
        }
        counter_goto(counter, counter->company_col, counter->company_row);
    }
}
