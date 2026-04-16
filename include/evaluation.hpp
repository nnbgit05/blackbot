#ifndef EVALUATION_HPP 
#define EVALUATION_HPP

#include "board.hpp"

inline constexpr int INF = 999990; // infinity value

// piece evaluation values
inline constexpr int KING_VALUE = 9000;
inline constexpr int QUEEN_VALUE = 880;
inline constexpr int ROOK_VALUE = 510;
inline constexpr int BISHOP_VALUE = 320;
inline constexpr int KNIGHT_VALUE = 300;
inline constexpr int PAWN_VALUE = 100;

int evaluate(BoardWrapper &br);

#endif
