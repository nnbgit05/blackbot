import subprocess
import sys
import time
from pathlib import Path

FENS = [
    # "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
    # "r3kb1r/pppb1ppp/2n1p3/qB6/3P4/1QP1PN2/P4PPP/R1B1K2R w KQkq - 0 1",
    # "r1bq1rk1/pppn1ppp/3bpn2/3p4/3P4/2NBPN2/PPQ2PPP/R1B2RK1 w - - 0 1",
    # "2r2rk1/1bqnbppp/p3pn2/1p6/3P4/1BN1PN2/PPQ2PPP/2RR2K1 w - - 0 1",
    # "r2q1rk1/pp2bppp/2npbn2/2p5/2P5/2NP1NP1/PP2PPBP/R1BQ1RK1 w - - 0 1",
    # "4rrk1/1pp1qppp/p1np1n2/8/2P5/1PN1PN2/PB1Q1PPP/2RR2K1 w - - 0 1",
    # "r1bqk2r/pppp1ppp/2n2n2/2b1p3/2B1P3/2NP1N2/PPP2PPP/R1BQ1RK1 w kq - 0 1",
    # "r2q1rk1/ppp2ppp/2npbn2/4p3/2B1P3/2NP1N2/PPP2PPP/R1BQ1RK1 w - - 0 1",
    # "2r2rk1/pp1nqppp/2pbpn2/3p4/3P4/2NBPN2/PPQ2PPP/2RR2K1 w - - 0 1",
    # "r1bq1rk1/pp3ppp/2n1pn2/2bp4/2P5/2NP1NP1/PPQ1PPBP/R1B2RK1 w - - 0 1",
    # "8/8/8/3k4/8/4K3/8/8 w - - 0 1",
    # "8/8/8/3k4/8/4K3/6P1/8 w - - 0 1",
    # "8/8/8/2k5/8/2K5/5P2/8 w - - 0 1",
    # "8/8/3k4/8/3K4/8/8/8 w - - 0 1",
    # "6k1/5ppp/8/8/8/8/5PPP/6K1 w - - 0 1",
    # "8/2p5/3p4/3P4/2P5/8/8/4K2k w - - 0 1",
    # "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1",
    # "4k3/8/8/3pP3/8/8/8/4K3 w - d6 0 1",
    # "8/8/8/1pP5/8/8/8/4K2k w - b6 0 1",
    # "rnbq1rk1/ppp2ppp/3bpn2/3p4/3P4/2NBPN2/PPP2PPP/R1BQ1RK1 w - - 0 1",
    # "r1bqk2r/pppp1ppp/2n5/2b1p3/2B1P1n1/2NP1N2/PPP2PPP/R1BQ1RK1 w kq - 0 1",
    # "2rq1rk1/pp2bppp/2npbn2/2p5/2P5/2NP1NP1/PP2PPBP/R1BQ1RK1 w - - 0 1",
    # "r4rk1/pp1n1ppp/2pbpn2/q7/3P4/1PN1PN2/PBQ2PPP/2RR2K1 w - - 0 1",
    # "3r2k1/5pp1/p2p1n1p/1p1P4/1P3P2/P1N3P1/5K1P/3R4 w - - 0 1",
    # "8/8/2k5/8/8/2K5/2Q5/7q w - - 0 1",
    "rn1qkb1r/pp2pppp/5n2/3p1b2/3P4/2N1P3/PP3PPP/R1BQKBNR w KQkq - 0 1",
    "rn1qkb1r/pp2pppp/5n2/3p1b2/3P4/1QN1P3/PP3PPP/R1B1KBNR b KQkq - 1 1",
    "r1bqk2r/ppp2ppp/2n5/4P3/2Bp2n1/5N1P/PP1N1PP1/R2Q1RK1 b kq - 1 10",
    "r1bqrnk1/pp2bp1p/2p2np1/3p2B1/3P4/2NBPN2/PPQ2PPP/1R3RK1 w - - 1 12",
    "rnbqkb1r/ppp1pppp/5n2/8/3PP3/2N5/PP3PPP/R1BQKBNR b KQkq - 3 5",
    "rnbq1rk1/pppp1ppp/4pn2/8/1bPP4/P1N5/1PQ1PPPP/R1B1KBNR b KQ - 1 5",
    "r4rk1/3nppbp/bq1p1np1/2pP4/8/2N2NPP/PP2PPB1/R1BQR1K1 b - - 1 12",
    "rn1qkb1r/pb1p1ppp/1p2pn2/2p5/2PP4/5NP1/PP2PPBP/RNBQK2R w KQkq c6 1 6",
    "r1bq1rk1/1pp2pbp/p1np1np1/3Pp3/2P1P3/2N1BP2/PP4PP/R1NQKB1R b KQ - 1 9",
    "rnbqr1k1/1p3pbp/p2p1np1/2pP4/4P3/2N5/PP1NBPPP/R1BQ1RK1 w - - 1 11",
    "rnbqkb1r/pppp1ppp/5n2/4p3/4PP2/2N5/PPPP2PP/R1BQKBNR b KQkq f3 1 3",
    "r1bqk1nr/pppnbppp/3p4/8/2BNP3/8/PPP2PPP/RNBQK2R w KQkq - 2 6",
    "rnbq1b1r/ppp2kpp/3p1n2/8/3PP3/8/PPP2PPP/RNBQKB1R b KQ d3 1 5",
    "rnbqkb1r/pppp1ppp/3n4/8/2BQ4/5N2/PPP2PPP/RNB2RK1 b kq - 1 6",
    "r2q1rk1/2p1bppp/p2p1n2/1p2P3/4P1b1/1nP1BN2/PP3PPP/RN1QR1K1 w - - 1 12",
    "r1bqkb1r/2pp1ppp/p1n5/1p2p3/3Pn3/1B3N2/PPP2PPP/RNBQ1RK1 b kq - 2 7",
    "r2qkbnr/2p2pp1/p1pp4/4p2p/4P1b1/5N1P/PPPP1PP1/RNBQ1RK1 w kq - 1 8",
    "r1bqkb1r/pp3ppp/2np1n2/4p1B1/3NP3/2N5/PPP2PPP/R2QKB1R w KQkq e6 1 7",
    "rn1qk2r/1b2bppp/p2ppn2/1p6/3NP3/1BN5/PPP2PPP/R1BQR1K1 w kq - 5 10",
    "r1b1kb1r/1pqpnppp/p1n1p3/8/3NP3/2N1B3/PPP1BPPP/R2QK2R w KQkq - 3 8",
    "r1bqnr2/pp1ppkbp/4N1p1/n3P3/8/2N1B3/PPP2PPP/R2QK2R b KQ - 2 11",
    "r3kb1r/pp1n1ppp/1q2p3/n2p4/3P1Bb1/2PB1N2/PPQ2PPP/RN2K2R w KQkq - 3 11",
    "r1bq1rk1/pppnnppp/4p3/3pP3/1b1P4/2NB3N/PPP2PPP/R1BQK2R w KQ - 3 7",
    "r2qkbnr/ppp1pp1p/3p2p1/3Pn3/4P1b1/2N2N2/PPP2PPP/R1BQKB1R w KQkq - 2 6",
    "rn2kb1r/pp2pppp/1qP2n2/8/6b1/1Q6/PP1PPPBP/RNB1K1NR b KQkq - 1 6"
]

CPP_CMD = ["./build/engine"]


def run_engine(fen: str, depth: int) -> tuple[int, str, str, float]:
    cmd = CPP_CMD + [fen, str(depth)]
    start = time.perf_counter()
    proc = subprocess.run(
        cmd,
        capture_output=True,
        text=True,
        check=False,
    )
    elapsed = time.perf_counter() - start
    return proc.returncode, proc.stdout, proc.stderr, elapsed


def main() -> int:
    if len(sys.argv) < 2:
        print("Usage: python3 run_cpp_suite.py <depth> [output_file]")
        return 1

    depth = int(sys.argv[1])
    output_file = Path(sys.argv[2]) if len(sys.argv) >= 3 else Path(f"cpp_suite_depth_{depth}.txt")

    total_wall_time = 0.0

    with output_file.open("w", encoding="utf-8") as f:
        f.write(f"C++ engine suite run\n")
        f.write(f"Depth: {depth}\n")
        f.write(f"Command: {' '.join(CPP_CMD)} <fen> <depth>\n")
        f.write("=" * 100 + "\n\n")

        for idx, fen in enumerate(FENS, start=1):
            rc, stdout, stderr, wall_time = run_engine(fen, depth)
            total_wall_time += wall_time

            f.write(f"Position {idx}\n")
            f.write(f"FEN: {fen}\n")
            f.write(f"Return code: {rc}\n")
            f.write(f"Wall time: {wall_time:.6f}s\n")
            f.write("-" * 100 + "\n")
            f.write(stdout if stdout.endswith("\n") else stdout + "\n")

            if stderr:
                f.write("\n[STDERR]\n")
                f.write(stderr if stderr.endswith("\n") else stderr + "\n")

            f.write("\n" + "=" * 100 + "\n\n")

        f.write("SUMMARY\n")
        f.write("-" * 100 + "\n")
        f.write(f"Positions run: {len(FENS)}\n")
        f.write(f"Total wall time: {total_wall_time:.6f}s\n")

    print(f"Wrote results to {output_file}")
    print(f"Total wall time: {total_wall_time:.6f}s")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
