#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "board.h"

void do_turn(Stone);
void parse_command(int*,int*);

int main()
{
    Stone players[]={WHITE,BLACK};
    int turn=1;
    int count=0,prev_count=0;

    init();
    draw_board();

    while(1){
        printf("turn:%d\n",turn);
        count=search_all(players[turn%2]);
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

    int count_blacks=count_stones(BLACK);
    int count_whites=count_stones(WHITE);
    if(count_blacks>count_whites){
        printf("Black Wins\n");
    }else if(count_blacks<count_whites){
        printf("White Wins\n");
    }else{
        printf("Draw\n");
    }

    return 0;
}

void do_turn(Stone stone)
{
    int x,y;
    do{
        printf("player %d ",stone);
        parse_command(&x,&y);
    }while(!put_stone(x,y,stone));
}

void parse_command(int* x,int* y)
{
    char command[4];
    printf(">> ");
    fgets(command,sizeof(command),stdin);

    *x=toupper(command[0])-'A'+1;
    *y=command[1]-'0';
}