#include "client_utils.h"

int messageCounter = 100003;

/*
 * Returns unique message id. Is used by few threads but does not reqire sharing due to diffirent message types.
 */
int GetMessageID() {
	return ((++messageCounter) % (int)10e9);
}
///////////////////////////////////////

int SndPrivateMessage(char message[512], char recipient[MAX_USER_NAME_LENGTH]) {

	CLEAR(standardMessage);
	standardMessage.type = MSG_PRIVATE;
	standardMessage.content.id = GetMessageID();
	time_t rawtime; time(&rawtime);
	standardMessage.content.send_date = rawtime;
	strcpy(standardMessage.content.sender, NICK);
	strcpy(standardMessage.content.recipient, recipient);
	strcpy(standardMessage.content.message, message);

	return Msgsnd(SERVER_QUEUE_ID, &standardMessage, sizeof(standardMessage), 0);
}

int SndStandardMessage(type_t type, char message[512]) {
	
	CLEAR(standardMessage);
	standardMessage.type = type;
	standardMessage.content.id = GetMessageID();
	time_t rawtime; time(&rawtime);
	standardMessage.content.send_date = rawtime;
	strcpy(standardMessage.content.sender, NICK);
	strcpy(standardMessage.content.message, message);

	return Msgsnd(SERVER_QUEUE_ID, &standardMessage, sizeof(standardMessage), 0);
}

int SndCompactMessage(type_t type, int value) {
	
	CLEAR(compactMessage);
	compactMessage.type = type;
	compactMessage.content.value = value;
	strcpy(compactMessage.content.sender, NICK);
	compactMessage.content.id = GetMessageID();

	// printf("Sending to %d\n", SERVER_QUEUE_ID);
	return Msgsnd(SERVER_QUEUE_ID, &compactMessage, sizeof(compactMessage), 0);
}

ssize_t RcvCompactMessage(type_t type) {
	
	CLEAR(compactMessage);
	return Msgrcv(CLIENT_QUEUE_ID, &compactMessage, sizeof(compactMessage), type, IPC_NOWAIT);
}

ssize_t RcvStandardMessage(type_t type) {
	
	CLEAR(standardMessage);
	return Msgrcv(CLIENT_QUEUE_ID, &standardMessage, sizeof(standardMessage), type, IPC_NOWAIT);
}

// int SndCompact(type_t type) {

// 	Msgsnd(SERVER_QUEUE_ID, structure, sizeof(structure), 0);
// 	return sizeof(*structure);
// }

// ssize_t Rcv(void *structure, long type) {
// 	CLEAR(structure);
// 	return Msgrcv(CLIENT_QUEUE_ID, structure, sizeof(*structure) + 1, type, IPC_NOWAIT);
// }


char* GetCurrentTime() {

	static char TIME[6];
	time_t rawtime;
	struct tm* timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(TIME, 6, "%H:%M", timeinfo);

	return TIME;
}

char* GetTime(time_t *time) {

	static char TIME[6];
	time_t rawtime = *time;
	struct tm* timeinfo;

	timeinfo = localtime(&rawtime);
	strftime(TIME, 6, "%H:%M", timeinfo);

	return TIME;	
}