CC = gcc

CFLAGS = -Wall -std=c11

TARGET = reversi

OBJS = \
	main.o\
	board.o\
	evaluation.o

RM = rm

ifeq ($(OS),Windows_NT)
	EXT = .exe
	RM = cmd.exe /C del
endif

all: clean target

target: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET)$(EXT) $(OBJS)
	-$(RM) $(OBJS)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

board.o: board.c
	$(CC) $(CFLAGS) -c board.c

evaluation.o: evaluation.c
	$(CC) $(CFLAGS) -c evaluation.c

.PHONY: clean
clean:
	-$(RM) $(TARGET)$(EXT) $(OBJS)