*This project has been created as part of the 42 curriculum by nide-wil, adesmet.*

# push_swap

## Description

**push_swap** is a sorting project from the 42 curriculum. The goal is to sort a
list of integers using two stacks (`a` and `b`) and a limited set of operations,
while producing as few operations as possible.

The program embeds four strategies:

| Selector    | Strategy | Complexity class |
| ----------- | -------- | ---------------- |
| `--simple`  | Minimum extraction | O(n²) |
| `--medium`  | Chunk-based sorting | O(n√n) |
| `--complex` | LSD radix sort on ranks | O(n log n) |
| `--adaptive`| Chooses one of the three according to the measured disorder | depends on regime |

`--adaptive` is the default when no selector is given. The program also supports
`--bench`, which prints sorting statistics to `stderr` without polluting the
operation stream on `stdout`.

The implementation validates every argument, rejects duplicates and
out-of-range values, prints `Error` on `stderr` when needed, and does not use
global variables.

## Instructions

### Compilation

```sh
make        # build push_swap (and libft)
make bonus  # build the bonus checker
make clean  # remove object files
make fclean # remove object files and binaries
make re     # rebuild from scratch
```

The project requires a C compiler (`cc`) and `make`.

### Execution

```sh
./push_swap 3 1 2
./push_swap --simple 5 4 3 2 1
./push_swap --medium 4 67 3 87 23
./push_swap --complex 4 67 3 87 23
./push_swap --adaptive 4 67 3 87 23
```

The first argument is the top of stack `a`. Operations are written to `stdout`,
one per line. With no arguments the program prints nothing and exits normally.

### Benchmark mode

```sh
./push_swap --bench --complex 5 4 3 2 1 2>/tmp/bench.txt
cat /tmp/bench.txt
```

Example output on `stderr`:

```text
disorder: 100.00%
strategy: Complex (O(n log n))
total operations: 25
sa: 0
sb: 0
ss: 0
pa: 10
pb: 10
ra: 5
rb: 0
rr: 0
rra: 0
rrb: 0
rrr: 0
```

### Error handling

Invalid input (non-integers, values outside the `int` range, duplicates, unknown
flags) produces `Error` followed by a newline on `stderr`.

```sh
./push_swap --adaptive 0 one 2 3
./push_swap --simple 3 2 3
./push_swap 2147483648
```

### Bonus checker

The bonus `checker` program reads operations from `stdin`, applies them to the
stack given as arguments, and prints `OK` when stack `a` is sorted and stack `b`
is empty. It prints `KO` otherwise, and `Error` on `stderr` for invalid
arguments or unknown instructions.

```sh
make bonus
ARG="4 67 3 87 23"
./push_swap --complex $ARG | ./checker $ARG
printf "sa\npb\npa\n" | ./checker 2 1
```

## Algorithms

### Disorder metric

Before any move, the program computes

```text
disorder = (number of inverted pairs) / (total number of pairs)
```

where a pair `(i, j)` with `i < j` is inverted when `a[i] > a[j]`. This value is
between `0` (already sorted) and `1` (worst possible order) and is displayed
with two decimals in `--bench` mode.

### Simple — O(n²)

The minimum of stack `a` is located, rotated to the top with the cheapest
direction (`ra` or `rra`), and pushed to `b` with `pb`. This is repeated until
`a` is empty. Stack `b` is then drained back into `a` with `pa`, which places the
values in ascending order.

For each of the `n` extractions, finding and rotating the minimum costs at most
`O(n)`, hence an upper bound of `O(n²)` operations. Space usage is `O(n)` for
the stack arrays.

### Medium — O(n√n)

A sorted copy of the input is built and split into chunks of roughly `√n`
values. Chunks are processed in ascending order: while a chunk still has
elements in `a`, the program pushes the current top if it belongs to that chunk,
otherwise it rotates `a`. All pushed values therefore arrive in `b` grouped by
value range. Finally the maximum of `b` is repeatedly rotated to the top and
pushed back to `a`.

There are about `√n` chunks, and each pass over a stack costs at most `O(n)`
rotations, giving `O(n√n)` operations in the push_swap model. Space usage is
`O(n)` for the stacks plus `O(n)` for the sorted copy.

### Complex — O(n log n)

The algorithm computes the rank of every value (its index in the sorted input)
and performs an LSD radix sort on those ranks, from the least significant bit to
the most significant one. For each bit, values whose bit is `0` are pushed to
`b`, while values whose bit is `1` are rotated in `a`; afterwards `b` is pushed
back onto `a`. Ranks are found with a binary search in `O(log n)`.

There are `⌈log₂ n⌉` passes, each costing `O(n)` stack operations, for an upper
bound of `O(n log n)`. Space usage is `O(n)`.

### Adaptive — threshold justification

The adaptive strategy follows the subject's regimes:

| Disorder | Selected strategy | Complexity target |
| -------- | ----------------- | ----------------- |
| `< 0.2`  | Simple            | `O(n²)` |
| `0.2 ≤ d < 0.5` | Medium     | `O(n√n)` |
| `≥ 0.5`  | Complex           | `O(n log n)` |

Low-disorder inputs are almost sorted, so the quadratic minimum extraction
needs few rotations and remains cheap while keeping the implementation simple.
Medium disorder benefits from chunk partitioning, which reduces long rotations.
High disorder is handled by radix sort, whose cost grows predictably with
`n log n` and does not degrade on adversarial permutations. These thresholds
mirror the required complexity regimes and leave a safety margin around the
`0.2` and `0.5` boundaries.

### Measured performance

Random inputs on the reference environment:

| Input size | `--simple` | `--medium` | `--complex` |
| ---------- | ---------- | ---------- | ----------- |
| 100        | ~1550      | ~790       | ~1080       |
| 500        | ~32400     | ~8200      | ~6800       |

For 500 random values, `--complex` stays below the 8000-operation "good" target
and close to the 5500-operation "excellent" target; `--adaptive` selects it for
typical random permutations.

## Usage examples

```sh
# Sort and verify with the provided checker
ARG="4 67 3 87 23"; ./push_swap --complex $ARG | ./checker_linux $ARG

# Count operations
./push_swap --adaptive $(shuf -i 0-9999 -n 500) | wc -l

# Save benchmark output separately from the operation stream
./push_swap --bench --medium 9 1 8 2 7 3 2>/tmp/bench.txt >/tmp/moves.txt
```

## Resources

- 42 push_swap subject (v1.2).
- Big-O notation and algorithm analysis:
  <https://en.wikipedia.org/wiki/Big_O_notation>
- Radix sort:
  <https://en.wikipedia.org/wiki/Radix_sort>
- Chunk-based / bucket sorting:
  <https://en.wikipedia.org/wiki/Bucket_sort>
- Stack data structure:
  <https://en.wikipedia.org/wiki/Stack_(abstract_data_type)>
- `checker_linux`, the reference checker distributed with the subject.
- Norminette, the 42 coding-style checker.

### AI usage

AI was used only to generate and format this README. It was not used to design,
write, debug, or review the source code; the algorithms, tests, and fixes were
produced by the learners.

## Contributions

- **nide-wil**: stack representation, the eleven operations and their counters,
  disorder computation, the Simple and Complex strategies, adaptive dispatch, and
  correctness/performance testing.
- **adesmet**: argument parsing and validation, the Medium chunk-based strategy,
  benchmark output, the bonus checker, Makefile and libft integration, Norminette
  and leak verification, and README validation.
