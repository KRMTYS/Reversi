#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>

typedef enum Stone{
    NONE, BLACK, WHITE
} Stone;

void init();

bool in_area(int,int);
bool is_blank(int,int);

int search_all(Stone);
int search_8dir(int,int,Stone,bool);
int search_dir(int,int,int,Stone,bool);

void put_stone(int,int,Stone);

int count_stones(Stone);

void draw_board();

#endif