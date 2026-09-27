# Complexity Analysis

All figures marked **measured** come from `output/program_output.txt` (n = 8, 8 queries).

---

## 1. Hashing

### 1.1 The operation itself

`h(k) = k mod m`

A remainder is a single hardware division instruction. In the C program the compiler
turns `key % m` into an `idiv`, one arithmetic instruction with no loop and no data
movement. Cost is **O(1)** time, O(1) space, independent of n and m.

Measured: every one of the 8 insertions and 8 searches evaluated the hash function
exactly once per key examined — there is no re-hashing, no probing inside the hash
function itself, and no resizing or rehash of the table.

### 1.2 Lookup in the table

Search is a probe sequence `h, h+1, h+2, … (mod m)`, stopping at the first empty slot
(a key cannot be further along, because linear probing always fills the first free slot).

| Case | Time | Space |
|---|:-----|:-----|
| Successful search, no collision | **O(1)** | O(1) |
| Successful search, inside a cluster of length L | **O(L)** | O(1) |
| Unsuccessful search | **O(1 + L)** worst case, O(1) best case | O(1) |
| Insertion | **O(1)** average, O(n) worst case | O(1) |
| Best / average / worst case | **O(1) / O(1) / O(n)** | O(1) per operation |

The worst case is linear because every key could pile into one slot, so a lookup for a
missing key — or for the last key of a full cluster — walks all m slots. Measured worst
case in this experiment: the query `840` in the m = 21 table, which needed **8 probes**.

### 1.3 Space complexity

The table is a single array of m slots, each holding a key, a home index and a probe
count.

- **Table:** O(m)
- Choosing m ≈ 1.5 × n (as done here) keeps the load factor near 0.67 and the table
  linear in n, so space is **O(n)**
- Extra space beyond the stored data: **O(1)** per operation — probing is done in place,
  with no auxiliary array and no recursion

For comparison, a sorted array of the same 8 IDs needs the same O(n) space, so hashing
buys time, not space.

---

## 2. Linear Search

Searching the raw unsorted array `[105, 210, 315, 420, 525, 630, 735, 840]`, one key
comparison per element, stopping at the match.

| Case | Comparisons | Time | Space |
|---|:------------|:-----|:-----|
| Best (key is first element) | 1 | O(1) | O(1) |
| Average (key at a random position) | (n+1)/2 = 4.5 | **O(n)** | O(1) |
| Worst (key last, or absent) | n = 8 | **O(n)** | O(1) |

- **Time complexity: O(n)** average *and* worst case
- **Space complexity: O(1)** auxiliary

Measured: 52 comparisons over 8 queries, average **6.50**. That average is above the
theoretical 4.5 because 4 of the 8 queries are absent, and every absent query costs the
full n = 8 comparisons. For a successful search on a uniformly random key, the expected
cost is (n+1)/2 = 4.5 — the two figures are consistent.

**Linear search on an unsorted array cannot beat O(n)**, and it degrades linearly: doubling
the library from 8 to 16 songs doubles the comparisons, whereas hashing leaves the cost
essentially unchanged.

---

## 3. Observed versus theoretical

| Quantity | Theoretical | Measured | Verdict |
|---|:---|:---|:---|
| Hash function `h(k)` | O(1) | 1 division per key | matches |
| Hashing, no collisions (m = 11) | O(1) per search | 1.00 probes for all 4 successful searches | matches |
| Hashing, clustered (m = 12) | O(1 + L), L = 2 | 1.88 probes average | matches |
| Hashing, all keys in one cluster (m = 21) | O(1 + L), L = 8 | up to 8 probes; 3.25 average | matches |
| Hashing worst case | O(n) | 8 probes with n = 8 | matches |
| Linear search best case | O(1) | 1 comparison (query 105) | matches |
| Linear search worst case | O(n) | 8 comparisons (4 queries) | matches |
| Linear search average | O(n), ≈ (n+1)/2 | 6.50 (inflated by the 4 absent queries) | matches |

**The measured values sit exactly on the theoretical bounds**, so the implementation is
efficient — there is no hidden waste. The important caveat is that the theory is stated in
terms of L, the cluster length, and L is *not* a function of the load factor alone.

---

## 4. Load factor

α = n / m = number of stored keys / table size

| m | α = 8/m | Slots used | Collisions | Longest cluster | Measured avg probes |
|:-:|:------:|:-----------:|:----------:|:---------------:|:--------------------:|
| 12 | **0.667** | 8 of 12 | 4 | 2 | 1.88 |
| 11 | **0.727** | 8 of 11 | 0 | 4 | 1.62 |
| 21 | **0.381** | 8 of 21 | 7 | 8 | 3.25 |

### The load factor does not explain the results

Ranking by load factor alone (lower is better): 21 (0.381) → 12 (0.667) → 11 (0.727).

Ranking by measured search cost (lower is better): 11 (1.62) → 12 (1.88) → 21 (3.25).

**These two rankings disagree.** The m = 21 table is the emptiest of the three, at 38%
occupancy, and yet it is the slowest to build (36 probes vs 8) and the slowest to search
(3.25 probes vs 1.62). α assumes keys are spread at random; these keys are not random —
they are consecutive multiples of 105, so `k mod 21` is 0 for all of them.

The predictor that actually fits the data is the **longest cluster** L:

| m | Longest cluster L | Avg probes measured | Probes the cluster explains |
|:-:|:------------------:|:-------------------:|:----------------------------:|
| 11 | 4 | 1.62 | 111 walks all 4 slots of `{1,2,3,4}`; every other query costs ≤ 2 |
| 12 | 2 | 1.88 | 735 and 840 each need 1 extra probe; 111 and 999 each walk 2 slots |
| 21 | 8 | 3.25 | 420, 735 and 840 must walk 4, 7 and 8 slots of the single cluster |

α is a useful *average* indicator across many random keys. For a single structured key set
like this one, L is the quantity that determines the worst case, and it is what should be
monitored.

### Standard guidance for keeping L small

- Keep α well below 1 — the usual threshold for switching to a more efficient collision
  scheme is α ≈ 0.7, and m = 12 at 0.667 sits right at that boundary
- Choose m so it shares no common factor with the key pattern. Since every song ID is a
  multiple of 105, avoid m ∈ {3, 5, 7, 15, 21, 35, …} — these send keys to slot 0 or to a
  handful of slots. Prefer a **prime m with no small factors**, e.g. 11, 13, 17
- Linear probing degrades gracefully but suffers primary clustering, the effect visible in
  m = 11 above. **Double hashing** (step = h2(k) = 1 + k mod (m − 1)) or **quadratic
  probing** would scatter the secondary clusters at a small extra cost
- When the key set grows, rebuild the table at roughly double the size — rehashing costs
  O(n) but restores O(1) lookups

---

## 5. Why the key set is the root cause

Every song ID is a multiple of 105 = 3 × 5 × 7. The division method is only well
distributed when m is coprime with the differences between the keys. Because the
differences here (105, 210, 315, …) are all multiples of 105, any m sharing a factor with
105 collapses several keys onto the same slot:

| m | gcd(m, 105) | h(k) values produced | Distinct slots used (of 8 keys) |
|:-:|:-----------:|:----------------------|:--------------------------------|
| 3 | 3 | all 0 | 1 |
| 5 | 5 | all 0 | 1 |
| 7 | 7 | all 0 | 1 |
| 11 | 1 | 6,1,7,2,8,3,9,4 | 8 (all distinct) |
| 12 | 3 | 9,6,3,0,9,6,3,0 | 4 |
| 21 | 21 | all 0 | 1 |

**m = 11 is coprime with 105 and gives a perfect distribution; every m tested that shares
a factor with 105 produces collisions.** Choosing a prime table size greater than the
number of keys protects against exactly this failure, and is the reason m = 11 is the
recommended configuration for this application.
