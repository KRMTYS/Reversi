///
/// @file   eval.h
/// @brief  盤面の評価
/// @author kentakuramochi
///

#ifndef EVAL_H_
#define EVAL_H_

#include "board.h"

/// 
/// @fn     evaluate
/// @brief  評価値の計算
/// @param  board   [in]    盤面
/// @param  disk    [in]    手番
/// @return 指定した手番の評価値
///
int evaluate(Board *board, Disk disk);

#endif // EVAL_H_
