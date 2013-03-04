#ifndef CLIENT_H
#define CLIENT_H

#include "main.h"
#include "client_utils.h"
#include "client_ui.h"

int SERVER_QUEUE_ID;
int CLIENT_QUEUE_KEY;
int CLIENT_QUEUE_ID;
char NICK[16];

char room[16];
char roomRequested[16];
int roomRequestedMessageID;
int connectionStatus;

extern standard_message standardMessage;
extern compact_message compactMessage;
<<<<<<< HEAD
extern user_list roomListMessage;
=======
extern room_list roomListMessage;
>>>>>>> master

#endif