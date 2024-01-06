/*
* ENSICAEN
* 6 Boulevard Maréchal Juin
* F-14050 Caen Cedex
* Projet : A LICENSE TO KILL
*
* Copyright (C) Cécile LU (cecile.lu (at) ecole.ensicaen.fr)
*/

/**
 * \file enemy_country.c
 */
#include "enemy_country.h"

void simulation_end(int sig)
{
    mq_unlink("/spy_queue");
}
/*
void receive_msg(memory_t * mem)
{

    int n,priority;
    mqd_t mq;
    struct mq_attr attr;
    char *buffer = NULL;
    
    mq = mq_open("/spy_queue", O_RDONLY);
    mq_getattr(mq, &attr);
    buffer = malloc(attr.mq_msgsize);
    //mem->enemy_country_monitor = malloc(10 * attr.mq_msgsize);

    while (1) {
        n = mq_receive(mq, buffer, attr.mq_msgsize, &priority);
        if (n >= 0) {
            mem->enemy_country_monitor[mem->nb_of_messages_received] = buffer;
            mem->nb_of_messages_received += 1;
            printf("RECEIVED : %s\n", buffer);
        }
    }
}*/

void receive_message_from_enemy_spy_network_and_update()
{
    semaphore_t* sem;
    sem = open_semaphore("/spy_semaphore");
    memory_t * mem = get_data();
    
    struct sigaction end;
    end.sa_handler = &simulation_end;
    end.sa_flags = 0;
    if (sigaction(SIGTERM, &end, NULL) < 0) {
        handle_fatal_error("Error using sigaction");
    }

    // SET UP MQUEUE and RECEIVE MESSAGES
    mqd_t mq;
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_LENGTH_OF_MESSAGE;
    attr.mq_curmsgs = 0;

    mq = mq_open("/spy_queue", O_CREAT | O_RDONLY, 0644, &attr);
    if (mq == (mqd_t) -1) {
        perror("mq_open()");
        exit(EXIT_FAILURE);
    }

    char buffer[MAX_LENGTH_OF_MESSAGE];
    unsigned int priority;

    while (1) {
        
        ssize_t bytes_read;
        
        bytes_read = mq_receive(mq, buffer, MAX_LENGTH_OF_MESSAGE, &priority);
        if (bytes_read >= 0) {
            if (DISPLAY == 0) {
                printf("RECEIVED MSG IN ENEMY COUNTRY\n");
            }
            //buffer[bytes_read] = '\0';
 
            char * decrypted_message = malloc(MAX_LENGTH_OF_MESSAGE);
            strcpy(decrypted_message, caesar_decipher(buffer, CIPHER_KEY));

            if (mem->nb_of_messages_received == 9) {
                //for (int k = 0; k < 10; k++) {
                //    free(mem->enemy_country_monitor[k]);
                //}
                mem->nb_of_messages_received = 0;
            }
            //printf("TEST 1 %s\n", decrypted_message);
            //mem->enemy_country_monitor[mem->nb_of_messages_received] = malloc(MAX_LENGTH_OF_MESSAGE);
            strcpy(mem->enemy_country_monitor[mem->nb_of_messages_received], decrypted_message);
            //printf("TEST 2\n");
            mem->nb_of_messages_received += 1;

            if (DISPLAY == 0) {
                printf("Received decrypted message: %s (Priority: %u)\n", mem->enemy_country_monitor[mem->nb_of_messages_received - 1], priority);  
            } 
            free(decrypted_message);
        } else {
           
        }
    }
}
