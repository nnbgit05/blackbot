#include "../third_party/chess.hpp"

#include "../include/evaluation.hpp"
#include "../include/board.hpp"
#include "../include/search.hpp"


int main(int argc, char *argv[]) {

    if (argc != 3)
        return 1;

    std::string fen = argv[1];
    int max_depth = std::stoi(argv[2]);
    BoardWrapper br(fen);
    chess::Move best_move = get_best_move(br, max_depth);
    std::cout << chess::uci::moveToUci(best_move) << "\n";

    return 0;
}

