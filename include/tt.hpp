#ifndef TT_HPP 
#define TT_HPP 

#include "../third_party/chess.hpp"

#include <unordered_map>
#include <optional>
#include <cstdint>


enum TTFlag {
    EXACT_FLAG,
    ALPHA_FLAG,
    BETA_FLAG
};


struct TTEntry {
    int depth;
    TTFlag flag;
    int val;
    chess::Move best;
};


class TranspositionTable {

    private:
        std::unordered_map<uint64_t, TTEntry> table;

    public:
        TranspositionTable() = default;
        std::optional<int> probe_hash(int depth, int alpha, int beta, uint64_t zhash);        
        void record_hash(int depth, TTFlag flag, int val, chess::Move best, uint64_t zhash);
        std::optional<chess::Move> get_best_move(uint64_t zhash);
        size_t size();

};


#endif
