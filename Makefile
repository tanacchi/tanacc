CC       = gcc
CC_FLAGS = --ansi -g3 -Wall
TARGET   = tanacc

$(TARGET): compiler.c
	$(CC) $(CC_FLAGS) -o $(TARGET) compiler.c

clean:
	-@rm tanacc *.o *.s
