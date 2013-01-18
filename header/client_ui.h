#ifndef CLIENT_UI_H
#define CLIENT_UI_H

#include "main.h"
#include <ncurses.h>

typedef enum {
	WHITE,
	RED,
	BLUE,
	YELLOW,
	GREEN,
	MAGENTA,
	CYAN,
	CYAN_BLUE,
	GREEN_BLUE,
	RED_BLUE
} font_color;

/**
  * Define some font styles used to display messages.
  */
#define HOUR (COLOR_PAIR(YELLOW))
#define STATUS_BAR_ROOM (COLOR_PAIR(CYAN_BLUE)|A_BOLD)
#define STATUS_BAR_CONNECTED (COLOR_PAIR(GREEN_BLUE)|A_BOLD)
#define STATUS_BAR_DISCONNECTED (COLOR_PAIR(RED_BLUE)|A_BOLD)
#define DEFAULT (COLOR_PAIR(WHITE))

typedef enum {
	PRIVATE_MESSAGE_GET,
	PRIVATE_MESSAGE_SEND,
	MESSAGE_GET,
	MESSAGE_SEND,
	INFO,
	ERROR,
	CONFIG
} font_style;

font_style STYLES[7][2]; // Initialization of "objects" only in *.c files...

#define DEFAULT_ROOM_NAME "Lobby"
#define DEFAULT_NICK_NAME "noname"

extern char NICK[16]; 

extern char room[16];
extern int connectionStatus;

void InitializeNcurses(); 
void TerminateNcurses();

void SetFont(WINDOW*, int);
void PrintfInfo(char[], font_style);
void PrintfMessage(char*, char[], char[], font_style);
void PrintfStatusBar();
void CleanInputBar();

char* GetCurrentTime();
char* GetTime(time_t *);

#endif

// A_NORMAL        Normal display (no highlight)
// A_STANDOUT      Best highlighting mode of the terminal.
// A_UNDERLINE     Underlining
// A_REVERSE       Reverse video
// A_BLINK         Blinking
// A_DIM           Half bright
// A_BOLD          Extra bright or bold
// A_PROTECT       Protected mode
// A_INVIS         Invisible or blank mode
// A_ALTCHARSET    Alternate character set
// A_CHARTEXT      Bit-mask to extract a character
// COLOR_PAIR(n)   Color-pair number n 