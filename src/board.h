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

// 石を置く、石を返す
int put_disk(int, int, Disk);

// 石の集計
int count_disks(Disk);

// 盤面の描画
void print_board();

#endif