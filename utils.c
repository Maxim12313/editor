#include "utils.h"
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

void die(const char *error) {
	perror(error); //prints global errno and our s
	exit(1);
}

