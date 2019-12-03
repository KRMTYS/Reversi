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
    WALL  = -1, ///< 壁
    BLACK =  0, ///< 黒石
    WHITE =  1, ///< 白石
    EMPTY =  2, ///< 空
} Disk;

///
/// @def    OPPONENT
/// @brief  逆の色を取得する
///
#define OPPONENT(disk)     (1 ^ (disk))

///
/// @enum   Pos
/// @brief  盤面上の座標インデックス（列アルファベット・行番号）
///
typedef enum {
    NONE = 0,   // 座標なし
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
#define XY2POS(x, y) (((y) + 1) * (BOARD_SIZE + 1) + ((x) + 1))

///
/// @fn     CHAR2POS
/// @brief  行・列文字からインデックスを取得する
///
#define CHAR2POS(col, row) (XY2POS((toupper((col)) - 'A'), ((row) - '1')))

///
/// @fn     POS2COL
/// @brief  座標インデックスから列アルファベットを取得する
///
#define POS2COL(pos) ('A' + ((pos) % (BOARD_SIZE + 1) - 1))

///
/// @fn     POS2ROW
/// @brief  座標インデックスから行数を取得する
///
#define POS2ROW(pos) ('0' + ((pos) / (BOARD_SIZE + 1) - 1))

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
/// @fn     Board_disk
/// @brief  盤面の状態を取得する
/// @param[in]  board   盤面
/// @param[in]  pos     座標
/// @return 指定した座標の状態
///
Disk Board_disk(const Board *board, Pos pos);

///
/// @fn     Board_count_disks
/// @brief  指定した色の石数を数える
/// @param[in]  board   [in]    盤面
/// @param[in]  disk    [in]    石
/// @return 指定した石の数
///
int Board_count_disks(const Board *board, Disk disk);

///
/// @fn     Board_flip
/// @brief  指定した座標に着手する
/// @param[in,out]  board   盤面
/// @param[in]      disk    手番
/// @param[in]      pos     座標
/// @return 返した石数
///
int Board_flip(Board *board, Disk disk, Pos pos);

///
/// @fn     Board_unflip
/// @brief  局面を一手戻す
/// @param[in,out]  board    盤面
/// @return 返した石数
///
int Board_unflip(Board *board);

///
/// @fn     Board_count_flips
/// @brief  返せる石数を数える
/// @param[in]  board   盤面
/// @param[in]  disk    手番
/// @param[in]  pos     座標
/// @return 指定した座標に石を置いたとき返せる石数
///
int Board_count_flips(const Board *board, Disk disk, Pos pos);

///
/// @fn     Board_can_flip
/// @brief  指定した座標に返せる石があるか（有効手であるか）確認する
/// @param[in]  board   盤面
/// @param[in]  disk    手番
/// @param[in]  pos     座標
/// @retval true    返せる石がある（有効手である）
/// @retval false   返せる石がない
///
bool Board_can_flip(const Board *board, Disk disk, Pos pos);

///
/// @fn     Board_can_play
/// @brief  返せる石があるか（有効手があるか）確認する
/// @param[in]  board   盤面
/// @param[in]  disk    手番
/// @retval true    返せる石がある（有効手がある）
/// @retval false   返せる石がない（有効手がない）
///
bool Board_can_play(const Board *board, Disk disk);

///
/// @fn     Board_copy
/// @brief  盤面をコピーする
/// @param[in]  src コピー元盤面
/// @param[out] dst コピー先盤面
///
void Board_copy(const Board *src, Board *dst);

///
/// @fn     Board_reverse
/// @brief  石色を逆転する
/// @param[in]  board   盤面
///
void Board_reverse(Board *board);

///
/// @fn     Board_print
/// @brief  盤面を描画する
/// @param[in]  board   盤面
/// @param[in]  turn    現在の手番
///
void Board_print(const Board *board, Disk turn);

#endif // BOARD_H_
