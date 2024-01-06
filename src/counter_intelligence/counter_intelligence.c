/*
* ENSICAEN
* 6 Boulevard Maréchal Juin
* F-14050 Caen Cedex
* Projet : A LICENSE TO KILL
*
* Copyright (C) Cécile LU (cecile.lu (at) ecole.ensicaen.fr)
*/

/**
 * \file counter_intelligence.c
 */
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

    counter->company_row = -1;
    counter->company_col = -1;

    counter->mailbox_row = -1;
    counter->mailbox_col = -1;
    counter->mailbox_is_found = 0;
    counter->days_without_finding = 0;
    counter->company_is_found = 0;
    counter->spy_is_spotted = 0;
    counter->find_a_residential_building = 0;
    counter->hour = 0;
    counter->mission_success = 0;
}

void counter_routine(int sig)
{
    memory_t * memory;
	memory = get_data();
    if (memory == NULL) {
        perror("Failed to get shared memory");
        return;
    }

    counter_int_t * counter = &memory->counter_intelligence;
    update_counter(counter, memory);

    if (munmap(memory, sizeof(memory_t)) == -1) {
        perror("Error un-mapping shared memory");
    }
}

void simulation_end(int sig)
{
    return EXIT_SUCCESS;
}

void begin_counter_intelligence_routine()
{
    struct sigaction next_round;
	next_round.sa_handler = &counter_routine;
    next_round.sa_flags = 0;
    if (sigaction(SIGTTIN, &next_round, NULL) == -1) {
        handle_fatal_error("Error using sigaction");
    }

    struct sigaction sim_end;
	sim_end.sa_handler = &simulation_end;
    sim_end.sa_flags = 0;
    if (sigaction(SIGTERM, &sim_end, NULL) == -1) {
        handle_fatal_error("Error using sigaction");
    }
/*
    struct sigaction damage;
	damage.sa_handler = &receive_damage;
    damage.sa_flags = 0;
    if (sigaction(SIGUSR2, &damage, NULL) == -1) {
        handle_fatal_error("Error using sigaction");
    }*/

	while(1) {}
}

void update_counter(counter_int_t * counter, memory_t * memory)
{
    semaphore_t *sem;
    sem = open_semaphore("/spy_semaphore");

    if(memory->hour >= 18 || memory->hour < 8 && counter->company_is_found == 0){
        find_suspiscious_act(counter, memory);
    } else {
        if(memory->hour == 8){
            counter->hour = (rand()%(17 - 8 + 1)) + 8;
        }
        if(counter->mailbox_is_found == 1 && counter->hour == memory->hour){
            if(counter->row != memory->mailbox_row || counter->col != memory->mailbox_col){
                go_to_mailbox(counter, memory);
            } else {
                int spy_present = 0;
                spy_t *spy;
                for (int i = 0; i < NUM_SPIES; i++) {
                    spy = &memory->spies[i];
                    if (spy->row == counter->row && spy->col == counter->col) {
                        spy_present = 1;
                        break;
                    }
                }   
                if (spy_present) {
                    altercation_with_spy(counter, memory, spy);
                } else {
                    //get_message(counter, memory);
                }
            }
        } else if(counter->mailbox_is_found == 0 && counter->hour == memory->hour && counter->days_without_finding > 0){
            //printf("Jour 2 pour aller a la mailbox\n");
            P(sem);
            counter->mission_success = 1;
            V(sem);
            if (counter->row != memory->mailbox_row || counter->col != memory->mailbox_col) {
                go_to_mailbox(counter, memory);
            } else {
                find_mailbox(counter, memory);
            }
        } else if (counter->current_state == going_back_home || counter->current_state == resting_at_home) {
            if (counter->row != counter->home_row || counter->col != counter->home_col) {
                go_back_home(counter, memory);
            } else {
                rest_at_home(counter, memory);
            }
        }
    }
    if(counter->company_is_found == 1 && counter->mission_success == 0 && counter->days_without_finding == 0){
        if((counter->row != counter->company_row || counter->col != counter->company_col) && counter->spy_is_spotted == 0){
            go_to_company(counter, memory);
        } else {
            for (int i = 0; i < NUM_SPIES; i++) {
                spy_t *spy = &memory->spies[i];
                if (spy->col == counter->company_col && spy->row == counter->company_row) {
                    P(sem);
                    counter->target_id = spy->id;
                    counter->spy_is_spotted = 1;
                    V(sem);
                    break;
                }
            }
        }
        if(counter->spy_is_spotted == 1 && counter->find_a_residential_building == 0){
            follow_counter(counter, memory);
        } else if(counter->find_a_residential_building == 1 && counter->mailbox_is_found == 0){
            find_mailbox(counter, memory);
            if(counter->mailbox_is_found == 1){
                //get_message(counter, memory);
            }
        } else if (counter->mailbox_is_found == 1) {
            if (counter->row != counter->home_row || counter->col != counter->home_col) {
                go_back_home(counter, memory);
            } else {
                P(sem);
                counter->mission_success = 1;
                V(sem);
                rest_at_home(counter, memory);
            }
        }
    }
}

void find_suspiscious_act(counter_int_t * counter, memory_t *memory)
{
    semaphore_t *sem;
    sem = open_semaphore("/spy_semaphore");
    
    for (int i = 0; i < NUM_COMPANIES; i++) {
        company_t *company = &memory->companies[i];

        if (company->nb_of_people != 0) {
            if (DISPLAY == 2) {
                printf("Counter found a suspicious act\n");
            }
            P(sem);
            counter->company_col = company->col;
            counter->company_row = company->row;
            counter->company_is_found = 1;
            counter->current_state = found_suspiscious_act;
            V(sem);
            break;  
        }
    }
}

void follow_counter(counter_int_t *counter, memory_t *memory) 
{
    int target_id = counter->target_id; // L'ID de l'espion à suivre
    spy_t *target_spy = &memory->spies[abs(target_id - 127)];

    semaphore_t *sem;
    sem = open_semaphore("/spy_semaphore");
    P(sem);
    if (DISPLAY == 2) {
        printf("FOLLOWING SPY\n");
    }
    counter->current_state = following_counter;
    V(sem);

    // Utilisez la fonction counter_goto pour se déplacer vers l'espion
    if(counter->col != memory->mailbox_col || counter->row != memory->mailbox_row){
        //printf("going to (%d, %d)\n", target_spy->row, target_spy->col);
        counter_goto(counter, target_spy->col, target_spy->row);
    } else {
        P(sem);
        counter->find_a_residential_building = 1;
        counter->current_state = searching_mailbox;
        V(sem);
    }
}


void find_mailbox(counter_int_t * counter, memory_t * memory)
{
    int chance_to_find = 70;
    int num;
    semaphore_t *sem;
    sem = open_semaphore("/spy_semaphore");

    if (DISPLAY == 2) {
        printf("FINDING MAILBOX\n");
    }

    chance_to_find += 10 * counter->days_without_finding;
    P(sem);
    counter->current_state = searching_mailbox;
    V(sem);
    num =rand()% 100;
    if(memory->mailbox_col == counter->col && memory->mailbox_row == counter->row){
        if (num < chance_to_find) {
            P(sem);
            counter->mailbox_is_found = 1;
            counter->days_without_finding = 0;
            counter->mailbox_col = counter->col;
            counter->mailbox_row = counter->row;
            V(sem);
            //get_message(counter, memory);
        } else {
            if (DISPLAY == 2) {
                printf("Mailbox non trouvée, réessayera demain.\n");
            }
            P(sem);
            counter->mailbox_is_found = 0;
            counter->days_without_finding++;
            counter->current_state = going_back_home;
            V(sem);
        }
    } else {
        P(sem);
        counter->current_state = going_back_home;
        V(sem);
    }
}

void altercation_with_spy(counter_int_t * counter, memory_t * memory, spy_t* spy)
{
    semaphore_t *sem;
    sem = open_semaphore("/spy_semaphore");

    int chance = rand() % 100;

    if(chance < 10) {
        fight_with_spy(counter, memory, spy);
    } else {
        P(sem);
        counter->current_state = run_away;
        V(sem);
        go_back_home(counter,memory);
    }
}

void fight_with_spy(counter_int_t* counter, memory_t * memory, spy_t* spy)
{
    int spy_tid;
    for(int i = 0; i< NUM_SPIES; i++){
        if(spy->id == memory->spy_threads[i].id){
            spy_tid = memory->spy_threads[i].thread;
        }
    }
    pthread_kill(spy_tid, SIGUSR2);
}

void get_message(counter_int_t* counter, memory_t* memory)
{
    if (DISPLAY == 2) {
        printf("\nGet Message\n\n");
    }
    semaphore_t *sem;
    sem = open_semaphore("/spy_semaphore");
    for(int i = 0 ; i < MAILBOX_CAPACITY ; i++){
        if(memory->mailbox_content[i].importance != -1){
            P(sem);
            // get the message
            strcpy(counter->message[i].content, memory->mailbox_content[i].content);
            counter->message[i].importance = memory->mailbox_content[i].importance;

            //remove the message from the mailbox
            strcpy(memory->mailbox_content[i].content, EMPTY);
            memory->mailbox_content[i].importance = -1;
            V(sem);
        }
    }
    P(sem);
    memory->nb_of_messages_in_mailbox = 0;
    counter->current_state = going_back_home;
    V(sem);
}


void go_back_home(counter_int_t * counter, memory_t * memory)
{
    if (DISPLAY == 2) {
        printf("go back home\n");
    }
    counter_goto(counter, counter->home_col, counter->home_row);
	
    semaphore_t *sem;
    sem = open_semaphore("/spy_semaphore");
    P(sem);
	counter->current_state = going_back_home;
    V(sem);

}

void rest_at_home(counter_int_t *counter, memory_t *memory)
{
    if (DISPLAY == 2) {
        printf("rest at home\n");
    }

    semaphore_t *sem;
    sem = open_semaphore("/spy_semaphore");
    P(sem);
	counter->current_state = resting_at_home;
	V(sem);
}

void go_to_mailbox(counter_int_t * counter, memory_t * memory)
{   
    if (DISPLAY == 2) {
        printf("go to mailbox (%d, %d)\n", memory->mailbox_row, memory->mailbox_col);
    }
    counter_goto(counter, memory->mailbox_col, memory->mailbox_row);
	
    semaphore_t *sem;
    sem = open_semaphore("/spy_semaphore");
    P(sem);
    counter->current_state = going_to_mail_box;
    V(sem);
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

    semaphore_t *sem;
    sem = open_semaphore("/spy_semaphore");
    P(sem);
    counter->col = best_col;
    counter->row = best_row;
    V(sem);
}

void go_to_company(counter_int_t * counter, memory_t * memory)
{
	semaphore_t *sem;
    sem = open_semaphore("/spy_semaphore");
    P(sem);
	counter->current_state = going_to_company;
	V(sem);

    for (int step = 0; step < 3; ++step) {
        if (counter->row == counter->company_row && counter->col == counter->company_col) {
            P(sem);
            counter->current_state = hiding;
            V(sem);
            break;
        }
        counter_goto(counter, counter->company_col, counter->company_row);
    }
}
