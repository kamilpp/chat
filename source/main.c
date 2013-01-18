#include "main.h"

void Error(char message[]) {

	perror(message);
	exit(-1);
}