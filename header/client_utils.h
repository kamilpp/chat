#ifndef CLIENT_UTILS_H
#define CLIENT_UTILS_H

#include "main.h"
#include "sys_utils.h"

extern int SERVER_QUEUE_ID;
extern int CLIENT_QUEUE_ID;
//extern int CLIENT_QUEUE_KEY;

extern char NICK[16]; 

standard_message standardMessage;
compact_message compactMessage;
user_list roomListMessage;

// int Snd(void*);
// ssize_t Rcv(void*, long);

int GetMessageID();

int SndStandardMessage(type_t, char[], char[]);
int SndCompactMessage(type_t, int);

ssize_t RcvCompactMessage(type_t);
ssize_t RcvStandardMessage(type_t);

#endif