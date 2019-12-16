///
/// @file   learn.h
/// @brief  自己対局による学習
/// @author kentakuramochi
///

#ifndef LEARN_H_
#define LEARN_H_

#include "board.h"
#include "com.h"

void learn(Board *board, Evaluator *evaluator, Com *com, const int iteration, const char* file);

#endif // LEARN_H_
