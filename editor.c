#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>


#define CTRL_KEY(k) ((k) & 0x1f) //00011111 strip bit 5 and 6

struct editorConfig {
	struct termios startTerm;
	int screenRows;
	int screenCols;
};

struct abuf {
	char *data;
	int size;
};
#define ABUF_INIT { NULL, 0 };

void abAppend(struct abuf *ab, const char *s, int size) {
	char *newData = realloc(ab->data, ab->size + size);
	if (newData == NULL) return;
	memcpy(&newData[ab->size], s, size);
	ab->data = newData;
	ab->size += size;
}

void abFree(struct abuf *ab) {
	free(ab->data);
}


struct editorConfig E;

void drawExtraRows(struct abuf *ab) {
	for (int r = 0; r < E.screenRows - 1; r++) {
		abAppend(ab, "~\r\n", 3);
	}
	abAppend(ab, "~", 1);
}

void clearScreen() {
	write(STDIN_FILENO, "\x1b[2J\x1b[H", 8);
}

void refreshScreen() {
	struct abuf ab = ABUF_INIT;
	abAppend(&ab, "\x1b[2J\x1b[H", 8); //clear screen
	drawExtraRows(&ab);
	write(STDIN_FILENO, ab.data, ab.size);
}

//exit with error
void die(const char *s) {
	clearScreen();

	perror(s); //prints global errno and our s
	exit(1);
}

void disableRaw() {
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.startTerm) == -1) die("set disable raw");
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

	if (tcgetattr(STDIN_FILENO, &E.startTerm) == -1) die("initial get tcgetattr");
	struct termios term = E.startTerm;
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


char readKey() {
	char c;
	while (read(STDIN_FILENO, &c, 1) == -1) {
		if (errno != EAGAIN) die("reading");
	}
	return c;
}

void processKeyPress() {
	char c = readKey();
	printf("%d\r\n", c);
	switch(c) {
	case CTRL_KEY('q'):
		clearScreen();
		exit(0);
		break;
	}
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

void initEditor() {
	if (getWindowSize(&E.screenRows, &E.screenCols) == -1) die("getting screen size");
}


int main() {
	enableRaw();
	initEditor();
	while (1) {
		refreshScreen();
		processKeyPress();
	}
	
	return 0;
}
