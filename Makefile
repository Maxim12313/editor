CC = gcc -std=c17
FLAGS = -Wall -Werror -Wextra
TARGET = editor
SRCS = $(wildcard *.c)

.PHONY: all
all: $(TARGET)

.PHONY: run
run: $(TARGET)
	./$(TARGET)

$(TARGET) : $(SRCS)
	$(CC) -o $@ $^ $(FLAGS)

.PHONY: clean
clean:
	rm -f $(TARGET)

