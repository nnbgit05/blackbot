#include "debug.hpp"

#include <iostream>

/**
 * print debug struct
 */
void Debug::print() {
    std::cerr << "DEBUG" << "\n"
        << "\t" << "positions: " << this->positions << "\n"
        << "\t" << "time: " << this->time_seconds << "\n"
        << "\t" << "positions per second: " << this->nodes_per_second << "\n"
        << "\t" << "TT move orders: " << this->tt_move_orders << "\n"
        << "\t" << "TT hits: " << this->tt_hits << "\n"
        << "\t" << "TT length: " << this->tt_length << "\n"
        << "\t" << "KT move orders: " << this->kt_move_orders << "\n"
        << "\t" << "KT move stores: " << this->kt_move_stores << "\n"
        << "\t" << "avg beta cutoff move num: " << this->average_beta_cutoff_move_num << "\n";
}


/**
 * print pv line
 */
void print_pv_line(std::vector<chess::Move> pv) {
    std::cerr << "PV: ";
    for (size_t i = 0; i < pv.size(); i++) {
        std::cerr << chess::uci::moveToUci(pv[i]);
        if (i != pv.size() - 1)
            std::cerr << ", ";
    }
    std::cerr << "\n";
}

