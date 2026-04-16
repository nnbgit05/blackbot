#ifndef MOVE_ORDERING_HPP
#define MOVE_ORDERING_HPP

#include "../third_party/chess.hpp"

#include "board.hpp"
#include "tt.hpp"
#include "kt.hpp"
#include "debug.hpp"


/**
 * move ordering function
 */
std::vector<chess::Move> sort_moves(
        BoardWrapper& br, 
        const chess::Movelist& legal_moves, 
        TranspositionTable& tt, 
        KillerMovesTable& kt, 
        Debug& debug, 
        int ply
);


/**
 * sort captures according to eval function
 */
std::vector<chess::Move> sort_captures(
        BoardWrapper &br, 
        const chess::Movelist& captures, 
        TranspositionTable& tt, 
        Debug& debug
);

#endif
