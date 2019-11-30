///
/// @file   com.h
/// @brief  COM思考ルーチンの動作
/// @author kentakuramochi
///

#ifndef COM_H_
#define COM_H_

#include "board.h"
#include "evaluator.h"

typedef struct Com_ Com;

///
/// @fn     Com_create
/// @brief  COMを生成する
/// @param[in]  evaluator   評価器
/// @return COM
///
Com *Com_create(Evaluator* evaluator);

///
/// @fn     Com_delete
/// @brief  COMを破棄する
/// @param[in]  com     COM
///
void Com_delete(Com *com);

///
/// @fn     Com_init
/// @brief  COMを初期化する
/// @param[in] com  COM
///
void Com_init(Com *com);

///
/// @fn     Com_set_level
/// @brief  COMのレベルを設定する
/// @param[in] com          COM
/// @param[in] mid_depth    中盤の先読み手数
/// @param[in] th_exact     完全読みする空きマスの数
/// @param[in] th_wld       必勝読みする空きマスの数
///
void Com_set_level(Com *com, int mid_depth, int th_exact, int th_wld);

///
/// @fn     Com_get_nextmove
/// @brief  次手を取得する
/// @param[in]  com     COM
/// @param[in]  board   盤面
/// @param[in]  turn    手番
/// @param[out] value   評価値
/// @return COMの次手
///
Pos Com_get_nextmove(Com *com, Board *board, Disk turn, int *value);

///
/// @fn     Com_count_nodes
/// @brief  直前に探索したノード数を取得する
/// @param[in]  com     COM
/// @return 探索したノード数
///
int Com_count_nodes(Com *com);

#endif // COM_H_
