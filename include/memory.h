/*
 * The License to Kill Project
 *
 * Copyright (C) 1995-2022 Alain Lebret <alain.lebret [at] ensicaen [dot] fr>
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

/**
 * \file memory.h
 *
 * Defines structures and functions used to manipulate our shared memory.
 */
#ifndef MEMORY_H
#define MEMORY_H

#include <pthread.h>

#include "cell.h"
#include "common.h"

// DISPLAY 1 for showing info through monitor
// 0 to display in terminal (printf)
#define DISPLAY 0

#define CIPHER_KEY 5
#define ROUND_TIME "300000"

#define MAILBOX_CAPACITY 3

#define MAX_COLUMNS 7
#define MAX_ROWS 7
#define NUM_CITIZEN 127
#define NUM_SPIES 4

#define NUM_SUPERMARKETS 2
#define NUM_COMPANIES 8
#define NUM_RESIDENTIAL_BUILDINGS 11
#define MAX_NB_OF_MSG_IN_COMPANIES 71

typedef struct coordinates_s coordinates_t;

struct coordinates_s {
    //row, column
    int x, y;
};

/* MAP AND BUILDINGS*/
typedef struct map_s map_t;
typedef struct memory_s memory_t;


/**
 * \brief The city map.
 */
struct map_s {
    int columns;                         /*!< The number of columns of the city map. */
    int rows;                            /*!< The number of rows of the city map.*/
    cell_t cells[MAX_COLUMNS][MAX_ROWS]; /*!< Cells that constitute the city map. */
};

typedef enum {
	BIG,
    MEDIUM,
    SMALL,
    VERY_SMALL
} company_size_t;

typedef struct message_s message_t;

struct message_s {
    int importance; /*
                     * 10 : crucial
                     * 9 : strong
                     * 6 : medium
                     * 3 : low
                     * 2 : very low 
                     */
    char content[MAX_LENGTH_OF_MESSAGE];
};

typedef struct company_s company_t;

struct company_s {
    int index;
    int row, col;
    int nb_employee;
    int nb_of_spy;
    company_size_t size;
    int nb_messages;
    message_t messages[MAX_NB_OF_MSG_IN_COMPANIES];
};

/* CHARACTERS */
typedef enum {
/* general states */
    resting_at_home,      //0
    going_to_company,     //1
    going_to_supermarket, //2
    working,              //3
    doing_some_shopping,  //4
    going_back_home,      //5

/* spy states */
    going_to_spot,        //6
    going_to_steal,       //7
    stealing,             //8
    spotting,             //9
    sending_message,      //10
    going_to_mail_box,     //11

    founded_suspiscious_act, //12
    hide,                // 13
    following_spy,
    run_away
} state_t;

typedef enum {
	ROLE_CITY_HALL,
	ROLE_SUPERMARKET,
	ROLE_COMPANY
} citizen_role_t;

typedef struct citizen_s citizen_t;

typedef struct citizen_s {
    int id;
    citizen_role_t role;
    int col, row;
    int home_col, home_row;
    int work_col, work_row;
    int health;

    state_t current_state;
};

typedef struct spy_s spy_t;

typedef enum{
    NORMAL_SPY,
    SPY_OFFICER
} spy_role_t;

/**
 * \brief Structure of a spy agent.
 */
struct spy_s {
    int id;
    spy_role_t role;
    int health;
    int row, col;
    int home_row, home_col;
    int license; // 0 for false, 1 for true
    state_t current_state;

    //normal spy attributes
    int company_row, company_col;
    int is_spotted;
    int is_stolen;
    int find_company_to_steal;
    int number_round_spotting; // Ne pas depasser 12
    int number_round_stealing; // Le nombre de tour volé
    int flee_round;
    int company_index;
    int hour, day;
    company_t companies_stolen[NUM_COMPANIES];
    int nb_company_stolen;
    message_t stolen_message;

    // case officer attributes
    int first_message_time;
    int second_message_time;
    int shopping_time;
    message_t message[MAILBOX_CAPACITY];
};

typedef struct counter_int_s counter_int_t;

/**
 * \brief Structure of the counter_intelligence.
 */
struct counter_int_s {
    int id;
    spy_role_t role;
    int health;
    int row, col;
    int home_row, home_col;
    int company_row, company_col;
    int mailbox_row, mailbox_col;
    int mailbox_is_founded;
    int days_without_finding;
    int company_is_founded
    state_t current_state;

    int target_id;
};

typedef struct character_thread_s character_thread_t;
/**
 * \brief Structure representing a thread and the id of the citizen it represent.
 */
struct character_thread_s {
    pthread_t thread;
    int id;
};

/* SHARED MEMORY*/
/**
 * \brief Shared memory used by all processes.
 */
struct memory_s {
    int memory_has_changed;    /*!< This flag is set to 1 when the memory has changed. */
    int simulation_has_ended;  /*!< This flag is set to the following values:
                                * - 0: has not ended;
                                * - 1: the spy network has fled. It wins!
                                * - 2: the counterintelligence officer has discovered the mailbox. He wins.
                                * - 3: the counterintelligence officer did not discover the mailbox. The spy network
                                *      wins!
                                */
    int pids[7]; /*
                 * - pids[0]: spy_simulation pid
                 * - pids[1]: timer pid
                 * - pids[2]: citizen_manager
                 * - pids[3]: enemy_spy_network
                 * - pids[4]: counter_intelligence_officer
                 * - pids[5]: enemy_country
                 * - pids[6]: monitor
                 */
    character_thread_t citizen_threads[NUM_CITIZEN];
    character_thread_t spy_threads[NUM_SPIES];

    map_t map;
    coordinates_t city_hall;
    company_t companies[8];
    coordinates_t supermarkets[2];
    coordinates_t residential_buildings[11];

    /*enum cell_type_e map_plan[MAX_ROWS][MAX_COLUMNS] = {
        {WASTELAND, COMPANY, WASTELAND, RESIDENTIAL_BUILDING, COMPANY, WASTELAND, WASTELAND},
        {WASTELAND, SUPERMARKET, WASTELAND, WASTELAND, WASTELAND, RESIDENTIAL_BUILDING, WASTELAND},
        {RESIDENTIAL_BUILDING, COMPANY, WASTELAND, WASTELAND, WASTELAND, WASTELAND, WASTELAND},
        {WASTELAND, WASTELAND, CITY_HALL, WASTELAND, RESIDENTIAL_BUILDING, COMPANY, WASTELAND},
        {COMPANY, RESIDENTIAL_BUILDING, WASTELAND, COMPANY, SUPERMARKET, RESIDENTIAL_BUILDING, RESIDENTIAL_BUILDING},
        {WASTELAND, RESIDENTIAL_BUILDING, WASTELAND, WASTELAND, WASTELAND, WASTELAND, COMPANY},
        {RESIDENTIAL_BUILDING, COMPANY, WASTELAND, RESIDENTIAL_BUILDING, WASTELAND, RESIDENTIAL_BUILDING, WASTELAND}
    };*/

    int current_turn;
    int hour;
    int minute;

    // Characters
    spy_t spies[NUM_SPIES];
    citizen_t citizens[NUM_CITIZEN];
    counter_int_t counter_intelligence;

    int working, at_home, walking, shopping;

    int mailbox_row, mailbox_col; // Mail box coordinates
    int nb_of_messages_in_mailbox;
    message_t mailbox_content[MAILBOX_CAPACITY];

    char * enemy_country_monitor[10];
    int nb_of_messages_received;

};

#endif /* MEMORY_H */
