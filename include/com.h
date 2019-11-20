///
/// @file   com.h
/// @brief  COMの手番動作
/// @author kentakuramochi
///

#ifndef COM_H_
#define COM_H_

#include "board.h"

/// 
/// @fn     COM_get_move
/// @brief  COM側の有効手の探索
/// @param[in]  board   盤面
/// @param[in]  turn    手番
/// @return COMの次手
///
Pos COM_get_move(Board *board, Disk turn);

#endif // COM_H_
