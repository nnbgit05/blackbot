#ifndef SEARCH_HPP
#define SEARCH_HPP

#include "../third_party/chess.hpp"

#include "board.hpp"


chess::Move get_best_move(BoardWrapper &br, int max_depth);

#endif
