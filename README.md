# BlueBot2

Rewrite of [BlueTot/bluebot-chess-engine](https://github.com/BlueTot/bluebot-chess-engine) in C++ along with extra optimisations. Uses [Disservin](https://github.com/Disservin/chess-library)'s chess move generation library. This is currently a work in progress!

## Installation (linux only)

Clone the repository

```bash
git clone https://github.com/BlueTot/bluebot2
```

Build from source in release mode (install `cmake` if not already)

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Install python-chess (requires `pip3`, `python3`)

```
pip3 install python-chess
```

## Running the application

The engine can be queried directly (without opening book) with this command:

```
./build/engine <fen> <max-search-depth>
```


Alternatively, there is a *two-player* version where player vs computer (with opening book). Player must input moves in UCI form (e.g. e2e4)

```
python3 main.py <player's turn> <search depth> <starting fen>
```

where:

* player's turn is either *white* or *black*
* starting fen is optional, by default it is the starting position

## Performance

* Rated at least 2000+ chess.com in 10+0 rapid
* Beat 2100+ rated players on chess.com and Lichess Stockfish 6 (~2100) in 10+0 rapid
* Beat Komodo 18 (2200 rated) on chess.com
* Can play comfortably with depth 9, avg. 2M nps (on my machine)

## Features

- Alpha-beta fail-hard negamax search
- Quiescence search (with consideration for when side to move is in check)
- Evaluation function with PST
- Move ordering
- Captures move ordering using MVV-LVA
- Null move pruning
- Transposition table with principal variation move ordering, for both negamax and quiescence search
- Killer heuristic move ordering
- Iterative deepening
- Aspiration windows with proper re-searching
- Use of *Titans* opening book
