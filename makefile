reversi: main.o board.o
	gcc -o reversi.exe main.o board.o
	del *.o

main.o: main.c
	gcc -c main.c

board.o: board.c
	gcc -c board.c