#include "enemy_country.h"

void receive_message_from_enemy_spy_network(){
    int n,priority;
    mqd_t mq;
    struct mq_attr attr;
    char *buffer=NULL;
    
    mq=mq_open("/file",O_RDONLY);
    mq_getattr(mq,&attr);
    buffer=malloc(attr.mq_msgsize);
    n=mq_receive(mq,buffer,attr.mq_msgsize,&priority);
    fprintf(stdout,"\n\n\n[%d] %s\n\n\n",priority,buffer);
    exit(EXIT_SUCCESS);
}