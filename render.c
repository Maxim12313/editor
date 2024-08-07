#include <stdio.h>
#include <unistd.h>

#include "render.h"
#include "utils.h"


void drawRows(struct editorState *E) {
	printf("n: %d  m: %d\r\n", E->n, E->m);
}

void clearScreen() {
	// TODO: put stuff
	write(STDIN_FILENO, "\x1b[2J\x1b[H", 8);
}

void refreshScreen(struct editorState *E) {
	//hide cursor
    write(STDIN_FILENO, "\x1b[?25l", 6);
	
	//cursor top left
    write(STDIN_FILENO, "\x1b[H", 3);

    drawRows(E);

	//cursor top left
    write(STDIN_FILENO, "\x1b[H", 3);
	//bring cursor back
    write(STDIN_FILENO, "\x1b[?25h", 6);
}

