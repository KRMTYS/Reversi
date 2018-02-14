#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>

#define BOARD_LENGTH 100

// 盤面上の石
typedef enum
{
    WHITE = -1, // 白石
    EMPTY =  0, // 空
    BLACK =  1  // 黒石
}
Disk;

typedef struct
{
    // 盤面
    // 8x8マス + 周囲の番兵
    Disk square[BOARD_LENGTH];
    // 手番
    Disk turn;
}
Board;

// 盤面の初期化
void init_board();

// 座標判定
// 盤内か
bool is_on_board(int, int);
// 空か
bool is_empty(int, int);
// 有効手か
bool is_valid_move(int, int, Disk);

// 集計
// 有効手数を調べる
int count_valid_moves(Disk);
// ある座標から一方向の返せる石数を調べる
int count_straight_reversal_disks(int, int, int, Disk);
// ある座標に置くとき返せる石数を調べる
int count_reversal_disks(int, int, Disk);
// 返せるすべての石数を調べる
int count_all_reversal_disks(Disk);
// 盤上の石を数える
int count_disks(Disk);

// 有効手が存在するか
bool has_valid_move(Disk);

// 石の設置と反転
void put_and_flip(int, int, Disk);

// 盤面の描画
void print_board();

#endif