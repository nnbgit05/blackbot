# Blackbot

A chess engine with a C++20 search core and a Python command-line interface for playing against it.

## Features

- Iterative deepening with alpha-beta negamax search
- Quiescence search, including positions where the side to move is in check
- Piece-square-table evaluation
- Move ordering with MVV-LVA, principal variation and killer moves
- Null move pruning and transposition tables
- Aspiration windows with re-searching
- Opening-book support through the Python interface

## Build and run

The following commands target Linux. Install a C++20 compiler, CMake 3.20 or newer, Python 3 and pip.

```bash
git clone https://github.com/nnbgit05/blackbot.git
cd blackbot
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
pip3 install python-chess
```

Query the engine directly, without the opening book. Quote the FEN because it contains spaces:

```bash
./build/engine "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" 6
```

Play as White at search depth 6:

```bash
python3 main.py white 6
```

Use `black` to play as Black. An optional third argument supplies a quoted starting FEN. Enter moves in UCI notation, such as `e2e4`. Run from the repository root so the interface can find `build/engine` and `Titans.bin`.

## Project structure

- `src/` and `include/`: C++ engine implementation
- `main.py`: Python game loop and opening-book integration
- `run_cpp_suite.py` and `logs/`: test harness and recorded outputs
- `third_party/chess.hpp`: bundled chess library
- `Titans.bin`: opening book

## Performance

The repository includes recorded test outputs in `logs/`. These are useful development records, but do not establish a standardized Elo rating. Reproducible strength comparisons should specify the engine versions, hardware, time controls and full match results.

## Dependencies and credits

Blackbot uses [Disservin's chess-library](https://github.com/Disservin/chess-library), bundled in `third_party/chess.hpp` under its MIT license. Its copyright and license notice are retained in that file.

The Python interface uses `python-chess` and the bundled Titans opening book.

## License

See [LICENSE](LICENSE) for the repository's GPL-3.0 license. Third-party components retain their own notices.
