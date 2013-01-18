#include "server.h"

void ReadMemoryAdress(int, char *[]);
void AttachToIPCUtils();
void DettachToIPCUtils();
void RegisterUser();

int main(int argc, char *argv[]) {

	// signal(SIGINT, SIG_IGN);
	// signal(3, SIG_IGN);
	// signal(20, SIG_IGN);
	
	ReadMemoryAdress(argc, argv);
	AttachToIPCUtils();
	
	int pid = Fork();
	if (pid) {
		char symbol;
		printf("Press q to terminate.\n");
		do {
			scanf("%c", &symbol);
		} while(symbol != 'q');
		kill(pid, SIGTERM);
	} else {
		if (Fork()) {
			while (1) {
				if (RcvCompactMessage(MSG_REGISTER) > 0) {
					if (!Fork()) {
						Printf("Registering user\n");
						RegisterUser();
						return 0;
					}
				}

				while (RcvStandardMessage(MSG_ROOM) > 0) {
					if (!Fork()) {
						serverMessage.type = MSG_SERVER;
						serverMessage.msg = standardMessage;
						P(SERVER);
							for (int i = 0; i < MAX_SERVER_COUNT; ++i) {
								if (MEMORY_POINTER->servers[i].queue_id) {
									Snd(MEMORY_POINTER->servers[i].queue_id, &serverMessage, sizeof(serverMessage));							
								}
							}	
						V(SERVER);
						return 0;
					}
				}
				while (Rcv(&serverMessage, sizeof(serverMessage), MSG_SERVER) > 0) {
					if (!Fork()) {
						P(CLIENT);
							if (serverMessage.msg.type == MSG_ROOM) {
								int MAX = MAX_SERVER_COUNT * MAX_USER_COUNT_PER_SERVER;
								for (int i = 0; i < MAX; ++i) {
									if (MEMORY_POINTER->clients[i].server_queue_id == SERVER_QUEUE_ID && MEMORY_POINTER->clients[i].queue_id && !strcmp(serverMessage.msg.content.sender, MEMORY_POINTER->clients[i].name)) {
										Snd(MEMORY_POINTER->clients[i].queue_id, &(serverMessage.msg), sizeof(standardMessage));
									}
								}
							} else if (serverMessage.msg.type == MSG_PRIVATE) {

							}
						V(CLIENT);
						return 0;
					}
				}
			}
		} else {
			// here send hearbeats
			return 0;
		}
	}

	DettachToIPCUtils();
	return 0;
}

/**
 * Reads hexadecimal shared memory adress and save it to MEMORY_ADRESS as integer.
 */
void ReadMemoryAdress(int argc, char *argv[]) {

	/**
	 * Read memory adress or copy it from arguments if exists.
	 */	
  	char memoryAdress[15] = {0};
  	if (argc < 2) {
		printf("Type shared memory adress: ");
  		scanf("%s", memoryAdress);
  	} else {
  		strcpy(memoryAdress, argv[1]);
  	}

	/**
	 * Convert hexadecimal adress into binary.
	 */
  	MEMORY_ADRESS = (int)strtol(memoryAdress, NULL, 16);
	Printf("Memory adress %d saved.", MEMORY_ADRESS);  	
}

void AttachToIPCUtils() {

	MEMORY_ID = Shmget(MEMORY_ADRESS, sizeof(shm_type), 0600 | IPC_CREAT | IPC_EXCL);
	/**
	 * If segment exists get address and attach to it.
	 * Otherwise attach to it, cleanup memory and create semaphores set.
	 */
	if (MEMORY_ID == -1) {
		MEMORY_ID = Shmget(MEMORY_ADRESS, sizeof(shm_type), 0600);
		Printf("Attached to memory segment, ID = %d", MEMORY_ID);
		MEMORY_POINTER = (shm_type *)Shmat(MEMORY_ID, NULL, 0);

		SEMAPHORES_ID = MEMORY_POINTER->id_semaphores;
	} else {
		Printf("Created memory segment, ID = %d", MEMORY_ID);

		MEMORY_POINTER = (shm_type *)Shmat(MEMORY_ID, NULL, 0);
		memset(MEMORY_POINTER, 0, sizeof(shm_type));

		SEMAPHORES_ID = Semget(IPC_PRIVATE, 3, 0600 | IPC_CREAT);
		MEMORY_POINTER->id_semaphores = SEMAPHORES_ID;
		Printf("Created semaphores, ID = %d", SEMAPHORES_ID);
		Semctl(SEMAPHORES_ID, 0, SETVAL, 1);
		Semctl(SEMAPHORES_ID, 1, SETVAL, 1);
		Semctl(SEMAPHORES_ID, 2, SETVAL, 1);
	}

	SERVER_QUEUE_ID = Msgget(256 + SERVER_NUMBER, 0600 | IPC_CREAT);
	Printf("Server message queue ID is %d", SERVER_QUEUE_ID);
	
	SERVER_NUMBER = -1;
	P(SERVER);
	while (MEMORY_POINTER->servers[++SERVER_NUMBER].queue_id);
	MEMORY_POINTER->servers[SERVER_NUMBER].queue_id = SERVER_QUEUE_ID;
	V(SERVER);
	Printf("Server number is %d", SERVER_NUMBER);
	printf("Server queue number is %d\n", 256 + SERVER_NUMBER);
	
	/**
	 * Read and ignore old messages if exists.
	 */
	// while (Rcv(&roomListMessage, 0) != -1);
}

void DettachToIPCUtils() {

	// if (Semctl(SEMAPHORES_ID, 0, GETPID) == getpid() && Semctl(SEMAPHORES_ID, 0, GETVAL) == 0) {
	// 	V(SEMAPHORES_ID, 0);
	// }
	printf("Detaching from IPC utils\n");

	P(SERVER);
	MEMORY_POINTER->servers[SERVER_NUMBER].queue_id = 0;

	int isLast = 1;
	for (int i = 0; i < 100; ++i) {
		if (MEMORY_POINTER->servers[i].queue_id) {
			isLast = 0;
			break;
		}
	}
	V(SERVER);

	if (isLast) {
		Semctl((int)SEMAPHORES_ID, 0, IPC_RMID, 0);
		shmdt(MEMORY_POINTER);
		Shmctl(MEMORY_ID, IPC_RMID, NULL);
	} else {
		shmdt(MEMORY_POINTER);
	}

	Msgctl(SERVER_QUEUE_ID, IPC_RMID, NULL);
}

void RegisterUser() {

	int MAX = MAX_SERVER_COUNT * MAX_USER_COUNT_PER_SERVER;
	int alreadyExists = 0;
	int j;
	int clientQueueID = compactMessage.content.value;
	
	P(CLIENT);
	for (int i = 0; i < MAX; ++i) {
		if (MEMORY_POINTER->clients[i].queue_id == 0) j = min(i, j);
		if (strcmp(MEMORY_POINTER->clients[i].name, compactMessage.content.sender) == 0) {
			alreadyExists = 1;
			break;
		}
	}

	if (alreadyExists == 0) {
		MEMORY_POINTER->clients[j].queue_id = SERVER_QUEUE_ID;
		MEMORY_POINTER->clients[j].queue_id = clientQueueID;
		strcpy(MEMORY_POINTER->clients[j].name, compactMessage.content.sender);
		strcpy(MEMORY_POINTER->clients[j].room, GLOBAL_ROOM_NAME);
	}
	V(CLIENT);

	SndCompactMessage(compactMessage.content.value, MSG_REGISTER, (alreadyExists) ? -1 : 0, compactMessage.content.id);
	printf("Register user %s %s\n", compactMessage.content.sender, (alreadyExists) ? "failed" : "succeed");
}