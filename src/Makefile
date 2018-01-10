CC = gcc

TARGET = reversi

OBJS = \
	main.o\
	board.o

RM = rm

ifeq ($(OS),Windows_NT)
	EXT = .exe
	RM = cmd.exe /C del
endif

all: clean target

target: $(OBJS)
	$(CC) -o $(TARGET)$(EXT) $(OBJS)
	-$(RM) $(OBJS)

main.o: main.c
	$(CC) -c main.c

board.o: board.c
	$(CC) -c board.c

.PHONY: clean
clean:
	-$(RM) $(TARGET) $(OBJS)