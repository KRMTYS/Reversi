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
/// @brief  有効手の探索と取得
/// @param  board       [in]    盤面
/// @param  serf_disk   [in]    手番
/// @return COMの次手
///
Pos com_search_move(Board *board, Disk self_disk);

#endif // COM_H_
