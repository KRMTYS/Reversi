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
#define OPPONENT(color)  (1 ^ (color))

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
/// @fn     POS2COL
/// @brief  座標インデックスから列アルファベットを取得する
///
#define POS2COL(pos) ('A' + Board_x((pos)))

///
/// @fn     POS2ROW
/// @brief  座標インデックスから行数を取得する
///
#define POS2ROW(pos) ('0' + (Board_y((pos))))

typedef enum {
    PATTERN_HV4_1 = 0,
    PATTERN_HV4_2,
    PATTERN_HV4_3,
    PATTERN_HV4_4,
    PATTERN_HV3_1,
    PATTERN_HV3_2,
    PATTERN_HV3_3,
    PATTERN_HV3_4,
    PATTERN_HV2_1,
    PATTERN_HV2_2,
    PATTERN_HV2_3,
    PATTERN_HV2_4,
    PATTERN_DIAG8_1,
    PATTERN_DIAG8_2,
    PATTERN_DIAG7_1,
    PATTERN_DIAG7_2,
    PATTERN_DIAG7_3,
    PATTERN_DIAG7_4,
    PATTERN_DIAG6_1,
    PATTERN_DIAG6_2,
    PATTERN_DIAG6_3,
    PATTERN_DIAG6_4,
    PATTERN_DIAG5_1,
    PATTERN_DIAG5_2,
    PATTERN_DIAG5_3,
    PATTERN_DIAG5_4,
    PATTERN_DIAG4_1,
    PATTERN_DIAG4_2,
    PATTERN_DIAG4_3,
    PATTERN_DIAG4_4,
    PATTERN_EDGE8_1,
    PATTERN_EDGE8_2,
    PATTERN_EDGE8_3,
    PATTERN_EDGE8_4,
    PATTERN_EDGE8_5,
    PATTERN_EDGE8_6,
    PATTERN_EDGE8_7,
    PATTERN_EDGE8_8,
    PATTERN_CORNER8_1,
    PATTERN_CORNER8_2,
    PATTERN_CORNER8_3,
    PATTERN_CORNER8_4,
    NUM_PATTERN
} PatternId;

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
/// @param[in]  color   [in]    石
/// @return 指定した石の数
///
int Board_count_disks(const Board *board, Disk color);

///
/// @fn     Board_flip
/// @brief  指定した座標に着手する
/// @param[in,out]  board   盤面
/// @param[in]      color   手番
/// @param[in]      pos     座標
/// @return 返した石数
///
int Board_flip(Board *board, Disk color, Pos pos);

///
/// @fn     Board_unflip
/// @brief  局面を一手戻す
/// @param[in,out]  board    盤面
/// @return 返した石数
///
int Board_unflip(Board *board);

///
/// @fn     Board_can_flip
/// @brief  指定した座標に返せる石があるか（有効手であるか）確認する
/// @param[in]  board   盤面
/// @param[in]  color   手番
/// @param[in]  pos     座標
/// @retval true    返せる石がある（有効手である）
/// @retval false   返せる石がない
///
bool Board_can_flip(const Board *board, Disk color, Pos pos);

///
/// @fn     Board_count_flips
/// @brief  返せる石数を数える
/// @param[in]  board   盤面
/// @param[in]  color   手番
/// @param[in]  pos     座標
/// @return 指定した座標に石を置いたとき返せる石数
///
int Board_count_flips(const Board *board, Disk color, Pos pos);

void Board_init_pattern(Board *board);

int Board_pattern(const Board *board, int id);

int Board_flip_pattern(Board *board, Disk color, Pos pos);

int Board_unflip_pattern(Board *board);

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
/// @fn     Board_can_play
/// @brief  返せる石があるか（有効手があるか）確認する
/// @param[in]  board   盤面
/// @param[in]  color   手番
/// @retval true    返せる石がある（有効手がある）
/// @retval false   返せる石がない（有効手がない）
///
bool Board_can_play(const Board *board, Disk color);

Pos Board_pos(int x, int y);

int Board_x(Pos pos);

int Board_y(Pos pos);

int Board_opponent(Disk color);

#endif // BOARD_H_
