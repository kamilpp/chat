#include "client.h"

int IsStringEmptyAndTrim(char *);

void Initialize();
void ReadServerAdress(int, char*[]);
void ReadNickNameAndJoinServer();

void debug(char message[]) {
	 PrintfMessage(GetCurrentTime(), "DEBUG", message, ERROR);
}

int main(int argc, char *argv[]) {

	Initialize();
  	InitializeNcurses();
	ReadServerAdress(argc, argv);
	ReadNickNameAndJoinServer();

	int pid = Fork();
  	if (pid) {
  		// PrintfStatusBar();
  		srand(time(0));
		char txt[256];
		while(1) {
			CLEAR(txt);
    		getnstr(txt, 256); 
    		if (IsStringEmptyAndTrim(txt)) {
    			continue;
    		}
    		if (txt[0] == '/') {
				if (!strncmp(txt, "/quit", 5)) {
					SndCompactMessage(MSG_UNREGISTER, 0);
					kill(pid, SIGTERM);
					break;
				} else if (!strncmp(txt, "/list", 5)) {
					SndCompactMessage(MSG_LIST, 0);
				} else if (!strncmp(txt, "/pm", 3)) {
					// SndCompactMessage(MSG_LIST, 0);
				} else if (!strncmp(txt, "/join", 5)) {
					 strcpy(txt, txt+6);
					 PrintfMessage(GetCurrentTime(), "DEBUG", txt, ERROR);
					//SndStandardMessage(MSG_JOIN, strcpy(txt, txt+6));
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
    			SndStandardMessage(MSG_ROOM, txt);
				PrintfMessage(GetCurrentTime(), NICK, txt, MESSAGE_SEND);
    		}
  			// PrintfStatusBar();
		}   

  	} else {
		// execlp("ls", "ls", NULL);
			// PrintfMessage(GetCurrentTime(), NICK, "asdf", MESSAGE_SEND);
  		while (1) {
  			if (Msgrcv(CLIENT_QUEUE_ID, &roomListMessage, sizeof(roomListMessage) + 1, MSG_LIST, IPC_NOWAIT) > 0) {
				
				char userList[MAXX] = {0};
				for (int i = 0; i < MAX_SERVER_COUNT; ++i) {
					if (strlen(roomListMessage.content.list[i])) {
						strcpy(userList + strlen(userList), " <");
						strncpy(userList + strlen(userList), roomListMessage.content.list[i], strlen(roomListMessage.content.list[i]));
						strcpy(userList + strlen(userList), ">");
					} else {
						break;
					}
				}
				
				char help[100 + MAX_ROOM_NAME_LENGTH] = {0};
				strcpy(help, "Users in room ");
				strcpy(help + strlen(help), room);
				strcpy(help + strlen(help), ": ");
				
				PrintfMessage(GetCurrentTime(), "LIST", help, INFO);
				PrintfMessage("", "", userList, INFO);
  			}

  			if (RcvStandardMessage(MSG_ROOM) > 0) {
  				PrintfMessage(GetTime(&standardMessage.content.send_date), standardMessage.content.sender, standardMessage.content.message, MESSAGE_GET);
  			}
//
//  			if (RcvCompactMessage(MSG_JOIN) > 0) {
//  				// if (compactMessage.content.status == )
//  			}
  		}
		return 0; 		
		// while(1);  		
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

	CLIENT_QUEUE_ID = Msgget(IPC_PRIVATE, 0600 | IPC_CREAT);
	while (Msgrcv(CLIENT_QUEUE_ID, &roomListMessage, sizeof(roomListMessage), 0, IPC_NOWAIT) != -1);
}

/**
 * Reads hexadecimal server adress and save it to SERVER_QUEUE_ID as integer.
 */
void ReadServerAdress(int argc, char *argv[]) {

	/**
	 * Read server adress or copy it from arguments if exists.
	 */	
  	char serverAdress[10] = {0};
  	if (argc < 2) {
		PrintfMessage(GetCurrentTime(), "CONFIG", "Type server queue id...", CONFIG);
  		getstr(serverAdress);  		
  	} else {
  		strcpy(serverAdress, argv[1]);
  	}

	/**
	 * Convert hexadecimal server adress into binary.
	 */
  	SERVER_QUEUE_ID = (int)strtol(serverAdress, NULL, 10);
	SERVER_QUEUE_ID = Msgget(SERVER_QUEUE_ID, 0600 | IPC_CREAT);

	PrintfMessage(GetCurrentTime(), "INFO", "Server address saved.", INFO);
}

void ReadNickNameAndJoinServer() {
  	do {
		PrintfMessage(GetCurrentTime(), "CONFIG", "Type your nickname...", CONFIG);
  		CLEAR(NICK);
	  	getstr(NICK);  	

		PrintfMessage(GetCurrentTime(), "INFO", "Connecting to server... Checking nickname...", INFO);
		
		SndCompactMessage(MSG_REGISTER, CLIENT_QUEUE_ID);
		Msgrcv(CLIENT_QUEUE_ID, &compactMessage, sizeof(compactMessage) + 1, 0, 0);

		if (compactMessage.content.value == 0) {
			connectionStatus = true;
			PrintfMessage(GetCurrentTime(), "INFO", "Connection established. Nickname saved.", INFO);
		} else {
			PrintfMessage(GetCurrentTime(), "ERROR", "Nickname already used.", ERROR);
		}
  	} while (compactMessage.content.value);	
}