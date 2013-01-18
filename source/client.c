#include "client.h"

int IsStringEmptyAndTrim(char *);

void Initialize();
void ReadServerAdress(int, char*[]);
void ReadNickNameAndJoinServer();

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
				if (!strcmp(txt, "/quit")) {
					SndCompactMessage(MSG_UNREGISTER, 0);
					kill(pid, SIGTERM);
					break;
				}	
				if (!strcmp(txt, "/list")) {
					SndCompactMessage(MSG_LIST, 0);
				}		
				if (!strncmp(txt, "/pm", 3)) {
					// SndCompactMessage(MSG_LIST, 0);
				}	

				if (!strncmp(txt, "/join", 5)) {
					// SndCompactMessage(MSG_LIST, 0);
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
				// char roomList  				
  				// PrintfMessage(GetCurrentTime(), INFO, roomListMessage., MESSAGE_SEND);
  			}
  		}
		return 0; 		
		// while(1);  		
  	}

	getch();

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
	// while (Rcv(&roomListMessage, 0) != -1);
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