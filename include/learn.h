///
/// @file   learn.h
/// @brief  自己対局による学習
/// @author kentakuramochi
///

#ifndef LEARN_H_
#define LEARN_H_

#include "board.h"
#include "com.h"

///
/// @fn     learn
/// @brief  自己対局し評価値を学習する
/// @param[in]  board       盤面
/// @param[in]  evaluator   評価器
/// @param[in]  com         COM思考ルーチン
/// @param[in]  iteration   学習回数
/// @param[in]  file        評価値出力ファイル名
/// @note   モンテカルロ法による強化学習、終局時の石数差を最大化する
///
void learn(Board *board, Evaluator *evaluator, Com *com, const int iteration, const char* file);

#endif // LEARN_H_
