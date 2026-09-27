# Trace Tables

Hash function: **Division method**, `h(k) = k mod m`
Collision resolution: **linear probing** (open addressing), probe sequence `h, h+1, h+2, … (mod m)`

Song IDs inserted: `105, 210, 315, 420, 525, 630, 735, 840` (n = 8)
Query IDs searched: `105, 420, 735, 840` (present) and `111, 500, 999, 1000` (absent)

**Counting rules** (identical for every table size, so the numbers are comparable):

- **Probe** = one examination of a table slot, whether the slot is empty, occupied by
  another key, or holds the key. The empty slot that terminates a failed search *is*
  counted, because it is examined in order to end the search.
- **Comparison** = one comparison of a query against a key in the raw unsorted array.

---

## Why three table sizes?

All eight song IDs are multiples of 105, so they share the prime factors **3, 5 and 7**.
This single fact decides the whole experiment, so the program runs three table sizes:

| m | Why chosen | What it shows |
|---|---|---|
| **12** | composite, 12 > 8 | the realistic case — a mix of collisions and small clusters |
| **11** | prime, 11 > 8 | the textbook recommendation — a perfect distribution |
| **21** | a multiple of 105 | the worst case — every key hashes to slot 0 |

Only `m = 12` produces a full after-every-insertion table in `output/program_output.txt`;
the other two print their insertion lines and final table.

---

## Part (a) — Hash function evaluation, m = 12

`h(k) = k mod 12`

| Song ID | Division | h(k) | Status |
|--------:|:---------|-----:|:-------|
| 105 | 105 = 12 × 8 + 9   |  9 | home slot |
| 210 | 210 = 12 × 17 + 6  |  6 | home slot |
| 315 | 315 = 12 × 26 + 3  |  3 | home slot |
| 420 | 420 = 12 × 35 + 0  |  0 | home slot |
| 525 | 525 = 12 × 43 + 9  |  9 | **collides with 105** |
| 630 | 630 = 12 × 52 + 6  |  6 | **collides with 210** |
| 735 | 735 = 12 × 61 + 3  |  3 | **collides with 315** |
| 840 | 840 = 12 × 70 + 0  |  0 | **collides with 420** |

Only 4 distinct home slots (0, 3, 6, 9) are used for 8 keys, so **4 collisions** are
guaranteed before a single key is inserted. Two keys per slot.

## Part (a) — Insertion trace, m = 12

| # | Key | h(k) | Home slot | Slots probed | Stored at | Probes | Collision? |
|--:|----:|-----:|:---------:|:-------------|:---------:|-------:|:----------:|
| 1 | 105 | 9 | 9 | 9 | 9 | 1 | no |
| 2 | 210 | 6 | 6 | 6 | 6 | 1 | no |
| 3 | 315 | 3 | 3 | 3 | 3 | 1 | no |
| 4 | 420 | 0 | 0 | 0 | 0 | 1 | no |
| 5 | 525 | 9 | 9 | 9 → 10 | **10** | 2 | **yes** (105 in slot 9) |
| 6 | 630 | 6 | 6 | 6 → 7 | **7** | 2 | **yes** (210 in slot 6) |
| 7 | 735 | 3 | 3 | 3 → 4 | **4** | 2 | **yes** (315 in slot 3) |
| 8 | 840 | 0 | 0 | 0 → 1 | **1** | 2 | **yes** (420 in slot 0) |

Total probes to build the table = 1+1+1+1+2+2+2+2 = **12**, average **1.50** per key.

### Table state after each insertion, m = 12

`.` = empty slot

| slot | after 1 (105) | after 2 (210) | after 3 (315) | after 4 (420) | after 5 (525) | after 6 (630) | after 7 (735) | after 8 (840) |
|-----:|:--------------|:--------------|:--------------|:--------------|:--------------|:--------------|:--------------|:--------------|
| 0  | . | . | . | 420 | 420 | 420 | 420 | 420 |
| 1  | . | . | . | . | . | . | . | **840** |
| 2  | . | . | . | . | . | . | . | . |
| 3  | . | . | 315 | 315 | 315 | 315 | 315 | 315 |
| 4  | . | . | . | . | . | . | **735** | 735 |
| 5  | . | . | . | . | . | . | . | . |
| 6  | . | 210 | 210 | 210 | 210 | 210 | 210 | 210 |
| 7  | . | . | . | . | . | **630** | 630 | 630 |
| 8  | . | . | . | . | . | . | . | . |
| 9  | 105 | 105 | 105 | 105 | 105 | 105 | 105 | 105 |
| 10 | . | . | . | . | **525** | 525 | 525 | 525 |
| 11 | . | . | . | . | . | . | . | . |

**Final table (m = 12), load factor α = 8/12 = 0.667**

| slot | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 |
|:-----|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
| key  | 420 | 840 | . | 315 | 735 | . | 210 | 630 | . | 105 | 525 | . |
| home | 0 | 0 | - | 3 | 3 | - | 6 | 6 | - | 9 | 9 | - |

Four primary clusters, each 2 slots long: `{0,1}`, `{3,4}`, `{6,7}`, `{9,10}`.

---

## Insertion trace, m = 11 (prime) — no collisions

`h(k) = k mod 11`

| # | Key | h(k) | Stored at | Probes | Collision? |
|--:|----:|-----:|:---------:|-------:|:----------:|
| 1 | 105 | 6 | 6 | 1 | no |
| 2 | 210 | 1 | 1 | 1 | no |
| 3 | 315 | 7 | 7 | 1 | no |
| 4 | 420 | 2 | 2 | 1 | no |
| 5 | 525 | 8 | 8 | 1 | no |
| 6 | 630 | 3 | 3 | 1 | no |
| 7 | 735 | 9 | 9 | 1 | no |
| 8 | 840 | 4 | 4 | 1 | no |

Total probes = **8**, average **1.00**. **Zero collisions.**

| slot | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 |
|:-----|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
| key  | . | 210 | 420 | 630 | 840 | . | 105 | 315 | 525 | 735 | . |

**Zero collisions at insert time, yet two clusters of 4 slots** (`{1,2,3,4}` and
`{6,7,8,9}`). Because the song IDs are consecutive multiples of 105, the function
`105k mod 11` is also consecutive, so keys that never collided during insertion end up
adjacent. Clustering is a property of the *final layout*, not of the insertion events.

---

## Insertion trace, m = 21 (multiple of 105) — every key collides

`h(k) = k mod 21 = 0` for all eight keys, because 105 = 5 × 21.

| # | Key | h(k) | Slots probed | Stored at | Probes | Collision? |
|--:|----:|-----:|:-------------|:---------:|-------:|:----------:|
| 1 | 105 | 0 | 0 | 0 | 1 | no |
| 2 | 210 | 0 | 0 → 1 | 1 | 2 | **yes** |
| 3 | 315 | 0 | 0 → 1 → 2 | 2 | 3 | **yes** |
| 4 | 420 | 0 | 0 → 1 → 2 → 3 | 3 | 4 | **yes** |
| 5 | 525 | 0 | 0 → … → 4 | 4 | 5 | **yes** |
| 6 | 630 | 0 | 0 → … → 5 | 5 | 6 | **yes** |
| 7 | 735 | 0 | 0 → … → 6 | 6 | 7 | **yes** |
| 8 | 840 | 0 | 0 → … → 7 | 7 | 8 | **yes** |

Total probes = 1+2+3+4+5+6+7+8 = **36**, average **4.50** per key — nine times the cost of
m = 12. **7 collisions**, one single cluster of 8 slots, slots 0–7.

This is the case that breaks the load-factor rule of thumb: α = 0.381 is the *lowest* of
the three sizes, yet this table is the slowest to build and the slowest to search.

---

## Part (b) — Search trace

### m = 12 (primary table)

| Query | In data? | Linear search: comparisons | Hashing: probes | Probes performed |
|------:|:--------:|--------------------------:|----------------:|:-----------------|
| 105 | present |  1 | 1 | slot 9 → match |
| 420 | present |  4 | 1 | slot 0 → match |
| 735 | present |  7 | 2 | slot 3 (315) → slot 4 → match |
| 840 | present |  8 | 2 | slot 0 (420) → slot 1 → match |
| 111 | absent  |  8 | 3 | slot 3 (315) → slot 4 (735) → slot 5 empty → stop |
| 500 | absent  |  8 | 1 | slot 8 empty → stop |
| 999 | absent  |  8 | 3 | slot 3 (315) → slot 4 (735) → slot 5 empty → stop |
| 1000 | absent |  8 | 2 | slot 4 (735) → slot 5 empty → stop |
| **Total** | | **52** | **15** | |
| **Average** | | **6.50** | **1.88** | |

Linear search saved **37** operations (52 − 15), a **3.47×** speed-up.

Note how the unsuccessful queries behave. `111` and `999` both hash to slot 3, the head
of the cluster that 315 and 735 formed, so they must walk the whole cluster before
reaching the empty slot that proves absence. `500` hashes to slot 8, which is empty, and
is rejected after a single probe. **The same "not found" answer costs 1 probe or 3 probes
depending only on where the key happens to land.**

### m = 11 (prime, no collisions)

| Query | In data? | Linear: comparisons | Hashing: probes | Probes performed |
|------:|:--------:|--------------------:|----------------:|:-----------------|
| 105 | present | 1 | 1 | slot 6 → match |
| 420 | present | 4 | 1 | slot 2 → match |
| 735 | present | 7 | 1 | slot 9 → match |
| 840 | present | 8 | 1 | slot 4 → match |
| 111 | absent  | 8 | 5 | slots 1,2,3,4 occupied → slot 5 empty → stop |
| 500 | absent  | 8 | 1 | slot 5 empty → stop |
| 999 | absent  | 8 | 2 | slot 9 (735) → slot 10 empty → stop |
| 1000 | absent | 8 | 1 | slot 10 empty → stop |
| **Total** | | **52** | **13** | |
| **Average** | | **6.50** | **1.62** | |

Every successful search costs exactly **1 probe** — the theoretical optimum. Query `111`
still costs 5 probes because it lands on the head of the 4-slot cluster `{1,2,3,4}`, a
reminder that a collision-free insertion phase does not guarantee a collision-free
search phase.

### m = 21 (worst case)

| Query | In data? | Linear: comparisons | Hashing: probes | Probes performed |
|------:|:--------:|--------------------:|----------------:|:-----------------|
| 105 | present | 1 | 1 | slot 0 → match |
| 420 | present | 4 | 4 | slots 0,1,2,3 → match at 3 |
| 735 | present | 7 | 7 | slots 0…6 → match at 6 |
| 840 | present | 8 | 8 | slots 0…7 → match at 7 |
| 111 | absent  | 8 | 3 | slot 6 (735), slot 7 (840) → slot 8 empty → stop |
| 500 | absent  | 8 | 1 | slot 17 empty → stop |
| 999 | absent  | 8 | 1 | slot 12 empty → stop |
| 1000 | absent | 8 | 1 | slot 13 empty → stop |
| **Total** | | **52** | **26** | |
| **Average** | | **6.50** | **3.25** | |

The successful searches degrade to 1, 4, 7 and 8 probes purely because of where the keys
sit inside one long cluster. The absent keys are cheap by accident — they land on empty
slots far from the cluster.

Both routines returned the same found/not-found verdict for all 8 queries at every table
size, which confirms the hash table stores and retrieves exactly the same set of song IDs
as the array.

---

## Part (c) — Summary of the measured work

| Table size | n | m | Load factor α | Collisions | Longest cluster | Probes to build | Total search probes | Avg probes/query | Total linear comparisons |
|:----------:|:-:|:-:|:-------------:|:----------:|:---------------:|:----------------:|:-------------------:|:-----------------:|:------------------------:|
| 12 | 8 | 12 | 0.667 | 4 | 2 | 12 | 15 | 1.88 | 52 |
| 11 | 8 | 11 | 0.727 | 0 | 4 | 8 | 13 | 1.62 | 52 |
| 21 | 8 | 21 | 0.381 | 7 | 8 | 36 | 26 | 3.25 | 52 |

The load factor ranks the tables 21 < 12 < 11 (best to worst occupancy), but the measured
search cost ranks them 11 < 12 < 21. **The load factor alone does not predict search
performance** — the distribution of the keys matters just as much.
