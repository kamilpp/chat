#include "sys_utils.h"

// TODO V i P


// void *e_malloc(size_t size) 
// {
// 	void *x;
// 	if ((x = malloc(size)) == NULL) {
// 		printf("Error on malloc");
// 		exit(-1);
// 	}
// 	return x;
// }

// ssize_t e_read(int fd, void * buffer, size_t count)
// {
// 	ssize_t n = read(fd, buffer, count);
// 	if( n < 0 ) {
// 		perror("Error on read");
// 		exit(-1);
// 	}
// 	return n;
// }

// ssize_t e_write(int fd, const void * buffer, size_t count)
// {
// 	ssize_t n = write(fd, buffer, count);
// 	if( n < 0 ) {
// 		perror("Error on write");
// 		exit(-1);
// 	}
// 	return n;
// }

// int e_open(const char *pathname, int flags, mode_t mode)
// {
// 	int n = open(pathname, flags, mode);
// 	if( n < 0 ) {
// 		perror("Error on open");
// 		exit(-1);
// 	}
// 	return n;
// }

// int e_creat(const char *pathname, mode_t mode)
// {
// 	int n = creat(pathname, mode);
// 	if( n < 0 ) {
// 		perror("Error on create");
// 		exit(-1);
// 	}
// 	return n;
// }

// int e_close(int fd)
// {
// 	int n = close(fd);
// 	if( n < 0 ) {
// 		perror("Error on close");
// 		exit(-1);
// 	}
// 	return n;
// }

// int e_unlink(const char *pathname)
// {
// 	int n = unlink(pathname);
// 	if( n < 0 ) {
// 		perror("Error on unlink");
// 		exit(-1);
// 	}
// 	return n;
// }

// off_t e_lseek(int fd, off_t offset, int whence)
// {
// 	off_t n = lseek(fd, offset, whence);
// 	if( n < 0 ) {
// 		perror("Error on lseek");
// 		exit(-1);
// 	}
// 	return n;
// }

int Fork() {
	pid_t x = fork();
	if (x < 0) {
		Error("Error on fork");
	}
	return x;
}

// void e_pipe(int pipefd[2]) {
// 	if (pipe(pipefd) < 0) {
// 		printf("Error on pipe");
// 		exit(-1);
// 	}
// }

// void e_dup2(int a, int b) {
// 	if (dup2(a,b) < 0) {
// 		printf("Error od dup2");
// 		exit(-1);
// 	}
// }

//MHM... how do it?
// void e_execlp(const char *file, const char *arg, ...) {
// 	va_list argp;
// 	va_start(argp, arg);

// 	if (execlp(file, arg, argp) < 0) {
// 		printf("Error on execlp");
// 		exit(-1);
// 	}
// 	va_end(argp);
// }

int Msgget(key_t key, int msgflg) {
	int x = msgget(key, msgflg);
	if (x < 0) {
		// int err = errno;
		// if (err == ENOENT) Error("Program error (msgget)"); // nie istnieje
		// if (err == EIDRM)  Error("Program error (msgget)"); // przeznaczona do usuniecia
		Error("Error on msgget");
	} 
	return x;
}

int Msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg) {
	int x = msgsnd(msqid, msgp, msgsz + 1, msgflg);
	if (x < 0) {
		int err = errno;
		// if (err == EAGAIN) return x;
		if (err == EINVAL) Error("Wrong parameters [id?] (msgsnd)"); // bledne parametry
		if (err == EIDRM)  Error("Queue deleted! (msgsnd)"); // przeznaczona do usuniecia
		Error("Error on msgsnd");
	}
	return x;
}

ssize_t Msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg) {
	ssize_t x = msgrcv(msqid, msgp, msgsz + 2, msgtyp, msgflg);
	if (x < 0) {
		int err = errno;
		// if (err == EAGAIN) return x;
		if (err == ENOMSG) return -1;
		if (err == EINVAL) Error("Wrong parameters [id?] (msgrv)"); // bledne parametry
		if (err == EIDRM)  Error("Queue deleted! (msgrcv)"); // przeznaczona do usuniecia
		printf("EEEEEEEEEROOOOOR id = %d, size = %d, type = %d, flags = %d\n", msqid, (int)msgsz, (int)msgtyp, (int)msgflg);
		Error("Error on msgrcv");
	}
	// printf("LOOOL222 %d\n", msqid);

	return x;
}

int Msgctl(int msqid, int cmd, struct msqid_ds *buf) {
	int x = msgctl(msqid, cmd, buf);
	if (x < 0) {
		int err = errno;
		// if (err == EAGAIN) return x;
		if (err == EINVAL) Error("Program error (msgctl)");  // bledne parametry
		if (err == EIDRM)  Error("Program error (msgctl)");  // przeznaczona do usuniecia ??
		if (err == EFAULT) Error("Program error (msgctl)");  // bledny wskaznik na strukture
		Error("Error on msgctl");
	}
	return x;
}

int Shmget(key_t key, size_t size, int shmflg) {
	int x = shmget(key, size, shmflg);
	if (x < 0) {
		int err = errno;
		if (err == EEXIST) return -1;
		perror("TRalalla");
		Error("Error on shmget");
	}
	return x;
}

void* Shmat(int shmid, char *shmaddr, int shmflg) {
	void *wsk = shmat(shmid, shmaddr, shmflg);
	if (wsk == (char *)-1) {
		Error("Error on shmat");
	}
	return wsk;
}

int Shmctl(int shmid, int cmd, struct shmid_ds *buf) {
	int x = shmctl(shmid, cmd, buf);
	if (x < 0) {
		Error("Error on shmctl");
	}
	return x;
}

int Semget(key_t key, int nsems, int semflg) {
	int x = semget(key, nsems, semflg);
	if (x < 0) {
		Error("Error on semget");
	}
	return x;
}

int Semctl(int semid, int semnum, int cmd, int pom) {
	int x = semctl(semid, semnum, cmd, pom);
	if (x < 0) {
		perror("");
		Error("Error on semctl");
	}
	return x;
}

int Semop(int semid, struct sembuf *sops, unsigned nsops) {
	int x = semop(semid, sops, nsops);
	if (x < 0) {
		Error("Error on semop");
	}
	return x;
}

void v(int semid, int semnum) {
	struct sembuf task;
	task.sem_num = semnum;
	task.sem_op = 1;
	task.sem_flg = 0;
	Semop(semid, &task, 1);
}

void p(int semid, int semnum) {
	struct sembuf task;
	task.sem_num = semnum;
	task.sem_op = -1;
	task.sem_flg = 0;
	Semop(semid, &task, 1);
}
