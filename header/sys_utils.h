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


// #include <sys/stat.h>
// #include <unistd.h>
// #include <fcntl.h>
// #include <time.h>
// #include <stdarg.h>
// #include <signal.h>

// #define MALLOC(typ, ilosc) (typ*)e_malloc((ilosc)*sizeof(typ))
// void *e_malloc(size_t size);

// ssize_t e_read(int fd, void * buffer, size_t count);
// ssize_t e_write(int fd, const void * buffer, size_t count);
// int e_open(const char *pathname, int flags, mode_t mode);
// int e_creat(const char *pathname, mode_t mode);
// int e_close(int fd);
// int e_unlink(const char *pathname);
// off_t e_lseek(int fd, off_t offset, int whence);

int Fork();

// void e_pipe(int pipefd[2]);
// void e_dup2(int a, int b);

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



