#include <stdlib.h>
#include "unistd.h"
#include "stdio.h"
#include "errno.h"

#include "render.h"
#include "setup.h"
#include "edit.h"
#include "utils.h"


char readKey() {
	char c;
	while (read(STDIN_FILENO, &c, 1) == -1) {
		if (errno != EAGAIN) die("reading");
	}
	return c;
}

void processKeyPress() {
	char c = readKey();
	// printf("read keycode is %d\r\n", c);
	switch(c) {
	case CTRL_KEY('q'):
		exit(0);
		break;
	}
}

int main() {
	struct editorState E;
	setup(&E);
	while (1) {
		processKeyPress();
		refreshScreen(&E);
	}
	return 0;
}

