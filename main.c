#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "board.h"

bool is_finished(Stone);
void do_turn(Stone);
void parse_command(int*,int*);
void decide_winner();

int main()
{
    Stone players[]={WHITE,BLACK};
    int turn=1;
    int count=0,prev_count=1;

    init();
    draw_board();

    while(1){
        printf("[%d] ",turn);

        count=search_residue(players[turn%2]);
        if(count==0){
            if(prev_count==0){
                printf("finished\n");
                break;
            }
            printf("pass\n");
            turn++;
            prev_count=count;
            continue;
        }

        do_turn(players[turn%2]);
        draw_board();
        turn++;
    }

    decide_winner();

    return 0;
}

void do_turn(Stone stone)
{
    char* player;
    if(stone==BLACK){
        player="Black";
    }
    else{
        player="White";
    }

    int x,y;
    do{
        printf("%s: ",player);
        parse_command(&x,&y);
    }while(!put_stone(x,y,stone));
}

void parse_command(int* x,int* y)
{
    char command[4];
    fgets(command,sizeof(command),stdin);

    *x=toupper(command[0])-'A'+1;
    *y=command[1]-'0';
}

void decide_winner()
{
    int blacks=count_stones(BLACK);
    int whites=count_stones(WHITE);

    printf("***\n");
    printf("B : W = %d : %d\n",blacks,whites);

    if(blacks>whites){
        printf("* Black Wins *\n");
    }else if(blacks<whites){
        printf("* White Wins *\n");
    }else{
        printf("* Draw *\n");
    }
}