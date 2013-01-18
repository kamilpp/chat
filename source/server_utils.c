#include "server_utils.h"

int messageCounter = 0;

/*
 * Returns unique message id. Is used by few threads but does not reqire sharing due to diffirent message types.
 */
int GetMessageID() {
	return ((++messageCounter) % (int)10e9);
}

char typeString[15];
char *GetMessageType(int type);

//////////////////////////////////////////////////////////////////////////
int SndCompactMessage(int dest, type_t type, int value, int id) {
	
	Printf2("Sending compact message to %d of type %s", dest, GetMessageType(type));
	
	CLEAR(compactMessage);
	compactMessage.type = type;
	compactMessage.content.value = value;
	compactMessage.content.id = (id == -1) ? GetMessageID() : id;

	return Msgsnd(dest, &compactMessage, sizeof(compactMessage), 0);
}

ssize_t RcvCompactMessage(type_t type) {
	
	// printf("Recieving meessage of type %d, queue id is %d\n", type, SERVER_QUEUE_ID);
	CLEAR(compactMessage);
	printf("XX %d\n", SERVER_QUEUE_ID);
	ssize_t x = Msgrcv(SERVER_QUEUE_ID, &compactMessage, sizeof(compactMessage), type, IPC_NOWAIT);
	printf("XXX %d\n", SERVER_QUEUE_ID);
	if (x > 0) {
		Printf("Recieved compact message of type %s", GetMessageType(type));	
	} 
	return x;
}

ssize_t RcvStandardMessage(type_t type) {
	
	CLEAR(standardMessage);
	int x = Msgrcv(SERVER_QUEUE_ID, &standardMessage, sizeof(standardMessage), type, IPC_NOWAIT);
	if (x > 0) {
		Printf("Recieved standard message of type %s", GetMessageType(type));	
	} 
	return x;
}


int Snd(int destination, void *structure, int size) {
	
	Printf2("Sending special message to %d", destination);
	return Msgsnd(destination, structure, size, 0);
}

ssize_t Rcv(void *structure, int size, long type) {

	memset(structure, 0, size);

	ssize_t x = Msgrcv(SERVER_QUEUE_ID, structure, size, type, IPC_NOWAIT);
	if (x > 0) {
		Printf("Recieved special message of type %s", GetMessageType(type));	
	} 
	return x;
}

void V(int semnum) {
	Printf2("Semaphore %d up	", semnum);
	v(SEMAPHORES_ID, semnum);
}

void P(int semnum) {
	Printf2("Waiting for semaphore %d", semnum);
	p(SEMAPHORES_ID, semnum);
	Printf2("Semaphore %d down", semnum);
}
//////////////////////////////////////// LOCAL

char *GetMessageType(int type) {
	CLEAR(typeString);
	switch (type) {
		case MSG_HEARTBEAT:
			strcpy(typeString, "MSG_HEARTBEAT");
			break;
		case MSG_REGISTER:
			strcpy(typeString, "MSG_REGISTER");
			break;
		case MSG_UNREGISTER:
			strcpy(typeString, "MSG_UNREGISTER");
			break;
		case MSG_JOIN:
			strcpy(typeString, "MSG_JOIN");
			break;
		case MSG_LIST:
			strcpy(typeString, "MSG_LIST");
			break;
		case MSG_LEAVE:
			strcpy(typeString, "MSG_LEAVE");
			break;
		case MSG_STATUS:
			strcpy(typeString, "MSG_STATUS");
			break;
		case MSG_ROOM:
			strcpy(typeString, "MSG_ROOM");
			break;
		case MSG_PRIVATE:
			strcpy(typeString, "MSG_PRIVATE");
			break;
		case MSG_SERVER:
			strcpy(typeString, "MSG_SERVER");
			break;
		default:
			strcpy(typeString, "????");
	}
	return typeString;
}
