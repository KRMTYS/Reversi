///
/// @file   board.h
/// @brief  リバーシ盤面の操作
/// @author kentakuramochi
///

#ifndef BOARD_H_
#define BOARD_H_

#include <ctype.h>
#include <stdbool.h>

///
/// @def    BOARD_SIZE
/// @brief  盤面の幅
///
#define BOARD_SIZE 8

///
/// @def    BOARD_SIZE
/// @brief  番兵を含む盤面長
///
#define BOARD_LENGTH 91

///
/// @enum   Disk
/// @brief  盤面上の石・マスの状態
///
typedef enum {
    WHITE = -1, ///< 白石
    EMPTY =  0, ///< 空
    BLACK =  1, ///< 黒石
    WALL  =  2  ///< 壁
} Disk;

///
/// @def    OPPONENT
/// @brief  逆の色を取得する
///
#define OPPONENT(disk)     (-1 * (disk))

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
/// @fn     XY2POS
/// @brief  座標値からインデックスを取得する
///
#define XY2POS(x, y) ((x) * (BOARD_SIZE + 1) + (y))

///
/// @fn     CHAR2POS
/// @brief  行・列文字からインデックスを取得する
///
#define CHAR2POS(col, row) (XY2POS((toupper((col)) - 'A' + 1), ((row) - '0')))

///
/// @fn     POS2COL
/// @brief  座標インデックスから列アルファベットを取得する
///
#define POS2COL(pos) ('A' + ((pos) % (BOARD_SIZE + 1) - 1))

///
/// @fn     POS2ROW
/// @brief  座標インデックスから行数を取得する
///
#define POS2ROW(pos) ('0' + ((pos) / (BOARD_SIZE + 1)))

///
/// @typedef    Board
/// @brief      リバーシ盤面
///
typedef struct Board_ Board;

///
/// @fn     Board_create
/// @brief  盤面を生成する
///
Board *Board_create(void);

///
/// @fn     Board_delete
/// @brief  盤面を消去する
/// @param[in]  board    盤面
///
void Board_delete(Board *board);

///
/// @fn     Board_init
/// @brief  盤面を初期化する
/// @param[in,out]  board    盤面
///
void Board_init(Board *board);

///
/// @fn     Board_check_valid
/// @brief  手が有効か判定する
/// @param[in]  board   盤面
/// @param[in]  disk    手番
/// @param[in]  pos     座標
/// @retval true    指定した手は有効手である
/// @retval false   指定した手は有効手ではない
///
bool Board_check_valid(Board *board, Disk disk, Pos pos);

///
/// @fn     Board_has_valid_move
/// @brief  有効手があるか判定する
/// @param[in]  board   盤面
/// @param[in]  disk    手番
/// @retval true    指定した手番が有効手を持つ
/// @retval false   指定した手番が有効手を持たない
///
bool Board_has_valid_move(Board *board, Disk disk);

///
/// @fn     Board_count_valid_moves
/// @brief  有効手を数える
/// @param[in]  board   盤面
/// @param[in]  disk    手番
/// @return 指定した手番の有効手数
///
int Board_count_valid_moves(Board *board, Disk disk);

///
/// @fn     Board_count_disk
/// @brief  石数を数える
/// @param[in]  board   [in]    盤面
/// @param[in]  disk    [in]    石
/// @return 指定した石の数
///
int Board_count_disk(Board *board, Disk disk);

///
/// @fn     Board_count_flip_disks
/// @brief  返せる石数を数える
/// @param[in]  board   盤面
/// @param[in]  disk    手番
/// @param[in]  pos     座標
/// @return 指定した座標に石を置いたとき返せる石数
///
int Board_count_flip_disks(Board *board, Disk disk, Pos pos);

///
/// @fn     Board_put_and_flip
/// @brief  石を着手する
/// @param[in,out]  board   盤面
/// @param[in]      disk    手番
/// @param[in]      pos     座標
/// @return 返した石数
///
int Board_put_and_flip(Board *board, Disk disk, Pos pos);

///
/// @fn     Board_undo
/// @brief  局面を巻き戻す
/// @param[in,out]  board    盤面
///
void Board_undo(Board *board);

///
/// @fn     Board_print
/// @brief  盤面を描画する
/// @param[in]  board    盤面
///
void Board_print(Board *board, Disk current);

#endif // BOARD_H_
