#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>


struct termios startTerm;

void disableRaw() {
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &startTerm);
}

//exit with error
void die(const char *s) {
	perror(s); //prints global errno and our s
	exit(1);
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
	tcgetattr(STDIN_FILENO, &startTerm);
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

	tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
	atexit(disableRaw);

}	



int main() {
	enableRaw();
	while (1) {
		char c = '\0';
		read(STDIN_FILENO, &c, 1);
		if (iscntrl(c)) {
			printf("byte is %d\r\n", c);
		}
		else {
			printf("byte is %d and char is %c\r\n", c, c);
		}
		if (c == 'q') break;
	}
	
	return 0;
}
