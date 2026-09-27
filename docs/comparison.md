# Comparison Table

All figures measured from `output/program_output.txt`: n = 8 song IDs, 8 queries
(4 present, 4 absent).

---

## 1. Hashing versus linear search — the main comparison

| Aspect | Hashing (division method + linear probing) | Linear search |
|---|:---|:---|
| Data structure | Hash table, m slots | Raw unsorted array, n elements |
| Lookup mechanism | compute `h(k) = k mod m`, then probe | compare against each element in turn |
| Best case | **O(1)** — 1 probe (hit on home slot) | **O(1)** — 1 comparison |
| Average case | **O(1 + L)** ≈ 1.88 probes (m = 12) | **O(n)** — 6.50 comparisons measured |
| Worst case | **O(n)** — 8 probes (m = 21, key 840) | **O(n)** — 8 comparisons |
| Unsuccessful search | 1–5 probes depending on where the key lands | always n = 8 comparisons |
| Probes, present keys (105, 420, 735, 840) | 1, 1, 2, 2 = **6** | 1, 4, 7, 8 = **20** |
| Probes, absent keys (111, 500, 999, 1000) | 3, 1, 3, 2 = **9** | 8, 8, 8, 8 = **32** |
| **Total operations (8 queries)** | **15** | **52** |
| **Average per query** | **1.88** | **6.50** |
| **Speed-up** | **3.47× fewer operations** | — |
| Space complexity | O(m) = O(n) for the table, O(1) extra per operation | O(n) for the array, O(1) extra |
| Insertion cost | O(1) average, O(n) worst | O(n) — a sorted insert must shift elements |
| Sorted order available | no | no (unless the array is sorted separately) |
| Scaling behaviour | cost stays ~1 per lookup as n grows | cost grows linearly with n |
| Implementation complexity | moderate — needs a table, a probe rule, a load-factor policy | trivial |

---

## 2. The three table sizes

| m | 12 | 11 | 21 |
|:--|:--|:--|:--:|
| Type | composite | **prime** | multiple of 105 |
| Load factor α = 8/m | 0.667 | 0.727 | **0.381** (emptiest) |
| Collisions during insertion | 4 | **0** | 7 |
| Probes to build the table | 12 | **8** | 36 |
| Longest cluster L | **2** | 4 | 8 |
| Total probes, 8 queries | 15 | **13** | 26 |
| Average probes per query | 1.88 | **1.62** | 3.25 |
| Total linear comparisons | 52 | 52 | 52 |
| Speed-up over linear search | 3.47× | **4.00×** | 2.00× |
| Correct verdicts (of 8) | 8/8 | 8/8 | 8/8 |

**Every configuration is correct; they differ only in cost.** Hashing beat linear search
at all three sizes, but the margin ranged from 2.00× to 4.00× depending purely on how the
table size interacts with the keys.

---

## 3. Observed versus theoretical

| Quantity | Theoretical | Measured | Agrees? |
|---|:---|:---|:---:|
| Hash function `h(k)` | O(1) | 1 division | yes |
| Hashing, collision-free (m = 11) | O(1) | 1.00 probes per successful search | yes |
| Hashing, cluster length 2 (m = 12) | O(1 + L) | 1.88 probes | yes |
| Hashing, one 8-slot cluster (m = 21) | O(1 + L) | 3.25 probes, worst 8 | yes |
| Hashing worst case | O(n) | 8 probes with n = 8 | yes |
| Linear search best | O(1) | 1 comparison | yes |
| Linear search average | O(n), (n+1)/2 = 4.5 | 6.50 (4 of 8 queries absent, costing n each) | yes |
| Linear search worst | O(n) | 8 comparisons | yes |
| Space, both structures | O(n) | 12-slot table vs 8-element array | yes |

---

## 4. Load factor versus measured performance

| m | α (rank, best first) | Avg probes (rank, best first) | Agrees with α? |
|:-:|:---------------------|:------------------------------|:---------------|
| 11 | 3rd (0.727) | **1st (1.62)** | no |
| 12 | 2nd (0.667) | 2nd (1.88) | yes |
| 21 | **1st (0.381)** | 3rd (3.25) | **no** |

The m = 21 table has the lowest load factor and the worst search performance. **α alone
is not a sufficient predictor**; the longest cluster L and the coprimality of m with the
key structure decide the outcome.

---

## 5. Requirement coverage

| Requirement | Implementation | Where |
|---|:---|:---|
| Hash table, division method | `hash_div()` — `key % m` | `src/hash_table.c` |
| Insert the given song IDs | `insert_key()` with linear probing | `src/hash_table.c` |
| Table after each insertion | `print_table()` called per insert for m = 12 | `output/program_output.txt` |
| Identify collisions | `*collision` flag in `insert_key()`, counted in `build_table()` | output, part (a) |
| Search by hashing | `search_hash()` | output, part (b) |
| Search by linear search | `search_linear()` | output, part (b) |
| Operations per search | probes and comparisons returned by each routine | output, part (b) |
| Collision effect on search | probe counts vs cluster length L | `docs/complexity_analysis.md` §4 |
| Load factor | α = n/m reported for each size | output, part (c) |
| Compare with theory | measured vs O(1)/O(n) | `docs/complexity_analysis.md` §3 |
| Is hashing suitable? | justified recommendation | `docs/conclusion.md` |
