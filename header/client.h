#ifndef CLIENT_H
#define CLIENT_H

#include "main.h"
#include "client_utils.h"
#include "client_ui.h"

int SERVER_QUEUE_ID;
int CLIENT_QUEUE_ID;
char NICK[16];

char room[16];
int connectionStatus;

extern standard_message standardMessage;
extern compact_message compactMessage;
extern user_list roomListMessage;

#endif