#ifndef BOARD_HPP
#define BOARD_HPP

#include "../third_party/chess.hpp"

#include <string>


/**
 * board with a move stack 
 */
class BoardWrapper {

    private:
        std::vector<chess::Move> move_stack;

    public:
        chess::Board board; // make public
        BoardWrapper(std::string fen);

        /* wrapper for make move method */
        void push(chess::Move move);

        /* wrapper for unmake move method */
        void pop();

        /* wrapper for makeNullMove method */
        void push_null();

        /* get last move played */
        chess::Move last_move();

};

#endif

