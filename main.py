import chess
import chess.polyglot
import sys
import subprocess


"""
    get best move from c++ program
    no opening book
"""
def get_cpp_move(fen: str, depth: int) -> str:
    result = subprocess.run(
        ["./build/engine", fen, str(depth)],
        capture_output=True,
        text=True,
        check=True,
    )

    # stdout should contain only the move
    move = result.stdout.strip()

    # debug output is still available here if you want it
    debug_output = result.stderr

    print("DEBUG FROM C++:")
    print(debug_output)

    return move


"""
    get best move including opening book
"""
def get_computer_move(board: chess.Board, depth: int) -> chess.Move:
    try:
        return chess.polyglot.MemoryMappedReader("Titans.bin").weighted_choice(board).move # Opening book
    except IndexError:
        return chess.Move.from_uci(get_cpp_move(board.fen(), depth))


"""
    main program that runs the game loop
"""
def main(player_colour: bool, search_depth: int, start_fen : str | None = None) -> None:

    if (start_fen != None):
        board = chess.Board(start_fen)
    else:
        board = chess.Board()

    while not board.is_game_over():
        print(f"FEN: {board.fen()}")

        if board.turn == player_colour:
            move_uci = input()
            board.push_uci(move_uci)
            print(f"PLAYER played {move_uci}")

        else:
            move = get_computer_move(board, search_depth)
            board.push(move)
            print(f"COMPUTER played {move.uci()}")


# python3 main.py <player colour> <search depth>
if __name__ == "__main__":
    args = sys.argv

    if (len(args) not in (3, 4)):
        print("USAGE: python3 main.py <player colour> <search depth> optional<starting fen>")
        sys.exit()

    colour = args[1] == "white"
    if len(args) >= 4:
        fen = args[3]
        main(colour, int(args[2]), fen)
    else:
        main(colour, int(args[2]))
