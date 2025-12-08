# Makefile for EduShell Custom Shell

CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99
TARGET = edushell
OBJS = main.o input.o builtin.o executor.o preview.o utils.o

# Default target
all: $(TARGET)

# Link all object files to create the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compile each source file to object file
main.o: main.c input.h builtin.h executor.h utils.h preview.h
	$(CC) $(CFLAGS) -c main.c

input.o: input.c input.h
	$(CC) $(CFLAGS) -c input.c

builtin.o: builtin.c builtin.h utils.h
	$(CC) $(CFLAGS) -c builtin.c

executor.o: executor.c executor.h builtin.h utils.h preview.h
	$(CC) $(CFLAGS) -c executor.c

preview.o: preview.c preview.h utils.h
	$(CC) $(CFLAGS) -c preview.c

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

# Clean up compiled files
clean:
	rm -f $(OBJS) $(TARGET)

# Rebuild everything from scratch
rebuild: clean all

# Run the shell
run: $(TARGET)
	./$(TARGET)

# Run with a test script
test: $(TARGET)
	./$(TARGET) test_script.txt

.PHONY: all clean rebuild run test