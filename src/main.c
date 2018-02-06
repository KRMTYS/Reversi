#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>

#include "board.h"

bool play_turn(int,int*);
void player_input(int*, int*);
void program_input(int*, int*);
void judge();

int main()
{
    int prev_count = 1;
    int turn = 1;

    srand((unsigned)time(NULL));

    init();
    printf("You are Black\n");

    while (play_turn(turn, &prev_count))
    {
        print_board();
        turn++;
    }

    judge();

    return 0;
}

bool play_turn(int turn,int* prev_count)
{
    Disk disk = (turn % 2 == 1) ? BLACK : WHITE;

    int count = count_all_disks(disk);

    if (count == 0)
    {
        // finish a game if both of players couldn't put disk
        if (count == *prev_count)
        {
            return false;
        }
        else
        {
            printf("pass\n");
        }
    }

    *prev_count = count;

    int x, y;

    // put disk if a player could
    while (count > 0)
    {
        printf("[%d] %s: ", turn, (turn % 2 == 1) ? "Black" : "White");

        //player_input(&x, &y);
        program_input(&x, &y);

        if (in_board(x, y) || is_none(x, y))
        {
            if (put_disk(x, y, disk))
                break;
        }
    }

    printf("\n");

    return true;
}

void player_input(int* x, int* y)
{
    char input[4];
    fgets(input, 4, stdin);

    if (!isalpha(input[0]) || !isdigit(input[1]))
        return;

    *x = toupper(input[0]) - '@';
    *y = input[1] - '0';
}

void program_input(int* x, int* y)
{
    *x = rand() % 8 + 1;
    *y = rand() % 8 + 1;

    printf("%c%c\n", *x + '@', *y + '0');
}

// count disks and judge
void judge()
{
    int num_black = count_disks(BLACK);
    int num_white = count_disks(WHITE);

    printf("\n\n*********\n");
    printf("Black: %2d\n", num_black);
    printf("White: %2d\n", num_white);
    printf("*********\n");

    if (num_black > num_white)
    {
        printf("* Black Wins *\n");
    }
    else if (num_black < num_white)
    {
        printf("* White Wins *\n");
    }
    else
    {
        printf("* Draw *\n");
    }
}