#include "sys_utils.h"

int Fork() {
	pid_t x = fork();
	if (x < 0) {
		Error("Error on fork");
	}
	return x;
}


int Msgget(key_t key, int msgflg) {
	int x = msgget(key, msgflg);
	if (x < 0) {
		int err = errno;
		if (err == EEXIST) return -1;
		if (err == ENOENT) return -1;
		// int err = errno;
		// if (err == ENOENT) Error("Program error (msgget)"); // nie istnieje
		// if (err == EIDRM)  Error("Program error (msgget)"); // przeznaczona do usuniecia
		Error("Error on msgget");
	} 
	return x;
}

int Msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg) {
	int x = msgsnd(msqid, msgp, msgsz, msgflg);
	if (x < 0) {
		int err = errno;
		// if (err == EAGAIN) return x;
		if (err == EINVAL) Error("Program error (msgsnd)"); // bledne parametry
		if (err == EIDRM)  Error("Program error (msgsnd)"); // przeznaczona do usuniecia
		Error("Error on msgsnd");
	}
	return x;
}

ssize_t Msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg) {
	ssize_t x = msgrcv(msqid, msgp, msgsz + 2, msgtyp, msgflg);
	if (x < 0) {
//		if (!msgflg) return -1; // when killed heartbeat process to avoid error on 
				
		int err = errno;
		// if (err == EAGAIN) return x;
		if (err == ENOMSG) return -1;
		if (err == EINVAL) Error("Program error (msgrcv)"); // bledne parametry
		if (err == EIDRM)  Error("Program error (msgrcv)"); // przeznaczona do usuniecia
		Error("Error on msgrcv");
	}
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
		int err = errno;
		if (err == EEXIST) return -1;
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
