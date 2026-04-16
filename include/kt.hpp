#ifndef KT_HPP
#define KT_HPP

#include "../third_party/chess.hpp"

#include <unordered_map>
#include <array>
#include <optional>


class KillerMovesTable {

    private:
        std::unordered_map<int, std::array<chess::Move, 2>> table;

    public:
        KillerMovesTable() = default;
        void add_move(chess::Move move, int ply);
        bool in_table(chess::Move move, int ply);
        std::optional<std::array<chess::Move, 2>> get_moves(int ply);

};

#endif
