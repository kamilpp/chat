#include "main.h"

char* GetCurrentTime() {

	static char TIME[6];
<<<<<<< HEAD
	time_t rawtime = time(NULL);
	strftime(TIME, 6, "%H:%M", localtime(&rawtime));

	return TIME;
}

char* GetTime(time_t *time) {

	static char TIME[6];
	time_t rawtime = *time;
	strftime(TIME, 6, "%H:%M", localtime(&rawtime));

	return TIME;	
}

char* GetCurrentTimeLogFormat() {
	
	static char TIME[30]; // to nie wystarczy 25?
	time_t rawtime = time(NULL);
	strftime(TIME, 30, "%Y-%m-%dT%H:%M:%S%z", localtime(&rawtime));
=======
	time_t rawtime;
	struct tm* timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(TIME, 6, "%H:%M", timeinfo);
>>>>>>> master

	return TIME;
}

void Error(char message[]) {

	perror(message);
	exit(-1);
}

int Random() {
	return rand() % 1000 + 1;
}