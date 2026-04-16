#include "../third_party/chess.hpp"
#include "../include/kt.hpp"


void KillerMovesTable::add_move(chess::Move move, int ply) {

    if (!this->table.contains(ply)) {
        this->table.insert({ply, {chess::Move::NULL_MOVE, chess::Move::NULL_MOVE}});
    }

    auto& killers = this->table.at(ply);
    if (killers[0] == move || killers[1] == move)
        return;

    killers[1] = killers[0];
    killers[0] = move;
}



bool KillerMovesTable::in_table(chess::Move move, int ply) {
    if (this->table.contains(ply)) {
        auto& killers = this->table.at(ply);
        return killers[0] == move || killers[1] == move;
    }
    return false;
}


std::optional<std::array<chess::Move, 2>> KillerMovesTable::get_moves(int ply) {
    if (this->table.contains(ply)) {
        return this->table.at(ply);
    }
    return std::nullopt;
}
