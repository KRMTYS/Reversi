#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>

// 盤面上の石
typedef enum
{
    WHITE = -1, // 白石
    BLANK =  0, // 空
    BLACK =  1  // 黒石
}
Disk;

// 盤面の初期化
void init_board();

// 座標判定
// 盤内か
bool in_board(int, int);
// 空か
bool is_blank(int, int);
// 石を置けるか
bool can_put_disk(int, int, Disk);

// 集計
// 返せるすべての石数を調べる
int count_all_reversal_disks(Disk);
// ある座標に置くとき返せる石数を調べる/石を返す
int count_reversal_disks(int, int, Disk, bool);
// ある座標からある方向への返せる石数を調べる/石を返す
int count_linear_reversal_disks(int, int, int, Disk, bool);
// 盤上の石を数える
int count_disks(Disk);

// 石の設置と反転
void put_disk(int, int, Disk);

// 盤面の描画
void print_board();

#endif