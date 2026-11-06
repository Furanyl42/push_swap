*This project has been created as part of the 42 curriculum by adesmet, nide-wil.*

# push_swap

## Description

`push_swap` sorts a list of distinct integers using two stacks (`a` and `b`) and
only the eleven authorized operations (`sa`, `sb`, `ss`, `pa`, `pb`, `ra`, `rb`,
`rr`, `rra`, `rrb`, `rrr`). The program prints the shortest operation sequence
produced by the selected strategy on standard output.

The project embeds four strategies:

- **Simple** — an elementary selection-based approach, `O(n²)`.
- **Medium** — a chunk-based bucket sort, `O(n√n)`.
- **Complex** — a normalized chunk sort with cost-based insertion, `O(n log n)`.
- **Adaptive** — chooses one of the three strategies from the measured disorder
  of the input, which keeps the operation count low on already-sorted and
  nearly-sorted data.

The binary never prints anything on standard output except valid operations.
Diagnostics and the optional benchmark go to standard error.

A bonus `checker` program is also provided. It reads operations on standard input
and reports `OK` when they sort the given stack, `KO` otherwise.

## Instructions

### Compilation

```sh
make          # build ./push_swap
make bonus    # build ./checker (bonus)
make test     # build and run the local debug binary (not required for grading)
make clean    # remove object files
make fclean   # remove object files and binaries
make re       # full rebuild
```

The project is written in C and compiled with `cc -Wall -Wextra -Werror`.
`libft` and `ft_printf` are built from their own Makefiles and then linked into
the final binary.

### Execution

```sh
./push_swap 3 2 1
./push_swap "4 67 3 87 23"
./push_swap --simple 5 4 3 2 1
./push_swap --medium 5 4 3 2 1
./push_swap --complex 5 4 3 2 1
./push_swap --adaptive 5 4 3 2 1
./push_swap --bench --complex 5 4 3 2 1
```

Arguments may be given as separate integers or as one quoted string containing
space-separated integers. Duplicates, non-integers and values outside the
`int` range are rejected with `Error` on standard error and exit status `1`.

### Benchmark mode

`--bench` keeps the operation stream on standard output and writes the following
information to standard error after sorting:

- the computed disorder percentage (two decimals);
- the selected strategy and its theoretical complexity class;
- the total number of operations;
- the number of each operation type (`sa`, `sb`, `ss`, `pa`, `pb`, `ra`, `rb`,
  `rr`, `rra`, `rrb`, `rrr`).

Example:

```sh
$ ./push_swap --bench --adaptive 5 4 3 2 1 > /dev/null
Disorder: 100.00%
Strategy: Complex
Complexity: O(n log n)
Total operations: 18
sa: 0
sb: 0
...
```

When the input is already sorted (or contains at most one element), no operation
is printed and the benchmark reports the strategy that would have been selected
with zero operations.

## Bonus: checker

`checker` takes the same integer arguments as `push_swap`, reads instructions from
standard input (one per line) and executes them on the two stacks. It prints:

- `OK` on standard output when `a` is sorted and `b` is empty;
- `KO` on standard output when the operations do not sort the stacks;
- `Error` on standard error when the arguments or an instruction are invalid.

```sh
$ ./push_swap --adaptive 4 67 3 87 23 | ./checker 4 67 3 87 23
OK
$ printf 'sa\n' | ./checker 3 2 1
KO
```

Without arguments, `checker` exits silently. The bonus sources live in
`bonus/*_bonus.c` and `bonus/checker_bonus.h`, and are compiled only by the
`bonus` rule.

## Algorithms

### Normalization

Both the medium and complex strategies first replace the values by their rank in
the sorted input (`0 … n-1`). Ranks are stored in the `idx` array of the stack,
which makes comparisons and chunk boundaries independent of the actual values.

### Simple strategy — `O(n²)`

The simple strategy repeatedly finds the minimum element of `a`, rotates it to
the top with the cheapest direction (`ra` or `rra`), pushes it to `b`, and finally
pushes everything back to `a`. Each of the `n` iterations scans and rotates a
stack of size `O(n)`, hence `O(n²)` operations in the worst case. This strategy
is intentionally simple and is used as a reference or for very small inputs.

### Medium strategy — `O(n√n)`

The medium strategy uses a bucket sort. After normalization, the ranks are split
into chunks of size roughly `1.5√n`:

1. Rotate `a`, pushing the current chunk to `b` and rotating `b` when the pushed
   rank is in the lower part of the chunk.
2. Once `a` is empty, repeatedly bring the maximum of `b` to its top and push it
   back to `a`.

Chunk construction costs `O(n√n)`, and pulling the maximum of each chunk costs
`O(n√n)` as well, for a total of `O(n√n)` operations. The `1.5` factor was tuned
on random inputs to keep the medium regime comfortably below the 5500-operation
target at `n = 500`, without changing the complexity class.

### Complex strategy — `O(n log n)`

The complex strategy first pre-sorts the ranks into chunks (2 to 4 chunks
depending on `n`) so that `b` is already partially ordered. It then pulls the
ranges back from the highest to the lowest rank. For each candidate value it
computes:

- the cost of bringing the insertion target to the top of `a`;
- the cost of bringing the candidate to the top of `b`;
- the combined cost, taking advantage of `rr` and `rrr` when both rotations go
  in the same direction.

The cheapest candidate is inserted with the shortest sequence, which yields
`O(n log n)` operations in practice and in the upper bound of the chunk-based
scheme.

### Adaptive strategy

The adaptive strategy measures the *disorder* of the input: the fraction of
unordered pairs of elements. If `d` denotes this fraction:

| Disorder | Strategy | Rationale |
|---|---|---|
| `d < 0.2` | Simple | The input is nearly sorted, so the `O(n²)` scan is cheap in practice and avoids the overhead of chunk construction. |
| `0.2 ≤ d < 0.5` | Medium | A partially disordered array benefits from a square-root decomposition without paying for the full cost-based insertion. |
| `d ≥ 0.5` | Complex | Strongly disordered inputs need the `O(n log n)` strategy to stay below the benchmark thresholds. |

These thresholds (0.2 and 0.5) were chosen from measurements on random,
reversed and nearly-sorted inputs: they keep the adaptive mode within the
"excellent" bracket at `n = 100` and `n = 500` while preserving a simple code
path for nearly-sorted data. The thresholds are documented here so the choice can
be reviewed and justified during the defence.

### Complexity summary

| Strategy | Time (operations) | Extra space |
|---|---|---|
| Simple | `O(n²)` | `O(n)` |
| Medium | `O(n√n)` | `O(n)` |
| Complex | `O(n log n)` | `O(n)` |
| Adaptive | `O(n log n)` worst case | `O(n)` |

The two stacks themselves use `O(n)` space, and the rank array adds another
`O(n)`. The sorting helpers use a single `O(n)` temporary array during
normalization, so the total extra space is `O(n)`.

## Error handling

The parser rejects:

- non-integer arguments;
- integers smaller than `INT_MIN` or larger than `INT_MAX`;
- duplicate values;
- empty or whitespace-only arguments;
- unknown options.

On error, `Error` is written to standard error and the program exits with status
`1`. Calling the program without arguments exits silently with status `0`.

## Tests

Every command below is meant to be run from the repository root, after building
the project. The blocks can be copied and pasted as-is.

### Build and basic sorting

```sh
make fclean
make
make bonus

# Every strategy must produce a sorted stack.
for flag in --simple --medium --complex --adaptive; do
    ARG="5 4 3 2 1"
    ./push_swap "$flag" $ARG | ./checker $ARG
done

# An already sorted stack produces no operation.
test -z "$(./push_swap 1 2 3 4 5)" && echo "sorted: OK"

# No argument: no output, exit status 0.
test -z "$(./push_swap)" && echo "no args: OK"
```

### Error handling

```sh
expect_error()
{
    err=$(mktemp)
    if "$@" >/dev/null 2>"$err"; then
        echo "FAIL (no error): $*"
        rm -f "$err"
        return 1
    fi
    if ! grep -qx 'Error' "$err"; then
        echo "FAIL (wrong message): $*"
        rm -f "$err"
        return 1
    fi
    rm -f "$err"
    echo "error OK: $*"
}

expect_error ./push_swap 0 one 2 3
expect_error ./push_swap --simple 3 2 3
expect_error ./push_swap 2147483648
expect_error ./push_swap -2147483649
expect_error ./push_swap ""
expect_error ./push_swap --unknown 1 2 3
```

### Checker

```sh
# Examples from the subject.
printf 'rra\npb\nsa\nrra\npa\n' | ./checker 3 2 1 0   # OK
printf 'sa\nrra\npb\n' | ./checker 3 2 1 0             # KO

# push_swap output must be accepted by the checker.
for flag in --simple --medium --complex --adaptive; do
    ARG=$(shuf -i 0-9999 -n 100 | tr '\n' ' ')
    ./push_swap "$flag" $ARG | ./checker $ARG
done

# Unknown instruction: Error on stderr and exit status 1.
if printf 'xx\n' | ./checker 3 2 1 >/dev/null 2>/tmp/checker_err; then
    echo "FAIL: invalid instruction accepted"
else
    grep -qx 'Error' /tmp/checker_err && echo "invalid instruction: OK"
fi
```

### Benchmark

```sh
ARG=$(shuf -i 0-9999 -n 500 | tr '\n' ' ')
./push_swap --bench --medium $ARG > /tmp/ops.txt 2> /tmp/bench.txt

test "$(wc -l < /tmp/ops.txt)" -eq \
     "$(awk '/Total operations/ {print $3}' /tmp/bench.txt)" \
    && echo "bench total: OK"
```

The benchmark is written to stderr, so the operation stream stays clean on
stdout.

### Performance

This Python 3 script runs the default (adaptive) strategy several times and checks
the excellent thresholds: 700 operations for `n = 100` and 5500 for `n = 500`.

```python
import random
import statistics
import subprocess

random.seed(42)  # reproducible samples


def run(n, limit, trials=50):
    counts = []
    for _ in range(trials):
        values = random.sample(range(-1_000_000, 1_000_001), n)
        result = subprocess.run(
            ["./push_swap"] + [str(v) for v in values],
            capture_output=True, text=True, check=True)
        counts.append(len(result.stdout.split()))
    print(f"n={n}: min={min(counts)} median={statistics.median(counts):.0f} "
          f"max={max(counts)} limit={limit}")
    if max(counts) >= limit:
        raise SystemExit(f"FAIL: n={n} exceeded the excellent threshold")
    print(f"n={n}: PASS")


for size, threshold in [(100, 700), (500, 5500)]:
    run(size, threshold)
```

Save the block above as `perf_test.py` and run:

```sh
python3 perf_test.py
```

### Memory (optional)

```sh
ARG=$(shuf -i 0-9999 -n 500 | tr '\n' ' ')
valgrind --leak-check=full --error-exitcode=42 \
    ./push_swap --adaptive $ARG > /dev/null

./push_swap --adaptive $ARG > /tmp/ops.txt
valgrind --leak-check=full --error-exitcode=42 \
    ./checker $ARG < /tmp/ops.txt
```

Both commands must report `ERROR SUMMARY: 0 errors` and no leaks. The local test
binary can also be run with:

```sh
make test
```

## Resources

Classic references used for this project:

- [Big-O notation](https://en.wikipedia.org/wiki/Big_O_notation)
- [Sorting algorithm stability and complexity](https://en.wikipedia.org/wiki/Sorting_algorithm)
- [Bucket sort](https://en.wikipedia.org/wiki/Bucket_sort)
- [Insertion sort](https://en.wikipedia.org/wiki/Insertion_sort)
- [42 Norm](https://github.com/42School/norminette)
- [push_swap visualizer](https://github.com/o-reo/push_swap_visualizer)

### AI usage

AI was used only to format this README (Markdown structure and presentation).
It was not used to write, debug, review or test the code, the algorithms or the
test commands.

The algorithms, data structures and strategy thresholds were designed and
validated by both learners, who remain responsible for every line submitted.

## Contributions

- **adesmet** — parsing and error handling; stack implementation and operation
  counters; simple and medium strategies; Makefile and Norm cleanup; memory
  leak fixes.
- **nide-wil** — complex strategy (normalization, chunk pre-sort, cost-based
  insertion and alignment); adaptive disorder metric and threshold selection;
  benchmark output; legality checker and test harness.
- **Both** — bonus `checker`: argument parsing, instruction reader, silent stack
  operations and OK/KO decision, validated against the reference checker.

Both learners reviewed the final code, ran the correctness and performance
tests, and can explain any part of the project during the defence.
