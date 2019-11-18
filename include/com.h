///
/// @file   com.h
/// @brief  COMの手番動作
/// @author kentakuramochi
///

#ifndef COM_H_
#define COM_H_

#include "board.h"

/// 
/// @fn     com_search_move
/// @brief  COM側の有効手の探索
/// @param[in]  board       盤面
/// @param[in]  self_disk   手番
/// @return COMの次手
///
Pos com_search_move(Board *board, Disk self_disk);

#endif // COM_H_
