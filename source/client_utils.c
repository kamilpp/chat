#include "client_utils.h"

int messageCounter = 100003;

/*
 * Returns unique message id. Is used by few threads but does not reqire sharing due to diffirent message types.
 */
int GetMessageID() {
	return ((++messageCounter) % (int)10e9);
}
///////////////////////////////////////

int SndStandardMessage(type_t type, char message[512], char recipient[MAX_USER_NAME_LENGTH]) {
	
	int id = GetMessageID();

	CLEAR(standardMessage);
	standardMessage.type = type;
	standardMessage.content.id = id;
	time_t rawtime; time(&rawtime);
	standardMessage.content.send_date = rawtime;
	strcpy(standardMessage.content.sender, NICK);
	strcpy(standardMessage.content.recipient, recipient);
	strcpy(standardMessage.content.message, message);

	Msgsnd(SERVER_QUEUE_ID, &standardMessage, sizeof(standard_message), 0);
	return id;
}

int SndCompactMessage(type_t type, int value) {
	
	int id = GetMessageID();
	
	CLEAR(compactMessage);
	compactMessage.type = type;
	compactMessage.content.value = value;
	strcpy(compactMessage.content.sender, NICK);
	compactMessage.content.id = id;

	// printf("Sending to %d\n", SERVER_QUEUE_ID);
	Msgsnd(SERVER_QUEUE_ID, &compactMessage, sizeof(compact_message), 0);
	return id;
}

ssize_t RcvCompactMessage(type_t type) {
	
	CLEAR(compactMessage);
	return Msgrcv(CLIENT_QUEUE_ID, &compactMessage, sizeof(compact_message), type, IPC_NOWAIT);
}

ssize_t RcvStandardMessage(type_t type) {
	
	CLEAR(standardMessage);
	return Msgrcv(CLIENT_QUEUE_ID, &standardMessage, sizeof(standard_message), type, IPC_NOWAIT);
}

// int SndCompact(type_t type) {

// 	Msgsnd(SERVER_QUEUE_ID, structure, sizeof(structure), 0);
// 	return sizeof(*structure);
// }

ssize_t RcvHeartBeat() {
 	CLEAR(compactMessage);

	ssize_t x = msgrcv(CLIENT_QUEUE_ID, &compactMessage, sizeof(compactMessage) + 2, MSG_HEARTBEAT, IPC_NOWAIT);
	if (x < 0) {
		int errno;
		if (errno == ENOMSG) return 0;
		if (errno == EINVAL) return -1; 
		if (errno == EIDRM) return -1; 

		Error("HeartBeat!");
	}
	return x;
}
 
ssize_t SndHeartBeat(int id) {
 	CLEAR(compactMessage);
	
	compactMessage.type = MSG_HEARTBEAT;
	compactMessage.content.value = 0;
	strcpy(compactMessage.content.sender, NICK);
	compactMessage.content.id = id;
	
 	return Msgsnd(SERVER_QUEUE_ID, &compactMessage, sizeof(compactMessage), 0);
 }