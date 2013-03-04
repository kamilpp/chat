#ifndef SYS_UTILS_H
#define SYS_UTILS_H
	
#include "main.h"

#include <sys/types.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>

#include <errno.h>
#include <unistd.h>
#include <signal.h>

int Fork();

#define Execlp(...) \
	if (execlp(__VA_ARGS__) < 0) { \
		Error("Error on execlp"); \
	}
	

int Msgget(key_t, int);
int Msgsnd(int, const void*, size_t, int);
ssize_t Msgrcv(int, void *, size_t, long, int);
int Msgctl(int, int, struct msqid_ds*);

int Shmget(key_t, size_t, int);
void* Shmat(int, char*, int);
int Shmctl(int, int, struct shmid_ds*);

int Semget(key_t, int, int);
int Semctl(int, int, int, int);
int Semop(int, struct sembuf*, unsigned);
void v(int, int);
void p(int, int);

#endif



