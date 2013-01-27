#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H

#include "main.h"
#include "sys_utils.h"

#define DEBUG 1

extern int SERVER_QUEUE_ID;
extern int SEMAPHORES_ID;

standard_message standardMessage;
compact_message compactMessage;
user_list roomListMessage;
server_message serverMessage;

int SndCompactMessage(int, type_t, int, int);
ssize_t RcvHeartBeat();
ssize_t RcvCompactMessage(type_t);
ssize_t RcvStandardMessage(type_t);

int Snd(int, void*, int);
ssize_t Rcv(void*, int, long);
void V(int);
void P(int);
int GetMessageID();


#define Printf(...) \
		if (DEBUG) { \
			printf("    "); \
			printf(__VA_ARGS__); \
			printf("\n"); \
			fflush(stdout); \
		}

#define Printf2(...) \
		if (DEBUG) { \
			printf("	"); \
			Printf(__VA_ARGS__); \
		}

#endif