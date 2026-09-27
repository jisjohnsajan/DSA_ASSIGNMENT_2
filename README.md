# Hash Table Using the Division Method — Music Application Song IDs

DSA Assignment, Question 1 (15 marks)

A music application stores song IDs `105, 210, 315, 420, 525, 630, 735, 840`.
This repository implements a hash table with the **division method** `h(k) = k mod m`,
resolves collisions by **linear probing**, and compares hashing against linear search by
counting the operations each one performs.

## Headline result

| | Hashing (m = 11) | Linear search |
|---|:---|:---|
| Total operations, 8 queries | **13** | 52 |
| Average per query | **1.62** | 6.50 |
| Worst case per query | **5** | 8 |
| Successful-search cost | **1 probe each** | 1, 4, 7, 8 comparisons |

**Hashing is 4.00× cheaper and — unlike linear search — does not get slower as the song
library grows.** The full justification is in [`docs/conclusion.md`](docs/conclusion.md).

The catch found by this experiment: all eight IDs are multiples of 105, so the table size
matters enormously. A table of 21 slots is the *emptiest* of the three tested (α = 0.381)
and the *slowest* (3.25 probes per query), because every ID hashes to slot 0. Use a prime
m > n.

## Repository layout

| Path | Contents |
|---|:---|
| `src/hash_table.c` | the complete C implementation (parts a, b and c) |
| `input/song_ids.txt` | the song IDs and the 8 query IDs |
| `output/program_output.txt` | full program output — insertion traces, search tables, statistics |
| `docs/trace_tables.md` | hand-traced tables: hash evaluation, insertion after every step, per-query probe paths |
| `docs/complexity_analysis.md` | time and space complexity, observed vs theoretical, load factor analysis |
| `docs/comparison.md` | side-by-side comparison tables |
| `docs/conclusion.md` | final judgement and recommendation |

## How to build and run

Requires a C11 compiler (tested with GCC 15.2.0 on Windows/MinGW).

```sh
make run
```

or without `make`:

```sh
gcc -std=c11 -Wall -Wextra -O2 -o build/hash_table src/hash_table.c
./build/hash_table input/song_ids.txt
```

Pass a different data file as the argument if you want to try other song IDs:

```sh
./build/hash_table path/to/other_ids.txt
```

The input file format is documented at the top of `input/song_ids.txt`: `[SONGS]` holds the
IDs to store, `[QUERIES]` holds the IDs to search for, and `#` starts a comment.

## What the program does

Three table sizes are exercised, because the choice of m is the crux of the question:

| m | Why | Outcome |
|:-:|---|:---|
| 12 | composite, > n | 4 collisions, longest cluster 2 — the realistic case, traced in full |
| 11 | prime, > n | 0 collisions — the recommended configuration |
| 21 | a multiple of 105 | every key hashes to slot 0 — the worst case |

For each size it prints the hash value of every key, the table state after every insertion,
whether each insertion collided, a per-query search table counting operations for both
hashing and linear search, and the load factor, cluster statistics and speed-up.

### Counting rules

Kept identical across all table sizes so the numbers are directly comparable.

- **Probe** — one examination of a table slot, whether empty, occupied by another key, or
  holding the match. The empty slot that terminates a failed search **is** counted.
- **Comparison** — one comparison of a query against a key in the unsorted array.

### Hash function and collision resolution

```c
h(k) = k % m                          /* division method            */
probe sequence: h, h+1, h+2, ... (mod m)   /* linear probing        */
```

## Verification

Every reported figure was recomputed independently and matches the program's output:
collision counts 4 / 0 / 7, hash probe totals 15 / 13 / 26, linear comparisons 52, and
all 8 found/not-found verdicts identical between the two search routines at every table size.
The program compiles with `-Wall -Wextra` and produces no warnings.
