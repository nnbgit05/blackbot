#include "board.hpp"

BoardWrapper::BoardWrapper(std::string fen) {
    this->board = chess::Board(fen);
}


void BoardWrapper::push(chess::Move move) {
    this->board.makeMove(move);
    this->move_stack.push_back(move);
}


void BoardWrapper::push_null() {
    this->board.makeNullMove();
    this->move_stack.push_back(chess::Move::NULL_MOVE);
}


void BoardWrapper::pop() {
    chess::Move last = this->last_move();
    this->move_stack.pop_back();
    if (last == chess::Move::NULL_MOVE) {
        this->board.unmakeNullMove();
    } else {
        this->board.unmakeMove(last);
    }
}


chess::Move BoardWrapper::last_move() {
    return this->move_stack.empty() ? chess::Move::NULL_MOVE : this->move_stack.back();
}
