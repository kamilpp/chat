#ifndef SERVER_H
#define SERVER_H

#include "main.h"
#include "sys_utils.h"
#include "server_utils.h"

int MEMORY_ADDRESS;
int MEMORY_ID;
shm_type *MEMORY_POINTER;
int SERVER_QUEUE_KEY;
int SERVER_QUEUE_ID;
int SEMAPHORES_ID;
int SERVER_NUMBER;

extern standard_message standardMessage;
extern compact_message compactMessage;
<<<<<<< HEAD
extern user_list roomListMessage;
=======
extern room_list roomListMessage;
>>>>>>> master
extern server_message serverMessage;
extern int messageCounter;

#endif