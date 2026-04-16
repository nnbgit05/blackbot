#include "../third_party/chess.hpp"
#include "../include/tt.hpp"


std::optional<int> TranspositionTable::probe_hash(int depth, int alpha, int beta, uint64_t zhash) {

    if (this->table.contains(zhash)) {
        auto [entry_depth, entry_flag, entry_value, _] = this->table.at(zhash);
        if (entry_depth >= depth) {
            if (entry_flag == EXACT_FLAG) {
                return entry_value;
            } else if (entry_flag == ALPHA_FLAG && entry_value <= alpha) {
                return alpha;
            } else if (entry_flag == BETA_FLAG && entry_value >= beta) {
                return beta;
            }
        }
    }

    return std::nullopt;
}


void TranspositionTable::record_hash(int depth, TTFlag flag, int val, chess::Move best, uint64_t zhash) {
    auto it = table.find(zhash);

    if (it == table.end()) { // entry doesn't exist yet
        table.emplace(zhash, TTEntry{depth, flag, val, best});
        return;
    }

    const TTEntry& old_entry = it->second;

    // only replace if entry's depth is greater
    // prevents quiescence entries from overwriting negamax entries
    if (depth >= old_entry.depth) {
        it->second = TTEntry{depth, flag, val, best};

    }
}


std::optional<chess::Move> TranspositionTable::get_best_move(uint64_t zhash) {
    if (this->table.contains(zhash)) {
        return this->table.at(zhash).best;
    }
    return std::nullopt;
}


size_t TranspositionTable::size() {
    return this->table.size();
}
