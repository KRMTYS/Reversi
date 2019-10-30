#ifndef BOARD_H_
#define BOARD_H_

#include <stdbool.h>

// 盤面の幅
#define BOARD_SIZE 8
// 盤面長（番兵含む）
#define BOARD_LENGTH 91

// 座標インデックスへの変換
#define TO_POS(x, y)    ((y) * (BOARD_SIZE + 1) + (x))

// x-y座標への変換
#define TO_X(p)         ((p) % (BOARD_SIZE + 1))
#define TO_Y(p)         ((int)((p) / (BOARD_SIZE + 1)))

// 逆の色を返す
#define OPPONENT(c)     (-1 * (c))

// スタック長、適当
#define STACK_LENGTH 60 * 20

// 盤面として表示する文字列
#define BLACK_STR "@ "
#define WHITE_STR "O "
#define VALID_STR "* "
#define EMPTY_STR "- "

// 石とマスの状態
typedef enum {
    WHITE = -1, // 白石
    EMPTY =  0, // 空
    BLACK =  1, // 黒石
    WALL  =  2  // 壁
} Disk;

// 座標
typedef enum {
    A1 = 10, B1, C1, D1, E1, F1, G1, H1,
    A2 = 19, B2, C2, D2, E2, F2, G2, H2,
    A3 = 28, B3, C3, D3, E3, F3, G3, H3,
    A4 = 37, B4, C4, D4, E4, F4, G4, H4,
    A5 = 46, B5, C5, D5, E5, F5, G5, H5,
    A6 = 55, B6, C6, D6, E6, F6, G6, H6,
    A7 = 64, B7, C7, D7, E7, F7, G7, H7,
    A8 = 73, B8, C8, D8, E8, F8, G8, H8
} Pos;

// 方向
typedef enum {
    UPPER       =  -9, // 上
    UPPER_RIGHT =  -8, // 右上
    UPPER_LEFT  = -10, // 左上
    RIGHT       =   1, // 右
    LEFT        =  -1, // 左
    LOWER       =   9, // 下
    LOWER_RIGHT =  10, // 右下
    LOWER_LEFT  =   8  // 左下
} Dir;

// 盤
typedef struct {
    // 盤面
    Disk squares[BOARD_LENGTH];
    // 手番
    Disk current_turn;
    // 手番数
    int turn;
    // 手番を記録するスタック
    // (返した石の位置1), ... , (返した石の数), (置いた石の位置)
    // の順に記録される
    int stack[STACK_LENGTH];
    // スタックポインタ
    int *sp;
} Board;

// 盤面の初期化
void init_board(Board *board);

// 有効手か
bool is_valid(Board *board, Disk disk, Pos pos);

// 有効手を持つか
bool has_valid_move(Board *board, Disk disk);
// 有効手を数える
int count_valid_moves(Board *board, Disk disk);

// 手番の変更
void change_turn(Board *board, int n);

// ある座標に置くとき返せる石数を調べる
int count_flip_disks(Board *board, Disk disk, Pos pos);

// 石数を数える
int count_disks(Board *board, Disk disk);

// 石の設置と反転
int put_and_flip(Board *board, Disk disk, Pos pos);

// 局面を戻す
void undo(Board *board);

// 盤面の描画
void print_board(Board *board);

#endif // BOARD_H_
