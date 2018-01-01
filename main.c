#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "board.h"

bool continues(Stone);
void put(Stone);
void parse_command(int*,int*);

int main()
{
    Stone players[]={WHITE,BLACK};
    int turn=1;

    init();

    draw_board();

    while(continues(players[turn%2])){
        put(players[turn%2]);
        printf("turn:%d\n",turn);
        draw_board();
        turn++;
    }

    return 0;
}

bool continues(Stone stone)
{
    if(search_all(stone)==0){
        return false;
    }

    return true;
}

void put(Stone stone)
{
    int x,y,count;

    do{
        parse_command(&x,&y);

        if(!in_area(x,y)||!is_blank(x,y)){
            printf("You can't put a stone here.\n");
        }

        count=search_8dir(x,y,stone,true);
    }while(count==0);

    put_stone(x,y,stone);
}

void parse_command(int* x,int* y)
{
    char command[4];
    printf(">> ");
    fgets(command,sizeof(command),stdin);

    *x=toupper(command[0])-'A'+1;
    *y=command[1]-'0';
}