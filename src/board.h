#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>

// 盤面上の石
typedef enum Disk {
    NONE,  // 空
    BLACK, // 黒石
    WHITE  // 白石
} Disk;

// 盤面の初期化
void init();

// 座標判定
// 盤内か
bool in_board(int, int);
// 空か
bool is_none(int, int);

// 返せるすべての石数を調べる
int count_all_disks(Disk);
// ある座標に置くとき返せる石数を調べる/石を返す
int count_around_disks(int, int, Disk, bool);
// ある座標からある方向への返せる石数を調べる/石を返す
int count_straight_disks(int, int, int, Disk, bool);

// 石を置く、石を返す
int put_disk(int, int, Disk);

// 石の集計
int count_disks(Disk);

// 盤面の描画
void print_board();

#endif