#include "../third_party/chess.hpp"

#include "move_ordering.hpp"
#include "evaluation.hpp"
#include "tt.hpp"

#include <algorithm>



/* sort legal moves for negamax and root_search */
std::vector<chess::Move> sort_moves(
        BoardWrapper& br, 
        const chess::Movelist& legal_moves, 
        TranspositionTable& tt, 
        KillerMovesTable& kt, 
        Debug& debug, 
        int ply
) {

    std::vector<std::pair<chess::Move, int>> scored_moves;
    scored_moves.reserve(legal_moves.size()); // reserve size

    const auto& killers = kt.get_moves(ply); // get killer moves once

    // assign evaluation
    for (const chess::Move& move : legal_moves) {

        // captures
        if (br.board.isCapture(move)) {
            br.push(move);
            scored_moves.emplace_back(move, evaluate(br) - 100000);
            br.pop();

        // killer moves
        } else if (killers && (killers.value()[0] == move || killers.value()[1] == move)) {
            debug.kt_move_orders++;
            scored_moves.emplace_back(move, -50000);

        // non-captures
        } else {
            br.push(move);
            scored_moves.emplace_back(move, evaluate(br));
            br.pop();
        }
    }


    // sort the moves
    std::sort(scored_moves.begin(), scored_moves.end(),
            // sort in ascending order (smaller score = better move)
            [](const auto &lhs, const auto &rhs) {
            return lhs.second < rhs.second; // smaller score first
            });

    // get list of sorted moves from list of pairs
    std::vector<chess::Move> sorted_moves;
    sorted_moves.reserve(scored_moves.size()); // reserve size

    for (const auto& pair : scored_moves) {
        sorted_moves.push_back(pair.first);
    }

    // get best move from tt
    std::optional<chess::Move> best_move = tt.get_best_move(br.board.hash());

    // if best move exists and is in the sorted moves list, move to the start of
    // the list
    if (best_move) {
        auto it = std::find(sorted_moves.begin(), sorted_moves.end(), best_move.value());
        if (it != sorted_moves.end()) {
            debug.tt_move_orders++;
            sorted_moves.erase(it);
            sorted_moves.insert(sorted_moves.begin(), best_move.value());
        }
    }

    return sorted_moves;
}




/**
 * piece ordering value for MVV LVA
 */
constexpr int piece_order_value(chess::PieceType pt) {
    switch(pt) {
        case chess::PAWN: return PAWN_VALUE;
        case chess::KNIGHT: return KNIGHT_VALUE;
        case chess::BISHOP: return BISHOP_VALUE;
        case chess::ROOK: return ROOK_VALUE;
        case chess::QUEEN: return QUEEN_VALUE;
        case chess::KING: return KING_VALUE;
        default: return 0;
    }
}



/**
 * MVV-LVA evalaution function
 */
int mvv_lva_score(BoardWrapper& br, chess::Move move) {

    chess::Piece attacker = br.board.at(move.from());
    int attacker_value = piece_order_value(attacker.type());

    // determine victim value - be careful of en passant
    int victim_value = 0;

    if (move.typeOf() == chess::Move::ENPASSANT) {
        victim_value = 100;

    } else {
        chess::Piece victim = br.board.at(move.to());
        victim_value = piece_order_value(victim.type());
    }

    // evaluation formula
    // smaller = better so we sort in ASCENDING ORDER
    return -(victim_value * 10 - attacker_value);
}




/* sort captures for quiescence search */
std::vector<chess::Move> sort_captures(
        BoardWrapper& br, 
        const chess::Movelist& captures, 
        TranspositionTable& tt, 
        Debug& debug
) {

    std::vector<std::pair<chess::Move, int>> scored_moves;
    scored_moves.reserve(captures.size()); // reserve size

    // evaluate moves
    for (const chess::Move& move : captures) {
        scored_moves.emplace_back(move, mvv_lva_score(br, move)); // use mvv lva
    }

    // sort by evaluation
    std::sort(scored_moves.begin(), scored_moves.end(),
            // sort in ascending order (smaller score = better move)
            [](const auto &lhs, const auto &rhs) {
            return lhs.second < rhs.second; // smaller score first
            });

    // extract moves only
    std::vector<chess::Move> moves;
    moves.reserve(scored_moves.size()); // reserve size

    for (const auto& pair : scored_moves) {
        moves.push_back(pair.first);
    }

    // get best move from tt
    std::optional<chess::Move> best_move = tt.get_best_move(br.board.hash());

    // if best move exists and is in the sorted moves list, move to the start of
    // the list
    if (best_move) {
        auto it = std::find(moves.begin(), moves.end(), best_move.value());
        if (it != moves.end()) {
            debug.tt_move_orders++;
            moves.erase(it);
            moves.insert(moves.begin(), best_move.value());
        }
    }

    return moves;
}
