///
/// @file   board.h
/// @brief  リバーシ盤面の操作
/// @author kentakuramochi
///

#ifndef BOARD_H_
#define BOARD_H_

#include <stdbool.h>

///
/// @def    BOARD_SIZE
/// @brief  盤面の幅
///
#define BOARD_SIZE 8

///
/// @def    BOARD_LENGTH
/// @brief  番兵を含んだ盤面長
///
#define BOARD_LENGTH 91

///
/// @def    TO_POS
/// @brief  行・列数-座標変換
///
#define TO_POS(x, y)    ((y) * (BOARD_SIZE + 1) + (x))

///
/// @def    TO_X
/// @brief  座標-列数値変換
///
#define TO_X(p)         ((p) % (BOARD_SIZE + 1))

///
/// @def    TO_Y
/// @brief  座標-行数値変換
///
#define TO_Y(p)         ((int)((p) / (BOARD_SIZE + 1)))

///
/// @def    OPPONENT
/// @brief  逆手番の取得
///
#define OPPONENT(c)     (-1 * (c))

// スタック長、適当
#define STACK_LENGTH 60 * 20

///
/// @enum   Disk
/// @brief  盤面上のマスの状態
///
typedef enum {
    WHITE = -1, ///< 白石
    EMPTY =  0, ///< 空
    BLACK =  1, ///< 黒石
    WALL  =  2  ///< 壁
} Disk;

///
/// @enum   Pos
/// @brief  盤面上の座標インデックス（列アルファベット・行番号）
///
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

///
/// @enum   Dir
/// @brief  盤面上での8方向インデックス
///
typedef enum {
    UPPER       =  -9,  ///< 上
    UPPER_RIGHT =  -8,  ///< 右上
    UPPER_LEFT  = -10,  ///< 左上
    RIGHT       =   1,  ///< 右
    LEFT        =  -1,  ///< 左
    LOWER       =   9,  ///< 下
    LOWER_RIGHT =  10,  ///< 右下
    LOWER_LEFT  =   8   ///< 左下
} Dir;

///
/// @struct Board
/// @brief  リバーシ盤面
///
typedef struct {
    Disk squares[BOARD_LENGTH]; ///< マス
    Disk turn;                  ///< 現在の手番
    int stack[STACK_LENGTH];    ///< 手を記録するスタック
                                ///< (返した石の位置1), ... , (返した石の数), (置いた石の位置)
                                ///< の順に記録される
    int *sp;                    ///< スタックポインタ
} Board;

///
/// @fn     init_board
/// @brief  盤面の初期化
/// @param[in,out]  board    盤面
///
void init_board(Board *board);

///
/// @fn     is_valid
/// @brief  手が有効かの判定
/// @param[in]  board   盤面
/// @param[in]  disk    手番
/// @param[in]  pos     座標
/// @retval true    指定した手は有効手である
/// @retval false   指定した手は有効手ではない
///
bool is_valid(Board *board, Disk disk, Pos pos);

///
/// @fn     has_valid_move
/// @brief  有効手を持つかの判定
/// @param[in]  board   盤面
/// @param[in]  disk    手番
/// @retval true    指定した手番が有効手を持つ
/// @retval false   指定した手番が有効手を持たない
///
bool has_valid_move(Board *board, Disk disk);

///
/// @fn     count_valid_moves
/// @brief  有効手のカウント
/// @param[in]  board   盤面
/// @param[in]  disk    手番
/// @return 指定した手番の有効手数
///
int count_valid_moves(Board *board, Disk disk);

///
/// @fn     change_turn
/// @brief  手番の変更
/// @param[in,out]  board   盤面
///
void change_turn(Board *board);

///
/// @fn     count_flip_disks
/// @brief  返せる石数のカウント
/// @param[in]  board   盤面
/// @param[in]  disk    手番
/// @param[in]  pos     座標
/// @return 指定した座標に石を置いたとき返せる石数
///
int count_flip_disks(Board *board, Disk disk, Pos pos);

///
/// @fn     count_disks
/// @brief  石数のカウント
/// @param[in]  board   [in]    盤面
/// @param[in]  disk    [in]    手番
/// @return 指定した手番の石数
///
int count_disks(Board *board, Disk disk);

///
/// @fn     put_and_flip
/// @brief  石の設置と反転
/// @param[in,out]  board   盤面
/// @param[in]      disk    手番
/// @param[in]      pos     座標
/// @return 返した石数
///
int put_and_flip(Board *board, Disk disk, Pos pos);

///
/// @fn     undo
/// @brief  局面の巻き戻し
/// @param[in,out]  board    盤面
///
void undo(Board *board);

///
/// @fn     print_board
/// @brief  盤面の描画
/// @param[in]  board    盤面
///
void print_board(Board *board);

#endif // BOARD_H_
