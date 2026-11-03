*This project has been created as part of the 42 curriculum by nide-wil, adesmet.*

# push_swap

## Description

**push_swap** is a sorting project from the 42 curriculum. The program receives a
list of integers, stores it in a stack (`a`), and prints the sequence of allowed
push_swap operations that sorts the values in ascending order in `a`.

Only two stacks and eleven operations are available: `sa`, `sb`, `ss`, `pa`,
`pb`, `ra`, `rb`, `rr`, `rra`, `rrb` and `rrr`. The quality of a solution is
measured by the number of operations it emits, so the project implements four
strategies with different complexity profiles:

| Selector     | Algorithm / role                                                     | Target class |
| ------------ | -------------------------------------------------------------------- | ------------ |
| `--simple`   | Selection sort / minimum extraction                                  | `O(n²)`      |
| `--medium`   | Chunk-based bucket sort                                              | `O(n√n)`     |
| `--complex`  | Turk-style greedy insertion (chunked presort + cost-guided insertion) | `O(n log n)` |
| `--adaptive` | Dispatcher selecting one of the three from the disorder              | depends      |

`--adaptive` is the default when no selector is provided. `--bench` prints
sorting statistics to `stderr` while the operation stream remains clean on
`stdout`.

The implementation validates every argument (integers only, `int` range,
duplicates), prints `Error` on `stderr` for invalid input, uses no global
variables, and frees all heap allocations.

## Instructions

### Requirements

- A C compiler (`cc`) and `make`.
- The project's `libft` in the `libft/` directory. It is linked by the Makefile
  and supplies the small set of libc-style helpers used by the program. In this
  repository `libft/` is recorded as a submodule; make sure it is populated
  before building.

### Compilation

```sh
make        # build push_swap
make bonus  # build the bonus checker
make clean  # remove object files
make fclean # remove object files and binaries
make re     # rebuild from scratch
```

The Makefile compiles with `cc -Wall -Wextra -Werror` and does not relink
unnecessarily.

### Execution

```sh
./push_swap 3 1 2
./push_swap --simple 5 4 3 2 1
./push_swap --medium 4 67 3 87 23
./push_swap --complex 4 67 3 87 23
./push_swap --adaptive 4 67 3 87 23
```

The first value is the top of stack `a`. Operations are written to `stdout`,
one per line. A single argument may also contain several values separated by
spaces:

```sh
./push_swap "4 67 3 87 23"
```

With no arguments, or when the input is already sorted, the program prints
nothing and exits normally. Selector and `--bench` flags may be placed among
the arguments; if several selectors are given, the last one wins.

### Benchmark mode

```sh
./push_swap --bench --adaptive 2 1 3 6 5 8 2>/tmp/bench.txt
cat /tmp/bench.txt
```

Example output on `stderr`:

```text
disorder: 13.33%
strategy: Adaptive -> Simple (O(n^2))
total operations: 16
sa: 0
sb: 0
ss: 0
pa: 6
pb: 6
ra: 3
rb: 0
rr: 0
rra: 1
rrb: 0
rrr: 0
```

When a selector is forced, the `Adaptive -> ` prefix is omitted and the
requested strategy is shown directly.

### Error handling

Invalid input prints `Error` followed by a newline on `stderr` and exits with a
non-zero status. Errors include non-integers, values outside the `int` range,
duplicate values, malformed number strings, and unknown `--flags`.

```sh
./push_swap --adaptive 0 one 2 3
./push_swap --simple 3 2 3
./push_swap 2147483648
./push_swap --unknown 1 2 3
```

### Bonus checker

The bonus `checker` reads operations from `stdin`, applies them to the stack
given as arguments, and prints `OK` when `a` is sorted and `b` is empty. It
prints `KO` otherwise, and `Error` on `stderr` for invalid arguments or unknown
or malformed instructions.

```sh
make bonus
ARG="4 67 3 87 23"
./push_swap --complex $ARG | ./checker $ARG
printf "sa\npb\npa\n" | ./checker 2 1
```

The reference binary `checker_linux` distributed with the subject can also be
used:

```sh
ARG="4 67 3 87 23"
./push_swap --complex $ARG | ./checker_linux $ARG
```

## Algorithms

### Disorder metric

Before emitting any move, the program computes the inversion ratio of stack `a`:

```text
disorder = (number of inverted pairs) / (total number of pairs)
```

A pair `(i, j)` with `i < j` is inverted when `a[i] > a[j]`. The result is
between `0` (sorted) and `1` (worst possible order). The computation costs
`O(n²)` comparisons and `O(1)` extra memory; it is not counted as push_swap
operations. In `--bench` mode the value is displayed as a percentage with two
decimals.

If the disorder is `0`, the program skips sorting entirely and emits no
operation, which matches the required behaviour for an already sorted stack.

### Simple — Selection sort / minimum extraction (`O(n²)`)

This is an adaptation of selection sort: instead of selecting the minimum and
placing it directly in its final position, the minimum is extracted and pushed
to `b`, then `b` is drained back into `a` in sorted order. The smallest element
of `a` is located, brought to the top with the cheaper
direction (`ra` if it is in the first half, `rra` otherwise), and pushed to `b`
with `pb`. This is repeated until `a` is empty. Stack `b` is then drained back
into `a` with `pa`, largest first, so the values end up in ascending order.

For each of the `n` extractions, finding and rotating the minimum costs at most
`O(n)` operations, giving an `O(n²)` operation bound. The two stack arrays use
`O(n)` space; no auxiliary array is needed.

**Why this strategy:** it is the baseline required by the subject. It is easy
to verify, uses the minimum number of pushes, and behaves well on inputs that
are already almost sorted, where the minimum is usually close to the top.

### Medium — Chunk-based bucket sort (`O(n√n)`)

This strategy is a bucket-sort adaptation: ranks are divided into value buckets
(chunks), each bucket is emptied from `a` in turn, and the buckets are then
pulled back in descending order. The values are first normalized: a sorted copy
is built with insertion sort and each value is replaced by its rank (`0 … n-1`).
Working with ranks makes chunk boundaries independent of the original value
range.

The rank space is split into `O(√n)` value bands:

```text
div        = max(2, floor(sqrt(n)) / 4)
chunk_size = n / div
```

For each band, the program scans `a`: values belonging to the current band are
pushed to `b`, all others are rotated with `ra`. Higher bands are therefore
pushed on top of lower ones. Once `a` is empty, the bands are pulled back in
descending order by `complex_pull`. The medium strategy uses a split ratio of
1, so each band is handled as a single range; every value is inserted with a
rotation-cost heuristic.

There are `O(√n)` bands and each band costs at most one `O(n)` pass, so the
presort emits `O(n√n)` operations. The pull also processes each band in
`O(n)` moves in the worst case, keeping the overall bound at `O(n√n)`. Space
usage is `O(n)` for the stacks plus `O(n)` for the temporary sorted copy.

**Why this strategy:** chunk partitioning is the classic way to break the
quadratic behaviour of repeated full scans. Using `O(√n)` bands is the
standard balance between the number of passes and the amount of work per band,
and the cost-guided pull avoids blindly rotating the whole stack for each
insertion.

### Complex — Turk-style greedy insertion (`O(n log n)` target)

The neutral name for this strategy is **chunked presort + cost-guided
insertion**; the overall greedy-insertion approach is commonly called
**Turk-style** in the 42 community. The complex strategy also normalizes values
to ranks, then uses a constant number of value bands. The number of bands and
the split ratio are chosen from the input size:

| Input size | Bands (`div`) | Split ratio (`fr`) |
| ---------- | ------------- | ------------------ |
| `n < 300`  | 2             | 3                  |
| `n < 450`  | 3             | 3                  |
| `n < 800`  | 4             | 3                  |
| `n ≥ 800`  | 4             | 4                  |

**Presort (`a` → `b`).** Bands are pushed in ascending order. A value below the
band limit is pushed to `b`; if it also belongs to the low `1/fr` part of the
band, it is immediately rotated with `rb` to the bottom of `b`. This leaves the
high part of each band near the top and the low part near the bottom.

**Pull (`b` → `a`).** Bands are processed from highest to lowest. Each band is
split at `base + chunk_size / fr`, and the upper part is pulled first, followed
by the lower part. For every candidate in the active range, the program
computes the rotation cost on both stacks and picks the cheapest one. Rotations
that go in the same direction are merged with `rr`/`rrr`. Because bands are
processed in descending order, the target position in `a` is the current
minimum, which stays at the top of `a`; only the final alignment may need
`ra`/`rra`. All insertions use `pa`.

With `div` bounded by 4, the presort performs at most a constant number of full
passes over `a`, i.e. `O(n)` operations. The band split used by the presort
mirrors the split used by the pull, so each requested range sits at one end of
`b` and the greedy selection needs only a bounded number of rotations; the pull
therefore also emits `O(n)` operations, followed by an `O(n)` final alignment.
The resulting operation count is linear with a constant factor that depends on
the fixed geometry (measured at roughly `5.7n` to `9.4n` in the table below),
which is inside the subject's `O(n log n)` class. Space usage is `O(n)` for the
stacks and the temporary sorted copy.

**Why this strategy:** building ordered value bands first removes the global
search cost from the insertion phase. The cost-based pull, combined with
`rr`/`rrr` merging and the split/presort mirror, keeps the number of emitted
operations low and stable on adversarial permutations, where the simple
strategy degrades quadratically.

### Adaptive — threshold justification

Adaptive is a dispatcher rather than a fourth sorting algorithm: it selects one
of the three implementations above according to the measured disorder. It
follows the regimes defined by the subject:

| Disorder        | Selected strategy | Target class |
| --------------- | ----------------- | ------------ |
| `< 0.2`         | Simple            | `O(n²)`      |
| `0.2 ≤ d < 0.5` | Medium            | `O(n√n)`     |
| `≥ 0.5`         | Complex           | `O(n log n)` |

Low-disorder inputs are almost sorted: the minimum extraction usually finds the
next value near the top, so its quadratic worst case is not reached and its
small constant makes it the cheapest choice. Medium disorder benefits from
chunk partitioning, which avoids repeatedly scanning the whole stack. High
disorder is handled by the banded complex strategy, whose cost is predictable
and does not explode on random or reverse-sorted permutations.

### Measured performance

Average operation counts over 10 runs of `shuf -i 0-999999 -n N` on the
development environment:

| Input size | `--simple` | `--medium` | `--complex` | `--adaptive` |
| ---------- | ---------- | ---------- | ----------- | ------------ |
| 100        | ~1419      | ~566       | ~568        | ~572         |
| 500        | ~31997     | ~4374      | ~3986       | ~4144        |
| 1000       | ~128020    | ~10490     | ~9406       | ~10007       |

For reference, the subject targets are fewer than 2000 operations for 100
values (1500 good, 700 excellent) and fewer than 12000 for 500 values (8000
good, 5500 excellent). The medium and complex strategies are well inside the
"excellent" range at 100 values and the "excellent" range at 500 values.

## Repository layout

| File(s)                              | Role                                                  |
| ------------------------------------ | ----------------------------------------------------- |
| `main.c`                             | Program entry point, sorting run and error handling   |
| `parsing*.c`                         | Flags, argument splitting, integer validation         |
| `stack.c`, `utils.c`                 | Stack representation, indexing and cost helpers       |
| `ops_*.c`                            | The eleven push_swap operations and their counters    |
| `disorder.c`                         | Inversion-ratio metric and sortedness check           |
| `sort_dispatcher.c`                  | Adaptive selection and strategy dispatch              |
| `sort_simple.c`                      | Selection sort / minimum extraction, `O(n²)`          |
| `sort_medium.c`                      | Chunk-based bucket sort, `O(n√n)`                     |
| `sort_complex.c`                     | Turk-style greedy insertion, `O(n log n)` target      |
| `complex_*.c`                        | Presort, move costs, insertion and pull phases        |
| `array_utils.c`, `sorts_utils.c`     | Sorted copy, rank normalization and binary search     |
| `bench.c`, `bench_utils.c`           | `--bench` output on `stderr`                          |
| `checker_*.c`                        | Bonus checker: parse, read and apply instructions     |
| `libft/`                             | 42 library used by the program                        |

## Resources

- 42 push_swap subject, version 1.2 (included as `en.subject.pdf`).
- Big-O notation and algorithm analysis:
  <https://en.wikipedia.org/wiki/Big_O_notation>
- Inversion counting and the disorder metric:
  <https://en.wikipedia.org/wiki/Inversion_(discrete_mathematics)>
- Chunk-based / bucket sorting:
  <https://en.wikipedia.org/wiki/Bucket_sort>
- Greedy algorithms and cost-based insertion:
  <https://en.wikipedia.org/wiki/Greedy_algorithm>
- Stack data structure:
  <https://en.wikipedia.org/wiki/Stack_(abstract_data_type)>
- `checker_linux`, the reference checker distributed with the subject.
- Norminette, the 42 coding-style checker.

### AI usage

An AI coding assistant was used during the project to help implement and
refactor parts of the code base (stack and operation handling, parsing, the
sorting strategies, benchmark output and the bonus checker) and to help draft
and format this README. It was also used to review the complexity arguments and
to suggest test scenarios.

The learners defined the requirements and algorithm choices, reviewed the
generated code, ran correctness and performance tests with the provided checker,
and validated the final behaviour. All algorithms and the code submitted here
are understood and can be explained by both contributors.

## Contributions

- **nide-wil**: stack representation, the eleven operations and their counters,
  disorder computation, the Simple and Complex strategies, adaptive dispatch,
  and correctness/performance testing.
- **adesmet**: argument parsing and validation, the Medium chunk-based strategy,
  benchmark output, the bonus checker, Makefile and libft integration,
  Norminette and leak verification, and README validation.
