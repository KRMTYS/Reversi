///
/// @file   evaluator.h
/// @brief  盤面の評価
/// @author kentakuramochi
///

#ifndef EVALUATOR_H_
#define EVALUATOR_H_

#include <stdbool.h>

#include "board.h"

///
/// @def    DISK_VALUE
/// @brief  一石の評価値
///
#define DISK_VALUE 1000

///
/// @typedef    Evaluator
/// @brief      評価器
///
typedef struct Evaluator_ Evaluator;

///
/// @fn     Evaluator_create
/// @brief  評価器を生成する
/// @return 評価器
///
Evaluator *Evaluator_create(void);

///
/// @fn     Evaluator_delete
/// @brief  評価器を破棄する
/// @param[in,out]  eval    評価器
///
void Evaluator_delete(Evaluator *eval);

///
/// @fn     Evaluator_load
/// @brief  学習済み評価値ファイルから評価値を読み込む
/// @param[in,out]  eval    評価器
/// @param[in]      file    評価値ファイルパス
/// @retval true    読み込み成功
/// @retval false   読み込み失敗
///
bool Evaluator_load(Evaluator *eval, const char *file);

///
/// @fn     Evaluator_save
/// @brief  ファイルへ評価値を書き出す
/// @param[in]  eval    評価器
/// @param[in]  file    ファイル名
/// @retval true    出力成功
/// @retval false   出力失敗
///
bool Evaluator_save(Evaluator *eval, const char *file);

///
/// @fn     Evaluator_evaluate
/// @brief  局面を評価する
/// @param[in]  eval    評価器
/// @param[in]  board   盤面
/// @return 局面の評価値
///
int Evaluator_evaluate(Evaluator *eval, const Board *board);

///
/// @fn     Evaluator_add
/// @brief  局面を登録する
/// @param[out] eval    評価器
/// @param[in]  board   盤面
/// @param[in]  value   現在の評価値
///
void Evaluator_add(Evaluator *eval, const Board *board, int value);

///
/// @fn     Evaluator_update
/// @brief  評価値を更新する
/// @param[out] eval    評価器
///
void Evaluator_update(Evaluator *eval);

#endif // EVALUATOR_H_
