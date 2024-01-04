#include "enemy_country.h"

void receive_message_from_enemy_spy_network_and_update(memory_t *mem){
    int n,priority;
    mqd_t mq;
    struct mq_attr attr;
    char *buffer=NULL;
    
    mq=mq_open("/file",O_RDONLY);
    mq_getattr(mq,&attr);
    buffer=malloc(attr.mq_msgsize);
    mem->message_enemy_country=malloc(attr.mq_msgsize);
    n=mq_receive(mq,buffer,attr.mq_msgsize,&priority);
    mem->message_enemy_country=buffer;
    mem->memory_has_changed = 1;
    
}
