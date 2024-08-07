#ifndef UTILS_H
#define UTILS_H


#include <termios.h>

#define CTRL_KEY(k) ((k) & 0x1f) //00011111 strip bit 5 and 6

void die(const char *error);


struct editorState {
	int n; //row count
	int m; //col count
};

#endif
