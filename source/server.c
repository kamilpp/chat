#include "server.h"

void AttachToIPCUtils(int argc, char *args[]);
void DettachToIPCUtils();
void RegisterUser();
void UnregisterUser(char[]);
void WriteToLogFile(char []);

int main(int argc, char *argv[]) {

	// signal(SIGINT, SIG_IGN);
	// signal(3, SIG_IGN);
	// signal(20, SIG_IGN);
	signal(SIGCLD, SIG_IGN);
	srand(time(NULL));

	AttachToIPCUtils(argc, argv);
	
	int pid = Fork();
	if (pid) {
		/**
		 * GUI process.
         */
		printf("Press q to terminate.\n");
		
		char symbol;
		do {
			scanf("%c", &symbol);
		} while(symbol != 'q');
		
		Printf("Terminating...");
		kill(pid, SIGTERM); // TODO if time1
	} else {
		if (Fork()) {
			/**
			* Main process.
			*/
			while (1) {
				if (RcvCompactMessage(MSG_REGISTER) > 0) {
					if (!Fork()) {
						RegisterUser();
						return 0;
					}
				}
				if (RcvCompactMessage(MSG_UNREGISTER) > 0) {
					if (!Fork()) {
						UnregisterUser(compactMessage.content.sender);
						return 0;
					}
				}
				
				if (RcvStandardMessage(MSG_ROOM) > 0) {
					if (!Fork()) {
						CLEAR(serverMessage);
						serverMessage.type = MSG_SERVER;
						serverMessage.content.msg = standardMessage;
						P(SERVER);
							for (int i = 0; i < MAX_SERVER_COUNT; ++i) {
								if (MEMORY_POINTER->servers[i].queue_key >= 0) {
									Snd(MEMORY_POINTER->servers[i].queue_key, &serverMessage, sizeof(server_message));							
								}
							}		
						V(SERVER);
						return 0;
					}
				}
				
				if (RcvStandardMessage(MSG_PRIVATE) > 0) {
					if (!Fork()) {
						CLEAR(serverMessage);
						serverMessage.type = MSG_SERVER;
						serverMessage.content.msg = standardMessage;
						P(CLIENT);
							for (int i = 0; i < MAX_CLIENTS; ++i) {
								if (MEMORY_POINTER->clients[i].queue_key != -1 &&
								!strcmp(MEMORY_POINTER->clients[i].name, standardMessage.content.sender)) {
									Snd(MEMORY_POINTER->clients[i].server_queue_key, &serverMessage, sizeof(server_message));	
									break;
								}
							}		
						V(CLIENT);
						return 0;
					}
				}
				
				if (Rcv(&serverMessage, sizeof(server_message), MSG_SERVER) > 0) {
					if (!Fork()) {
						
						P(CLIENT);
							if (serverMessage.content.msg.type == MSG_ROOM) {
								Printf2("Recieved message to room %s", serverMessage.content.msg.content.recipient);
								for (int i = 0; i < MAX_CLIENTS; ++i) {
									if (MEMORY_POINTER->clients[i].queue_key != -1) {
//										
										Printf2("Comparing %d %d and %s != %s", MEMORY_POINTER->clients[i].server_queue_key, SERVER_QUEUE_KEY, serverMessage.content.msg.content.sender, MEMORY_POINTER->clients[i].name)
										if (MEMORY_POINTER->clients[i].server_queue_key == SERVER_QUEUE_KEY && 
										strcmp(serverMessage.content.msg.content.sender, MEMORY_POINTER->clients[i].name) && 
										!strcmp(serverMessage.content.msg.content.recipient, MEMORY_POINTER->clients[i].room)) {
											Snd(MEMORY_POINTER->clients[i].queue_key, &(serverMessage.content.msg), sizeof(standardMessage));
										}
									}
								}
							} else if (serverMessage.content.msg.type == MSG_PRIVATE) {
								Printf2("Recieved private message to user %s", serverMessage.content.msg.content.recipient);
								for (int i = 0; i < MAX_CLIENTS; ++i) {
									if (MEMORY_POINTER->clients[i].queue_key != -1) {
//										
										Printf2("Comparing %d == %d and %s == %s", MEMORY_POINTER->clients[i].server_queue_key, SERVER_QUEUE_KEY, serverMessage.content.msg.content.recipient, MEMORY_POINTER->clients[i].name)
										if (MEMORY_POINTER->clients[i].server_queue_key == SERVER_QUEUE_KEY &&  
										!strcmp(serverMessage.content.msg.content.recipient, MEMORY_POINTER->clients[i].name)) {
											Snd(MEMORY_POINTER->clients[i].queue_key, &(serverMessage.content.msg), sizeof(standardMessage));
										}
									}
								}
							}
						V(CLIENT);
						return 0;
					}
				}
				
				if (RcvCompactMessage(MSG_LIST) > 0) {
					if (!Fork()) {
						Printf2("Sending user list");
						
						int i, j = 0;
						char roomName[MAX_ROOM_NAME_LENGTH] = {0};
						int clientQueueKey;
						CLEAR(roomListMessage);
						
						P(CLIENT);
							for (i = 0; i < MAX_CLIENTS; ++i) {
								if (MEMORY_POINTER->clients[i].queue_key != -1 && 
								!strcmp(MEMORY_POINTER->clients[i].name, compactMessage.content.sender)) {
									strcpy(roomName, MEMORY_POINTER->clients[i].room);
									clientQueueKey = MEMORY_POINTER->clients[i].queue_key;									
									break;
								}
							}
						
							if (i == MAX_CLIENTS) {
								Printf2("Error in function handling MSG_LIST request - client not found.");
								return 0;
							}
						
							for (int i = 0; i < MAX_CLIENTS; ++i) {
								if (MEMORY_POINTER->clients[i].queue_key != -1 && 
								!strcmp(MEMORY_POINTER->clients[i].room, roomName)) {
									strcpy(roomListMessage.content.list[j++], MEMORY_POINTER->clients[i].name); 
								}
							}
						V(CLIENT);
						
						roomListMessage.content.id = compactMessage.content.id;
						roomListMessage.type = MSG_LIST;
						Snd(clientQueueKey, &roomListMessage, sizeof(roomListMessage));
						return 0;
					}
				}
				
				if (RcvStandardMessage(MSG_JOIN) > 0) {
					if (!fork()) {
						
						int clientQueueKey = -1;
						P(CLIENT);
							Printf2("User %s joining room %s", standardMessage.content.sender, standardMessage.content.message);
							for (int i = 0; i < MAX_CLIENTS; ++i) {
								if (MEMORY_POINTER->clients[i].queue_key != -1 && !strcmp(MEMORY_POINTER->clients[i].name, standardMessage.content.sender)) {
									strcpy(MEMORY_POINTER->clients[i].room, standardMessage.content.message);
									clientQueueKey = MEMORY_POINTER->clients[i].queue_key;
									break;
								}
							}
						V(CLIENT);
						
						if (clientQueueKey != -1) {
//							printf("%d\n", standardMessage.content.id);
							SndCompactMessage(clientQueueKey, MSG_JOIN, 0, standardMessage.content.id);
						} else {
							Printf2("Error in MSG_JOIN - user doesn't exits!");
//							Error("Error while handling MSG_JOIN.");
						}
						return 0;
					}
				}
			}
		} else {
			/**
			* Heartbeat process.
			*/
			int j;
			int id[MAX_USER_COUNT_PER_SERVER];
			char username[MAX_USER_COUNT_PER_SERVER][MAX_USER_NAME_LENGTH];
			
			while (1) {
				Printf3("Retrieving heartbeats...");
				
				memset(id, -1, sizeof(id));
				CLEAR(username);
				j = 0;
				
				P(CLIENT);
				for (int i = 0; i < MAX_CLIENTS; ++i) {
					if (MEMORY_POINTER->clients[i].queue_key != -1 &&
					MEMORY_POINTER->clients[i].server_queue_key == SERVER_QUEUE_KEY) {
						id[j] = GetMessageID();
						strcpy(username[j], MEMORY_POINTER->clients[i].name);
						SndCompactMessage(MEMORY_POINTER->clients[i].queue_key, MSG_HEARTBEAT, 0, id[j]);
						j++;
					}
				}
				V(CLIENT);
				
				sleep(2);
				if (getppid() == 1) { // parent process died
					return 0;
				}
				
				while (RcvHeartBeat() > 0) {
					for (int i = 0; i < MAX_USER_COUNT_PER_SERVER; ++i) {
						if (id[i] == compactMessage.content.id) {
							id[i] = -1;
							break;
						} 
					}
				} 
				
				for (int i = 0; i < MAX_USER_COUNT_PER_SERVER; ++i) {
					if (id[i] != -1) {
						if (!fork()) {
							Printf("Heartbeat detected inactive (unregistred) user - %s", username[i]);
							UnregisterUser(username[i]);
							return 0;
						}
					}
				} 
			} /* while 1 */
		} /* Heartbeat process */
	} /* Main process */

	DettachToIPCUtils();
	return 0;
} /* MAIN */

void AttachToIPCUtils(int argc, char *args[]) {

	MEMORY_ID = -1;
	
	/*
	 * Trying to attach shared memory if adress specified.
	 * Otherwise creates it.
	 */
	int created = 0;
	if (argc >= 2) {
		MEMORY_ADDRESS = (int)strtol(args[1], NULL, 10);
		MEMORY_ID = Shmget(MEMORY_ADDRESS, sizeof(shm_type), 0777 | IPC_CREAT | IPC_EXCL);
		
		if (MEMORY_ID != -1)  {
			Shmctl(MEMORY_ID, IPC_RMID, NULL);
			created = 1;
			printf("TYPED MEMORY SEGMENT DOES NOT EXISTS! [address %d]\n", MEMORY_ADDRESS);
		} else {
			MEMORY_ID = Shmget(MEMORY_ADDRESS, sizeof(shm_type), 0777);
			Printf("ATTACHED to memory segment. [adress %d][ID %d]", MEMORY_ADDRESS, MEMORY_ID);
			MEMORY_POINTER = (shm_type *)Shmat(MEMORY_ID, NULL, 0);

			SEMAPHORES_ID = Semget(MEMORY_POINTER->key_semaphores, 3, 0777);
			Printf("ATTACHED to semaphores. [ID %d]", SEMAPHORES_ID)	
		}
  	}
	
	if (created == 1 || argc < 2) {
		created = 1;
		
		int number = 256;
		do {
			MEMORY_ADDRESS = number++;
			MEMORY_ID = Shmget(MEMORY_ADDRESS, sizeof(shm_type), 0777 | IPC_CREAT | IPC_EXCL);
		} while(MEMORY_ID == -1);
	}

	/**
	 * If segment was created - cleanup memory and create semaphores set.
	 */
	if (created == 1) {
		printf("SERVER %d\n", MEMORY_ADDRESS);
		Printf("CREATED memory segment. [adress %d][ID %d]", MEMORY_ADDRESS, MEMORY_ID);

		Printf2("Cleanup memory");
		MEMORY_POINTER = (shm_type *)Shmat(MEMORY_ID, NULL, 0);
		memset(MEMORY_POINTER, 0, sizeof(shm_type));
		for (int i = 0; i < MAX_CLIENTS; ++i) {
			MEMORY_POINTER->clients[i].queue_key = -1;
		}
		for (int i = 0; i < MAX_SERVERS; ++i) {
			MEMORY_POINTER->servers[i].queue_key = -1;
		}
		
		do {
			MEMORY_POINTER->key_semaphores = Random();
			SEMAPHORES_ID = Semget(MEMORY_POINTER->key_semaphores, 3, 0777 | IPC_CREAT);
		} while (SEMAPHORES_ID == -1);
		
		Printf("CREATED semaphores. [key %d][ID %d]", MEMORY_POINTER->key_semaphores, SEMAPHORES_ID);
		Semctl(SEMAPHORES_ID, 0, SETVAL, 1);
		Semctl(SEMAPHORES_ID, 1, SETVAL, 1);
		Semctl(SEMAPHORES_ID, 2, SETVAL, 1);
	}

	/**
	 * Register server in memory.
     */
	SERVER_NUMBER = -1;
	P(SERVER);
		while (MEMORY_POINTER->servers[++SERVER_NUMBER].queue_key != -1);
		
		int number = 256;
		do {
			SERVER_QUEUE_KEY = number++;
			SERVER_QUEUE_ID = Msgget(SERVER_QUEUE_KEY, 0777 | IPC_CREAT | IPC_EXCL);
		} while (SERVER_QUEUE_ID == -1);
		MEMORY_POINTER->servers[SERVER_NUMBER].queue_key = SERVER_QUEUE_KEY;
		printf("CLIENT %d\n", MEMORY_POINTER->servers[SERVER_NUMBER].queue_key);
		Printf("Created server message queue. [key %d][ID %d]", MEMORY_POINTER->servers[SERVER_NUMBER].queue_key, SERVER_QUEUE_ID);
	V(SERVER);
	WriteToLogFile("Registered server.");
}

void DettachToIPCUtils() {

	P(SERVER);
		MEMORY_POINTER->servers[SERVER_NUMBER].queue_key= -1;

		int isLast = 1;
		for (int i = 0; i < MAX_SERVER_COUNT; ++i) {
			if (MEMORY_POINTER->servers[i].queue_key>= 0) {
				isLast = 0;
				break;
			}
		}
	V(SERVER);

	WriteToLogFile("Unregistered server.");
	shmdt(MEMORY_POINTER);
	Msgctl(SERVER_QUEUE_ID, IPC_RMID, NULL);
	if (isLast) {
		Semctl((int)SEMAPHORES_ID, 0, IPC_RMID, 0);
		Shmctl(MEMORY_ID, IPC_RMID, NULL);
	} 

	Printf("Detached to IPC utils\n");
}

void RegisterUser() {

	Printf2("Registering user %s", compactMessage.content.sender);

	int alreadyExists = 0;
	int j = MAX_CLIENTS;
	int clientsOnServer = 0;
	int clientQueueKey = compactMessage.content.value;
	
	P(CLIENT);
		for (int i = 0; i < MAX_CLIENTS; ++i) {
			if (MEMORY_POINTER->clients[i].queue_key == -1) { 
				j = min(i, j); // Remember first free position in tab.
			} else if (!strcmp(MEMORY_POINTER->clients[i].name, compactMessage.content.sender)) {
				alreadyExists = 1;
				break;
			} else if (MEMORY_POINTER->clients[i].server_queue_key == SERVER_QUEUE_KEY) {
				clientsOnServer++;
			}
		}

		if (alreadyExists == 0 && clientsOnServer < MAX_USER_COUNT_PER_SERVER) {
			MEMORY_POINTER->clients[j].server_queue_key = SERVER_QUEUE_KEY;
			MEMORY_POINTER->clients[j].queue_key = clientQueueKey;
			strcpy(MEMORY_POINTER->clients[j].name, compactMessage.content.sender);
			strcpy(MEMORY_POINTER->clients[j].room, GLOBAL_ROOM_NAME);
			
			char tmp[100] = {0}; 
			sprintf(tmp, "Registered user %s.", compactMessage.content.sender);
			WriteToLogFile(tmp);
		}
	V(CLIENT);

	SndCompactMessage(compactMessage.content.value, MSG_REGISTER, (alreadyExists) ? -1 : ((clientsOnServer < MAX_USER_COUNT_PER_SERVER) ? 0 : -2), compactMessage.content.id);
	Printf2("Register user %s %s", compactMessage.content.sender, (!alreadyExists && clientsOnServer < MAX_USER_COUNT_PER_SERVER) ?  "succeed" : "failed");
}

void UnregisterUser(char user[]) {
	
	Printf2("Unregistering user %s", user);
	int i;

	P(CLIENT);
		for (i = 0; i < MAX_CLIENTS; ++i) {
			if (!strcmp(MEMORY_POINTER->clients[i].name, user)) {
				break;
			}
		}

		if (i == MAX_CLIENTS) {
			Printf2("Unregistering failed. User name - %s - not found.", user);
		} else {
			memset(&(MEMORY_POINTER->clients[i]), 0, sizeof(client));
			MEMORY_POINTER->clients[i].queue_key = -1;
			Printf2("Unregistering succeed. User %s unregisterd.", user);
			
			char tmp[100] = {0}; 
			sprintf(tmp, "Unregistered user %s.", user);
			WriteToLogFile(tmp);
		}
	V(CLIENT);
}

void WriteToLogFile(char logMessage[]) {

	char fullLogMessage[1000] = {0};
	sprintf(fullLogMessage, "%s [%d] %s\n", GetCurrentTimeLogFormat(), SERVER_QUEUE_KEY, logMessage);

	P(LOG);
		FILE *log = fopen(LOGFILE, "a");
		fwrite(fullLogMessage, 1, sizeof(fullLogMessage), log);
		fclose(log);
	V(LOG);
}
