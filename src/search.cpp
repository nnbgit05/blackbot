#include "evaluation.hpp"
#include "kt.hpp"
#include "search.hpp"
#include "tt.hpp"
#include "board.hpp"
#include "move_ordering.hpp"
#include "debug.hpp"

#include <chrono>
#include <iostream>

#define R 2
#define WINDOW 25
#define VERSION "0.36"




/**
 * quiescence search - search captures until there are no more
 * generates all legal moves if side to move is in check
 * (to avoid the horizon effect)
 */
int quiescence(BoardWrapper &br, int alpha, int beta, TranspositionTable& tt, KillerMovesTable& kt, Debug &debug) {

    // check for insufficient material or draw by 50 move rule
    if (br.board.isInsufficientMaterial() || br.board.isHalfMoveDraw()) {
        return 0;
    }

    // check transposition table early on
    if (auto tt_val = tt.probe_hash(0, alpha, beta, br.board.hash())) {
        debug.tt_hits++;
        return tt_val.value();
    }

    // in check so not a quiet position
    // have to look at all legal moves
    if (br.board.inCheck()) {

        chess::Movelist legal_moves;
        chess::movegen::legalmoves(legal_moves, br.board);

        if (legal_moves.empty()) { // checkmate
            return -INF;
        }

        // sort all legal moves
        auto sorted_moves = sort_moves(br, legal_moves, tt, kt, debug, 0);

        chess::Move best_move = chess::Move::NULL_MOVE;
        TTFlag flag = TTFlag::ALPHA_FLAG;

        for (const chess::Move& move : sorted_moves) {

            debug.positions++;
            br.push(move);
            int score = -quiescence(br, -beta, -alpha, tt, kt, debug);
            br.pop();

            // beta cutoff
            if (score >= beta) {
                tt.record_hash(0, TTFlag::BETA_FLAG, beta, move, br.board.hash());
                return beta;
            }

            if (score > alpha) {
                flag = TTFlag::EXACT_FLAG;
                alpha = score;
                best_move = move;
            }
        }

        // store back into tt
        if (best_move != chess::Move::NULL_MOVE) {
            tt.record_hash(0, flag, alpha, best_move, br.board.hash());
        }

        return alpha;
    }

    // generate captures once here
    chess::Movelist captures;
    chess::movegen::legalmoves<chess::movegen::MoveGenType::CAPTURE>(captures, br.board);

    // check for checkmate or stalemate
    if (captures.empty()) {
        chess::Movelist legal_moves;
        chess::movegen::legalmoves(legal_moves, br.board);

        if (legal_moves.empty()) {
            return br.board.inCheck() ? -INF : 0;
        }
    }

    /* regular quiescence search */

    int stand_pat = evaluate(br);

    if (stand_pat >= beta)
        return beta;

    if (alpha < stand_pat)
        alpha = stand_pat;


    int score;
    chess::Move best_move = chess::Move::NULL_MOVE;
    TTFlag flag = TTFlag::ALPHA_FLAG;


    // recursive search
    for (const chess::Move& move : sort_captures(br, captures, tt, debug)) {

        debug.positions++;

        br.push(move);
        score = -quiescence(br, -beta, -alpha, tt, kt, debug);
        br.pop();

        // beta cutoff
        if (score >= beta) {
            tt.record_hash(0, TTFlag::BETA_FLAG, beta, move, br.board.hash());
            return beta;
        }

        if (score > alpha) {
            flag = TTFlag::EXACT_FLAG;
            alpha = score;
            best_move = move;
        }
    }

    // store back into tt
    if (best_move != chess::Move::NULL_MOVE) {
        tt.record_hash(0, flag, alpha, best_move, br.board.hash());
    }

    return alpha;
}



/**
 * negamax search - returns an evaluation
 * recursive function
 */
int negamax( BoardWrapper &br, int alpha, int beta, int depth, int ply, TranspositionTable &tt, KillerMovesTable &kt, Debug &debug) {

    // generate legal moves once here
    chess::Movelist legal_moves;
    chess::movegen::legalmoves(legal_moves, br.board);

    // check for checkmate or stalemate
    if (legal_moves.empty()) {
        return br.board.inCheck() ? -INF : 0;
    }

    // check for insufficient material
    if (br.board.isInsufficientMaterial()) {
        return 0;
    }

    // check for draws by 50 move rule
    if (br.board.isHalfMoveDraw()) {
        return 0;
    }

    // call quiescence at leaf node
    if (depth == 0) {
        if (br.board.inCheck()) {
            ++depth;
        } else {
            return quiescence(br, alpha, beta, tt, kt, debug);
        }
    }

    int score;

    // null move pruning
    if (depth > R && !br.board.inCheck() && br.last_move() != chess::Move::NULL_MOVE) {

        br.push_null();
        score = -negamax(br, -beta, -beta + 1, depth - R - 1, ply + R + 1, tt, kt, debug);
        br.pop();

        if (score >= beta) // check if there is a beta cutoff
            return beta;
    }

    // check transposition table
    if (auto tt_val = tt.probe_hash(depth, alpha, beta, br.board.hash())) {
        debug.tt_hits++;
        return tt_val.value();
    }

    // sort moves
    auto sorted_moves = sort_moves(br, legal_moves, tt, kt, debug, ply);

    chess::Move best_move = chess::Move::NULL_MOVE;
    TTFlag flag = TTFlag::ALPHA_FLAG;
    chess::Move move;

    // recursive search
    for (size_t i = 0; i < sorted_moves.size(); i++) {

        move = sorted_moves[i];

        debug.positions++;
        br.push(move);
        score = -negamax(br, -beta, -alpha, depth - 1, ply + 1, tt, kt, debug);
        br.pop();

        // beta cutoff
        if (score >= beta) {

            debug.beta_cutoff_move_num += i;
            debug.beta_cutoff_count++;
            debug.kt_move_stores++;

            tt.record_hash(depth, TTFlag::BETA_FLAG, beta, move, br.board.hash());

            if (!br.board.isCapture(move)) {
                kt.add_move(move, ply);
            }

            return beta;
        }

        if (score > alpha) {
            flag = TTFlag::EXACT_FLAG;
            alpha = score;
            best_move = move;
        }
    }

    debug.beta_cutoff_move_num += sorted_moves.size();
    debug.beta_cutoff_count++;

    if (best_move != chess::Move::NULL_MOVE) {
        tt.record_hash(depth, flag, alpha, best_move, br.board.hash());
    }

    return alpha;
}




/**
 * root negamax search function that returns a pair <move, evaluation>
 */
std::pair<chess::Move, int> root_search(BoardWrapper &br, int depth, int alpha, int beta, TranspositionTable &tt, KillerMovesTable &kt, Debug &debug) {

    // generate legal moves once here
    chess::Movelist legal_moves;
    chess::movegen::legalmoves(legal_moves, br.board);

    // check for checkmate or stalemate
    if (legal_moves.empty()) {
        return {chess::Move::NULL_MOVE, br.board.inCheck() ? -INF : 0};
    }

    // check for insufficient material or draws by 50 move rule
    if (br.board.isInsufficientMaterial() || br.board.isHalfMoveDraw()) {
        return {chess::Move::NULL_MOVE, 0};
    }


    chess::Move best_move_found = chess::Move::NULL_MOVE;
    TTFlag flag = TTFlag::ALPHA_FLAG;
    int ply = 0;
    // int orig_positions;
    chess::Move move;
    int score;
    bool is_repetition;

    auto sorted_moves = sort_moves(br, legal_moves, tt, kt, debug, ply);

    // recursive search
    for (size_t i = 0; i < sorted_moves.size(); i++) {

        move = sorted_moves[i];
        debug.positions++;

        br.push(move);
        score = -negamax(br, -beta, -alpha, depth - 1, ply + 1, tt, kt, debug);
        is_repetition = br.board.isRepetition();
        br.pop();

        // do not allow bot to repeat moves when winning
        if (evaluate(br) > 0 && is_repetition)
            continue;

        // beta cutoff
        if (score >= beta) {

            debug.beta_cutoff_move_num += i;
            debug.beta_cutoff_count++;

            // store in transposition table
            tt.record_hash(depth, TTFlag::BETA_FLAG, beta, move, br.board.hash());

            // store killer move
            if (!br.board.isCapture(move)) {
                kt.add_move(move, ply);
            }

            return {move, beta};
        }

        if (score > alpha) {
            flag = TTFlag::EXACT_FLAG;
            alpha = score;
            best_move_found = move;
        }
    }


    debug.beta_cutoff_move_num += sorted_moves.size();
    debug.beta_cutoff_count++;

    // store in transposition table at the end
    if (best_move_found != chess::Move::NULL_MOVE) {
        tt.record_hash(depth, flag, alpha, best_move_found, br.board.hash());
    }

    return {best_move_found, alpha};

}



/**
 * get the principal variation given the current board position and transposition table
 */
std::vector<chess::Move> get_pv_line(BoardWrapper &br, TranspositionTable &tt, int depth) {

    std::vector<chess::Move> pv;
    std::optional<chess::Move> best_move;


    // follow transposition table moves to get pv
    while (true) {

        best_move = tt.get_best_move(br.board.hash());
        if (!best_move.has_value())
            break;

        br.push(best_move.value());
        pv.push_back(best_move.value());

        if ((int) pv.size() > depth)
            break;
    }

    // undo pushes to get back to original board state
    for (chess::Move _ : pv) {
        br.pop();
    }

    return pv;
}



/**
 * main function in search.cpp
 * finds the best move for a given max depth for the current poisition
 * iterative deepning search
 */
chess::Move get_best_move(BoardWrapper &br, int max_depth) {


    TranspositionTable tt;
    KillerMovesTable kt;
    int alpha = -INF;
    int beta = INF;
    Debug debug = {};


    auto start_time = std::chrono::high_resolution_clock::now();
    std::cerr << "Current Evaluation: " << evaluate(br) << "\n";
    std::vector<chess::Move> pv_line;
    chess::Move best_move_found = chess::Move::NULL_MOVE;
    int score = 0;


    // iterative deepening
    for (int depth = 1; depth <= max_depth; depth++) {

        int search_alpha = alpha;
        int search_beta = beta;
        int num_researches = 0;

        // aspiration window re-search
        while (true) {

            auto result = root_search(br, depth, search_alpha, search_beta, tt, kt, debug);
            best_move_found = result.first;
            score = result.second;

            // use != -INF to prevent infinite loop
            // when score == search_alpha we need to re-search
            if (search_alpha != -INF && score <= search_alpha) { // fail low
                search_alpha = -INF;
                ++num_researches;
                continue;
            }

            // use != INF to prevent infinite loop
            // when score == search_beta we need to re-search
            if (search_beta != INF && score >= search_beta) { // fail high
                search_beta = INF;
                ++num_researches;
                continue;
            }
            break;
        }

        // get and print pv line
        pv_line = get_pv_line(br, tt, depth);
        if (depth == max_depth) {
            print_pv_line(pv_line);
        }

        // determine best move
        chess::Move display_move = (best_move_found != chess::Move::NULL_MOVE)
                ? best_move_found
                : (!pv_line.empty() ? pv_line[0] : chess::Move::NULL_MOVE);

        // debug print
        if (depth == max_depth) {
            std::cerr << "Depth " << depth << " complete with " << num_researches << " researches.\n";
            std::cerr << "Best move found is " 
                    << chess::uci::moveToUci(display_move) << ", nodes taken: " 
                    << debug.positions << ", score: " << score << "\n\n";
        }

        // update aspiration window
        alpha = score - WINDOW;
        beta = score + WINDOW;

    }


    /* timing and debug prints */

    auto end_time = std::chrono::high_resolution_clock::now();
    double seconds = std::chrono::duration<double>(end_time - start_time).count();  // seconds as double
    debug.time_seconds = seconds;                                                                                      

    double pps = (seconds > 0.0) ? (debug.positions / seconds) : std::numeric_limits<double>::infinity();
    debug.nodes_per_second = pps;

    debug.tt_length = tt.size();

    double avg_bcc = (debug.beta_cutoff_count > 0) ? (debug.beta_cutoff_move_num / static_cast<double>(debug.beta_cutoff_count))
                         : std::numeric_limits<double>::infinity();
    debug.average_beta_cutoff_move_num = avg_bcc;

    std::cerr << "NODES: " << debug.positions << "| TIME: " << debug.time_seconds << "| NPS: " << debug.nodes_per_second << "\n";

    return !pv_line.empty() ? pv_line.at(0) : best_move_found;
}
