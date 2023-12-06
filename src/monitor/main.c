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

#include <stdlib.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <semaphore.h>
typedef sem_t semaphore_t;

#include "monitor.h"
#include "monitor_common.h"
#include "memory.h"


extern WINDOW *main_window;
extern int old_cursor;

/**
 * \file main.c
 *
 * \brief Creates a new "Terminal User Interface" using ncurses and associated
 * with the given shared memory.
 *
 * The size of the terminal must have at least 40 rows and 145 columns. It
 * is decomposed in four sub-windows:
 * - The "city map and status" window (upper-left)
 * - The "character information" window (upper-right)
 * - The "mailbox content" window (bottom-left)
 * - The "enemy country monitor" window (bottom-right)
 * 'Q', 'q' and 'Esc' keys are used to exit from the TUI.
 */


semaphore_t *open_semaphore(char *name)
{
    semaphore_t *sem = NULL;

    sem = sem_open(name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 0);
    sem_init(sem,1,1);
    if (sem == SEM_FAILED) {
        sem_unlink(name); /* Try to unlink it */
        handle_fatal_error("Error [sem_unlink()]: ");
    }
    return sem;
}

void P(semaphore_t *sem)
{
    int r = 0;
    
    r = sem_wait(sem);
    if (r < 0) {
        handle_fatal_error("Error [P()]: ");
    }
}


void V(semaphore_t *sem)
{
    int r = 0;
    
    r = sem_post(sem);
    if (r < 0) {
        handle_fatal_error("Error [V()]: ");
    }
}


void handle_fatal_error(const char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

memory_t* get_data(){
    int fd;
    memory_t*ptr=malloc(sizeof(memory_t));
    fd = shm_open("/spy_memory", O_RDWR, 0666);
    if(fd==-1){
        printf("error");
    }
    ptr = (memory_t*) mmap(NULL, sizeof(memory_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    return ptr;
}

int main(int argc, char **argv)
{
    int rows;
    int cols;
    int key;
    memory_t *memory;
    monitor_t *monitor;
        

    /* ---------------------------------------------------------------------- */ 
    /* The following code only allows to avoid segmentation fault !           */ 
    /* Change it to access to the real shared memory.                         */
    semaphore_t *sem;
    sem = open_semaphore("/spy_semaphore");
    P(sem);
    memory = get_data();
    
    memory->memory_has_changed =  1;
    fprintf(stderr,"%d",sizeof(memory));
    V(sem);
    /* ---------------------------------------------------------------------- */ 

    monitor = (monitor_t *)malloc(sizeof(monitor_t));
    monitor->has_to_update = 0;

    set_timer();
    set_signals();

    if ((main_window = initscr()) == NULL) {
        quit_after_error("Error initializing library ncurses!");
    }

    clear();                  /* Start from an empty screen. */
    cbreak();                 /* No line buffering, pass every key pressed. */
    noecho();                 /* Do not echo the keyboard input. */
    old_cursor = curs_set(0); /* Use and invisible cursor. */
    keypad(stdscr, TRUE);     /* Allows functions keys, arrows, etc. */

    start_color();            /* Allow using colors... */
    create_color_pairs();     /* ... and create color pairs to use */

    if (!is_terminal_size_larger_enough(&rows, &cols)) {
        quit_after_error("Minimal terminal dimensions: 45 rows and 140 columns!");
    }

    /* Initialize terminal user interface elements */
    init_monitor_elements(main_window, memory, rows, cols);

    /*  Loop and get user input  */
    while (true) {
        key = getch();

        switch (key) {
            case 'Q':
            case 'q':
            case 27:
                quit_nicely(NO_PARTICULAR_REASON);
            default:
                break;
        }

        if (memory->memory_has_changed) {
            P(sem);
            update_values(memory);
            memory->memory_has_changed = 0;
            V(sem);
        }
        
    }

     

}

