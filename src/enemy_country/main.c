#include "enemy_country.h"
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

int main(void){
    int shm_fd; 
    memory_t* memory;

    shm_fd = shm_open("/spy_memory", O_RDWR, 0666);
    if (shm_fd == -1) {
        handle_fatal_error("shm_open()");
    }

    if (ftruncate(shm_fd, sizeof(memory_t)) == -1) {
        handle_fatal_error("ftruncate()");
    }

    memory = mmap(0, sizeof(memory_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (memory == MAP_FAILED) {
        handle_fatal_error("mmap()");
    }
    semaphore_t* sem;
    sem = open_semaphore("/spy_semaphore");
    
    P(sem);
    receive_message_from_enemy_spy_network_and_update(memory);
    printf("\n\n\n%s\n\n\n",memory->message_enemy_country);
    V(sem);

    munmap(memory, sizeof(memory_t));
    close(shm_fd);
    return 0;
}