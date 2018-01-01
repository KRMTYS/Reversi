#include <stdio.h>
#include <ctype.h>

#include "board.h"

const int WIDTH=10;

/*typedef enum Stone{
    NONE, BLACK, WHITE
} Stone;*/

// 盤面の上下左右に番兵
Stone board[100]={NONE};

// 上から時計回り
int directions[]={
    -10,-9,1,11,10,9,-1,-11
};

void init();
bool in_area(int,int);
bool is_blank(int,int);
void draw_board();

void init()
{
    board[44]=WHITE;
    board[45]=BLACK;
    board[54]=BLACK;
    board[55]=WHITE;
}

bool in_area(int x,int y)
{
    if(x<1||x>8||y<1||y>8) return false;
    return true;
}

bool is_blank(int x,int y)
{
    if(board[y*WIDTH+x]==NONE) return true;
    return false;
}

int search_all(Stone stone)
{
    int count=0;

    for(int y=1;y<=8;y++){
        for(int x=1;x<=8;x++){
            count+=search_8dir(x,y,stone,false);
        }
    }

    return count;
}

int search_8dir(int x,int y,Stone stone,bool flip)
{
    int count=0;

    for(int i=0;i<8;i++){
        count+=search_dir(x,y,directions[i],stone,flip);
    }

    return count;
}

int search_dir(int x,int y,int dir,Stone stone,bool flip)
{
    int indices[8]={0};
    int count=0;

    while(1){
        Stone next=board[y*WIDTH+x+dir];

        if(next==stone){
            break;
        }
        else if(next==NONE){
            count=0;
            break;
        }
        else{
            indices[count]=y*WIDTH+x+dir;
            count++;
            dir+=dir;
        }
    }

    if((count>0)&&flip){
        for(int i=0;i<8;i++){
            board[indices[i]]=stone;
        }
    }

    return count;
}

void put_stone(int x,int y,Stone stone)
{
    board[y*WIDTH+x]=stone;
}

int count_stones(Stone stone)
{
    int count=0;

    for(int i=0;i<100;i++){
        if(board[i]==stone) count++;
    }

    return count;
}

void draw_board()
{
    printf(" ABCDEFGH\n");

    for(int y=1;y<=8;y++){
        printf("%d",y);
        for(int x=1;x<=8;x++){
            switch(board[y*WIDTH+x]){
                case WHITE:
                    printf("W");
                break;
                case BLACK:
                    printf("B");
                break;
                default:
                    printf("-");
                break;
            }
        }
        printf("\n");
    }
}