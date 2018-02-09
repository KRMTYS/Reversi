#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>

// 盤面上の石
typedef enum Disk {
    WHITE = -1,  // 白石
    NONE  =  0,  // 空
    BLACK =  1 // 黒石
} Disk;

// 盤面の初期化
void init_board();

// 設置判定
bool can_put_disk(int, int, Disk);

// 座標判定
// 盤内か
bool in_board(int, int);
// 空か
bool is_none(int, int);

// 石を数える
// 返せるすべての石数を調べる
int count_reversal_disks(Disk);
// ある座標に置くとき返せる石数を調べる/石を返す
int count_reversal_disks_8dir(int, int, Disk, bool);
// ある座標からある方向への返せる石数を調べる/石を返す
int count_reversal_disks_1dir(int, int, int, Disk, bool);

// 石の設置と反転
int put_disk(int, int, Disk);

// 石の集計
int count_disks(Disk);

// 盤面の描画
void print_board();

#endif