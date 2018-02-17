#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>

// 番兵含む盤面の幅
#define SQUARE_SIZE 9
// 番兵含む盤面長
#define SQUARE_LENGTH 91

// 座標インデックスへの変換
#define TO_POS(x, y) (y * SQUARE_SIZE + x)

// 逆の色を返す
#define REVERSE(c) (-1 * c)

// スタック長、適当
#define STACK_LENGTH 1000

// 石
typedef enum
{
    WHITE = -1, // 白石
    EMPTY =  0, // 空
    BLACK =  1  // 黒石
}
Disk;

// 座標
typedef enum
{
    A1 = 10, B1, C1, D1, E1, F1, G1, H1,
    A2 = 19, B2, C2, D2, E2, F2, G2, H2,
    A3 = 28, B3, C3, D3, E3, F3, G3, H3,
    A4 = 37, B4, C4, D4, E4, F4, G4, H4,
    A5 = 46, B5, C5, D5, E5, F5, G5, H5,
    A6 = 55, B6, C6, D6, E6, F6, G6, H6,
    A7 = 64, B7, C7, D7, E7, F7, G7, H7,
    A8 = 73, B8, C8, D8, E8, F8, G8, H8
}
Pos;

// 方向
typedef enum
{
    UPPER       =  -9, // 上
    UPPER_RIGHT =  -8, // 右上
    UPPER_LEFT  = -10, // 左上
    RIGHT       =   1, // 右
    LEFT        =  -1, // 左
    LOWER       =   9, // 下
    LOWER_RIGHT =  10, // 右下
    LOWER_LEFT  =   8  // 左下
}
Dir;

// 手番状態
typedef enum
{
    DO_TURN,
    PASS,
    FINISH
}
State;

// 盤
typedef struct
{
    // 盤面
    Disk square[SQUARE_LENGTH];
    // 手番
    Disk current_turn;
    // 手番を記録するスタック
    // (返した石の位置1), ... , (返した石の数), (置いた石の位置)
    // の順に記録される
    int stack[STACK_LENGTH];
    // スタックポインタ
    int* sp;
}
Board;

// 盤面の初期化
void init_board(Board*);

// 盤内か
bool is_on_board(int, int);
// 空か
bool is_empty(int, int, Board*);
// 有効手か
bool is_valid(int, int, Disk, Board*);
// 有効手を持つか
bool has_valid_move(Disk, Board*);

// パス/終了判定
State get_state(Board*);

// ある座標から一方向の返せる石数を調べる
int count_flip_disks_line(Pos, Disk, Dir, Board*);
// ある座標に置くとき返せる石数を調べる
int count_flip_disks(Pos, Disk, Board*);

// 盤上の石を数える
int count_disks(Disk, Board*);

// 一方向への石の反転
int flip_line(Pos, Disk, Dir, Board*);
// 石の設置と反転
int put_and_flip(Pos, Disk, Board*);

// 局面を戻す
void undo(Board*);

// 盤面の描画
void print_board(Board*);

// 勝敗判定
void judge(Board*);

// スタック操作
void push(int n, Board*);
int pop(Board*);

#endif