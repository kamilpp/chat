#include "client_ui.h"

int ROW_MESSAGE_BOX_MAX;
int ROW_MAX;
int COL_MAX;

WINDOW *messageBox, *inputBar;

void InitializeNcurses() {
 
	/**
	 * Initialize ncurses libary.
	 */
    initscr(); 			
	
	/**
	 * Line buffering disabled.
	 */
	cbreak();			

	/**
	 * Special keys (ex. F1) handling enabled.
	 */
	keypad(stdscr, TRUE);

	/**
	 * Start color mode and define some colors.
	 */
	if (has_colors() == true) {
	    start_color(); 
	    init_pair(WHITE,   		COLOR_WHITE,   	COLOR_BLACK);
	    init_pair(RED, 	   		COLOR_RED, 	  	COLOR_BLACK);
	    init_pair(BLUE,    		COLOR_BLUE,    	COLOR_BLACK);
	    init_pair(YELLOW,  		COLOR_YELLOW,  	COLOR_BLACK);
	    init_pair(GREEN,      	COLOR_GREEN,   	COLOR_BLACK);
	    init_pair(MAGENTA,    	COLOR_MAGENTA, 	COLOR_BLACK);
	    init_pair(CYAN,       	COLOR_CYAN,    	COLOR_BLACK);
	    init_pair(CYAN_BLUE,  	COLOR_CYAN,   	COLOR_BLUE);
	    init_pair(GREEN_BLUE, 	COLOR_GREEN,   	COLOR_BLUE);
	    init_pair(RED_BLUE, 	COLOR_RED,   	COLOR_BLUE);

	    int styles[][2] = {{COLOR_PAIR(GREEN) | A_BOLD, COLOR_PAIR(RED)}, 
					{DEFAULT, COLOR_PAIR(RED)},
					{DEFAULT | A_BOLD, DEFAULT},
					{DEFAULT, DEFAULT},
					{COLOR_PAIR(YELLOW) | A_BOLD, DEFAULT},
					{COLOR_PAIR(RED) | A_BOLD, DEFAULT},
					{COLOR_PAIR(RED) | A_BOLD, DEFAULT}
				   };
		memcpy(STYLES, styles, sizeof(styles));
	} else {
	    Error("Colors unsupported");
    }

	/**
	 * Get window size.
	 */
    getmaxyx(stdscr, ROW_MAX, COL_MAX);

	/**
	 * Create messageBox.
	 */
	ROW_MESSAGE_BOX_MAX = ROW_MAX - 3;
	messageBox = newwin(ROW_MESSAGE_BOX_MAX, 0, 1, 0);

	/**
	 * Set pointer to actual number of printed lines to none.
	 */
    // ROW_MESSAGE_BOX_COUNTER = -1;

	/**
	 * Enable ncurses scroll function in messageBox.
	 */
    scrollok(messageBox, true);		

	/**
	 * Print program info.
	 */
	PrintfInfo("Client chat v1.0", INFO);

	/**
	 * Set nick and room to default, connectionStatus to disconnected.
	 */
	strcpy(NICK, DEFAULT_NICK_NAME);
	strcpy(room, GLOBAL_ROOM_NAME);
	connectionStatus = false;
	
	/**
	 * Create inputBar.
	 */
	inputBar = newwin(1, COL_MAX, ROW_MAX - 1, 0);
	PrintfStatusBar();
	CleanInputBar();
  	refresh();
}

void TerminateNcurses() {

	/**
	 * Destroy windows.
	 */
   	delwin(inputBar);
   	delwin(messageBox);
    
	/**
	 * Terminate ncurses.
	 */
    endwin();
}

void SetFont(WINDOW *window, int fontStyle) {

	/**
	 * Clear and set new window style.
	 */		
	wattrset(window, fontStyle);
}

/**
 * Shortcut to print client info, error or configuration message.
 */
void PrintfInfo(char message[], font_style fontStyle) {

	PrintfMessage(GetCurrentTime(), (fontStyle == INFO) ? "INFO" : ((fontStyle == CONFIG) ? "CONFIG" : "ERROR"), message, fontStyle);
}

void PrintfMessage(char *time, char sender[], char message[], font_style fontStyle) {

	// int y, x;
	// getyx(messageBox, y, x);
	/**
	 * If window is full - sroll one line and print in last one.
	 * Otherwise print in next line.
  	*/
	// if (y + 1 >= ROW_MESSAGE_BOX_MAX) {
	// 	wscrl(messageBox, 1);
	// } else {
	// 	y++;
	// }
	wscrl(messageBox, 1);


	/**
	 * Print hour.
	 */
	SetFont(messageBox, HOUR); 
	mvwprintw(messageBox, ROW_MESSAGE_BOX_MAX - 1, 1, time);

	/**
	 * Print sender.
	 */
	SetFont(messageBox, STYLES[fontStyle][0]); 
	char senderFormated[18];
	if (fontStyle != INFO && fontStyle != ERROR && fontStyle != CONFIG) {
		strcpy(senderFormated, "<");
		strcat(senderFormated, sender);
		strcat(senderFormated, ">");
	} else {
		strcpy(senderFormated, sender);
	}
	mvwprintw(messageBox, ROW_MESSAGE_BOX_MAX - 1, 25 - strlen(senderFormated), "%s", senderFormated);

	/**
	 * Print message.
	 */
	SetFont(messageBox, STYLES[fontStyle][1]); 
	mvwprintw(messageBox, ROW_MESSAGE_BOX_MAX - 1, 26, "%s", message);

	/**
	 * Refresh screen.
	 */
	wrefresh(messageBox);
	PrintfStatusBar();
	// if (!strcmp(sender, NICK)) {
		CleanInputBar();
	// }
}

void PrintfStatusBar() {

	/**
	 * Clear status panels.
	 */	
	SetFont(stdscr, STATUS_BAR_ROOM);
	for (int i = 0; i < COL_MAX; ++i) {
		mvwprintw(stdscr, 0, i, " ");
		mvwprintw(stdscr, ROW_MAX - 2, i, " ");
	} 

	/**
	 * Print room name.
	 */		
	mvwprintw(stdscr, ROW_MAX - 2, 1, "%s", room);

	/**
	 * Print connection status.
	 */
	if (connectionStatus == true) {
		SetFont(stdscr, STATUS_BAR_CONNECTED);
	} else {
		SetFont(stdscr, STATUS_BAR_DISCONNECTED);
	}
	mvwprintw(stdscr, ROW_MAX - 2, 2 + strlen(room), "[%s]", (connectionStatus == true) ? "connected" : "disconnected");

	
	SetFont(stdscr, DEFAULT);
}

void CleanInputBar() {
	/**
	 * Clean input bar.
	 */
	SetFont(inputBar, DEFAULT);
	for (int i = 0; i < COL_MAX; ++i) {
		mvwprintw(inputBar, 0, i, " ");
	} 	

	/**
	 * Print nick name.
	 */	
	mvwprintw(inputBar, 0, 1, "<%s>", NICK);

	/**
	 * Refresh and point cursor to proper position.
	 */	
	wrefresh(inputBar);
	move(ROW_MAX - 1, 1 + strlen(NICK) + 3);
}
