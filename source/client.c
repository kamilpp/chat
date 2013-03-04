#include "client.h"

int IsStringEmptyAndTrim(char *);

void Initialize();
void ReadServerAdress(int, char*[]);
void ReadNickNameAndJoinServer();

void debug(char message[]) {
	 PrintfMessage(GetCurrentTime(), "DEBUG", message, ERROR);
}

void signalJoinRoom() {
//	printf("%d %d \n", compactMessage.content.id, roomRequestedMessageID);
//	if (compactMessage.content.id == roomRequestedMessageID) { 
	strcpy(room, roomRequested);
	PrintfMessage(GetCurrentTime(), "INFO", "You have changed room.", INFO);
//	}
}

void signalRedraw() {
	PrintfStatusBar();
	CleanInputBar();
}

int main(int argc, char *argv[]) {

	Initialize();
  	InitializeNcurses();
	ReadServerAdress(argc, argv);
	ReadNickNameAndJoinServer();

	int pid = Fork();
  	if (pid) {
		/**
		 * GUI process.
         */
		char txt[512]; // for reading user messages
		
		while(1) {
			signal(SIGUSR1, signalJoinRoom); // c99 standard reset handlers after using...
			signal(SIGUSR2, signalRedraw); 

			CLEAR(txt);
    		getnstr(txt, 512); 

    		PrintfStatusBar();
    		CleanInputBar();

    		if (IsStringEmptyAndTrim(txt)) {
    			continue;
    		}
    		if (txt[0] == '/') { 
				if (!strncmp(txt, "/quit", 5) ||
					!strcmp(txt, "/q")) {
					SndCompactMessage(MSG_UNREGISTER, 0);
					kill(pid, SIGTERM);
					break;
				} else if (!strncmp(txt, "/list", 5)) {
					 // PrintfMessage(GetCurrentTime(), "DEBUG", NICK, ERROR);
					SndCompactMessage(MSG_LIST, 0);
				} else if (!strncmp(txt, "/pm", 3)) {
					char recipient[MAX_USER_NAME_LENGTH] = {0};
					
					strcpy(txt, txt+4); // trim /pm

					if (IsStringEmptyAndTrim(txt) || strstr(txt, " ") == 0) {
						PrintfMessage(GetCurrentTime(), "INFO", "Wrong parameters! See /help.", INFO);
						continue;
					}

					strncpy(recipient, txt, strlen(txt) - strlen(strstr(txt, " ")));
					strcpy(txt, strstr(txt, " "));
					
					if (IsStringEmptyAndTrim(txt)) {
						PrintfMessage(GetCurrentTime(), "INFO", "Wrong parameters! See /help.", INFO);
						continue;
					}

					char tmp[512 + MAX_USER_NAME_LENGTH] = {'[', 0};
					strcpy(tmp + 1, recipient);
					strcpy(tmp + strlen(tmp), "] ");
					strcpy(tmp + strlen(tmp), txt);

					PrintfMessage(GetCurrentTime(), recipient, tmp, PRIVATE_MESSAGE_SEND);
					SndStandardMessage(MSG_PRIVATE, txt, recipient);
				} else if (!strncmp(txt, "/join", 5)) {
					strcpy(txt, txt+6);
					// PrintfMessage(GetCurrentTime(), "DEBUG", txt, ERROR);
					if (IsStringEmptyAndTrim(txt)) {
						PrintfMessage(GetCurrentTime(), "INFO", "Wrong parameters! See /help.", INFO);
						continue;
					}
					strcpy(roomRequested, txt);
					roomRequestedMessageID = SndStandardMessage(MSG_JOIN, txt, "");
				} else if (!strncmp(txt, "/help", 5)) {
					PrintfMessage(GetCurrentTime(), "HELP", "USE:", INFO);
					PrintfMessage("", "", "/join [room_name] - to enter a room,", INFO);
					PrintfMessage("", "", "/pm [nick_name] [message] - to send private message,", INFO);
					PrintfMessage("", "", "/list - to display user list,", INFO);
					PrintfMessage("", "", "/quit - to terminate chat.", INFO);
				} else {
					PrintfMessage(GetCurrentTime(), "INFO", "Command not found. Use /help to get some info.", INFO);
				}
    		} else {
    			SndStandardMessage(MSG_ROOM, txt, room);
				PrintfMessage(GetCurrentTime(), NICK, txt, MESSAGE_SEND);
    		}
  			// PrintfStatusBar();
		}   

  	} else {
		int parentID = getppid();
		
		if (fork()) {
			while (1) {

	  			if (Msgrcv(CLIENT_QUEUE_ID, &roomListMessage, sizeof(roomListMessage), MSG_LIST, IPC_NOWAIT) > 0) {
					char userList[MAX_CLIENTS] = {0};
					for (int i = 0; i < MAX_USER_LIST_LENGTH; ++i) {
						if (strlen(roomListMessage.content.list[i])) {
							strcpy(userList + strlen(userList), " <");
							strncpy(userList + strlen(userList), roomListMessage.content.list[i], strlen(roomListMessage.content.list[i]));
							strcpy(userList + strlen(userList), ">");
						} else {
							break;
						}
					}
					
					char help[100 + MAX_ROOM_NAME_LENGTH] = {0};
					strcpy(help, "Users in room: ");
//					strcpy(help + strlen(help), room);
//					strcpy(help + strlen(help), ": ");
					
					PrintfMessage(GetCurrentTime(), "LIST", help, INFO);
					PrintfMessage("", "", userList, INFO);

					kill(parentID, SIGUSR2); // send signal to join room

					// PrintfStatusBar();
					// CleanInputBar();
	  			}

				if (RcvStandardMessage(MSG_ROOM) > 0) {
					PrintfMessage(GetTime(&standardMessage.content.send_date), standardMessage.content.sender, standardMessage.content.message, MESSAGE_GET);
					kill(parentID, SIGUSR2); // send signal to join room
				}
				
				if (RcvStandardMessage(MSG_PRIVATE) > 0) {
					PrintfMessage(GetTime(&standardMessage.content.send_date), standardMessage.content.sender, standardMessage.content.message, PRIVATE_MESSAGE_GET);
					kill(parentID, SIGUSR2); // send signal to join room
				}
				
				if (RcvCompactMessage(MSG_JOIN) > 0) {
					//debug("Recivied message join");
					kill(parentID, SIGUSR1); // send signal to join room
				}
			} 
  		} else {
			/**
			 * Heartbeat process.
			 */
			int status;
			
			while (1) {
				status = RcvHeartBeat();
				if (status > 0) {
//					debug("Recieved hearbeat");
					SndHeartBeat(compactMessage.content.id);
				}
				
				if (status == -1) {
					return 0;
				}
			}
		} /* Heartbeat process.*/
		
		return 0; 			
  	}

	TerminateNcurses();
	Msgctl(CLIENT_QUEUE_ID, IPC_RMID, NULL);

	return 0;
}

int IsStringEmptyAndTrim(char *str) {
	char *begin = str;
	char *end;

	// Trim leading space
	while (isspace(*str)) str++;

	if (*str == 0) { //all spaces
		return 1;
	}

	// Trim trailing space
	end = str + strlen(str) - 1;
	while(end > str && isspace(*end)) end--;

	// Write new null terminator
	*(end+1) = 0;

	strcpy(begin, str);	
	return 0;
}

void Initialize() {

	// signal(SIGINT, SIG_IGN);
	// signal(3, SIG_IGN);
	// signal(20, SIG_IGN);
	
	srand(time(0));
	
	do {
		CLIENT_QUEUE_KEY = Random();
		CLIENT_QUEUE_ID = Msgget(CLIENT_QUEUE_KEY, 0777 | IPC_CREAT | IPC_EXCL);
	} while (CLIENT_QUEUE_ID == -1);
}

/**
 * Reads hexadecimal server adress and save it to SERVER_QUEUE_ID as integer.
 */
void ReadServerAdress(int argc, char *argv[]) {

	/**
	 * Read server adress or copy it from arguments if exists.
	 */	
  	char serverQueueKey[10] = {0};
  	if (argc < 2) {
		PrintfMessage(GetCurrentTime(), "CONFIG", "Type server queue identifier...", CONFIG);
  		getstr(serverQueueKey);  		
  	} else {
  		strcpy(serverQueueKey, argv[1]);
  	}

	/**
	 * Convert hexadecimal server adress into binary.
	 */
	while (1) {
		SERVER_QUEUE_ID = Msgget((int)strtol(serverQueueKey, NULL, 10), 0777 | IPC_CREAT | IPC_EXCL);
		
		if (SERVER_QUEUE_ID != -1) {
			PrintfMessage(GetCurrentTime(), "ERROR", "Queue with typed key does not exists!.", ERROR);
			PrintfMessage(GetCurrentTime(), "CONFIG", "Type server queue identifier...", CONFIG);
			getstr(serverQueueKey); 
		} else {
			SERVER_QUEUE_ID = Msgget((int)strtol(serverQueueKey, NULL, 10), 0777);
			break;
		}
	} 

	PrintfMessage(GetCurrentTime(), "INFO", "Server indentifier saved.", INFO);
}

void ReadNickNameAndJoinServer() {
  	do {
		PrintfMessage(GetCurrentTime(), "CONFIG", "Type your nickname...", CONFIG);
  		CLEAR(NICK);
	  	getstr(NICK);  	

		PrintfMessage(GetCurrentTime(), "INFO", "Connecting to server... Checking nickname...", INFO);
		
		SndCompactMessage(MSG_REGISTER, CLIENT_QUEUE_KEY);
		Msgrcv(CLIENT_QUEUE_ID, &compactMessage, sizeof(compactMessage) + 1, 0, 0);

		if (compactMessage.content.value == 0) {
			connectionStatus = true;
			PrintfMessage(GetCurrentTime(), "INFO", "Connection established. Nickname saved.", INFO);
			PrintfMessage(GetCurrentTime(), "INFO", "If you need help, type /help.", INFO);
		} else {
			PrintfMessage(GetCurrentTime(), "ERROR", "Nickname already used.", ERROR);
		}
  	} while (compactMessage.content.value);	
}