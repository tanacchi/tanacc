CC       = gcc
CC_FLAGS = --ansi -g3 -Wall
TARGET   = tanacc
SRCS     = $(wildcard *.c)
OBJS     = $(SRCS:.c=.o)

$(TARGET): $(OBJS)
	$(CC) $(CC_FLAGS) -o $(TARGET) $(OBJS)

$(OBJS): tanacc.h

test:
	./test.bash

clean:
	-@rm tanacc *.o *.s
