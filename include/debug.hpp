#ifndef DEBUG_HPP
#define DEBUG_HPP

#include "../third_party/chess.hpp"
#include <vector>

struct Debug {

    int tt_move_orders;
    int kt_move_orders;
    int positions;
    int tt_hits;
    int beta_cutoff_move_num;
    int beta_cutoff_count;
    int kt_move_stores;
    double time_seconds;
    int tt_length;
    double nodes_per_second;
    double average_beta_cutoff_move_num;

    void print();
};

void print_pv_line(std::vector<chess::Move> pv);


#endif
