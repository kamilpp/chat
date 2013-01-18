#ifndef SERVER_H
#define SERVER_H

#include "main.h"
#include "sys_utils.h"
#include "server_utils.h"

int MEMORY_ADRESS;
int MEMORY_ID;
shm_type *MEMORY_POINTER;
int SERVER_QUEUE_ID;
int SEMAPHORES_ID;
int SERVER_NUMBER;

extern standard_message standardMessage;
extern compact_message compactMessage;
extern user_list userListMessage;
extern server_message serverMessage;

#endif