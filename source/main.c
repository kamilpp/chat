#include "main.h"

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

void Error(char message[]) {

	perror(message);
	exit(-1);
}

int Random() {
	return rand() % 1000 + 1;
}