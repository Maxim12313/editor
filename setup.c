#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "setup.h"
#include "utils.h"


struct termios startTerm;

void disableRaw() {
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &startTerm) == -1) die("set disable raw");
}

//disabling
//echo = print back 
//icanon = read after enter
//isig = allow signal C-c 
//ixon = able to suspend resume C-s C-q
//iexten = allows C-o C-v
//icrnl = change C-m to <CR>
//opost = output processing \r to \n
void enableRaw() {
	if (tcgetattr(STDIN_FILENO, &startTerm) == -1) die("initial get tcgetattr");
	struct termios term = startTerm;

	//useful
	term.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	term.c_iflag &= ~(ICRNL | IXON);
	term.c_oflag &= ~(OPOST);

	//convention
 	term.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	term.c_cflag |= (CS8);

	//control characters
	term.c_cc[VTIME] = 10; //read every 1/10 sec
	term.c_cc[VMIN] = 0; //try to read even if empty

	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) == -1) die("set enable raw");
	atexit(disableRaw);

}	



int getCursorPos(int *row, int *col) {
	//query cursor information
	if (write(STDOUT_FILENO, "\x1b[6n", 4) == -1) return -1;
	

	char buffer[32];
	size_t i = 0;
	while (read(STDIN_FILENO, &buffer[i], 1) == 1 && buffer[i] != 'R') {
		i++;
	}
	buffer[i] = '\0';

	if (buffer[1] != '[') return -1;
	if (sscanf(&buffer[2], "%d;%d", row, col) != 2) return -1;
	return 0;
}

int getWindowSize(int *rows, int *cols) {
	struct winsize ws;
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
		//send cursor bottom right and get cursor position
		if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) return -1;
		return getCursorPos(rows, cols); 
	}
	else {
		*rows = ws.ws_col;
		*cols = ws.ws_row;
		return 0;
	}
}


void setup(struct editorState *E) {
	enableRaw();
	if (getWindowSize(&E->n, &E->m) == -1) die("getting window size");
	atexit(disableRaw);
}


