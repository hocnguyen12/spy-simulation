#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "spy_simulation.h"
#include "memory.h"
#include "monitor.h"

/*




Chiedi perchè la shared memory non funziona non viene trovata 






*/
void handle_fatal_error(const char *message)
{
    fprintf(stderr, "%s", message);
    exit(EXIT_FAILURE);
}

memory_t* create_shared_memory() {
    int shm_fd; 
    memory_t* ptr;

    shm_fd = shm_open("/spy_memory", O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shm_fd, sizeof(memory_t)) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    ptr = mmap(0, sizeof(memory_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    return ptr;
}


int main(int argc, char **argv){
    memory_t *memory;
    memory = create_shared_memory(); 
    



    exit(EXIT_SUCCESS);

}
