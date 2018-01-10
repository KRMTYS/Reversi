#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>

typedef enum Disk {
    NONE,
    BLACK,
    WHITE
} Disk;

void init();

bool in_board(int, int);
bool is_none(int, int);

int count_all_disks(Disk);
int count_around_disks(int, int, Disk, bool);
int count_straight_disks(int, int, int, Disk, bool);

int put_disk(int, int, Disk);

int count_disks(Disk);

void print_board();

#endif