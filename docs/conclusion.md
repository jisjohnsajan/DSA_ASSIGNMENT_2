# Conclusion — Is Hashing Suitable for This Application?

## Verdict

**Yes. Hashing is the right structure for this application, provided the table size is
chosen as a prime number greater than the number of stored song IDs — concretely
m = 11 or 13 for this data set.**

Measured over 8 queries against 8 stored song IDs:

| | Hashing (m = 11) | Linear search |
|---|:---|:---|
| Total operations | **13** | 52 |
| Average per query | **1.62** | 6.50 |
| Worst case per query | **5** | 8 |
| Successful-search cost | **1 probe each** | 1, 4, 7, 8 comparisons |

Hashing is 4.00× cheaper on average, and — more importantly — its cost does **not** grow
when the song library grows. Doubling the library to 16 songs leaves a hash lookup at
1–2 probes while linear search rises to 8–16 comparisons. For a music application that
must look up a song by ID on every play, that is the decisive property.

## The one condition attached to the verdict

Hashing is not unconditionally O(1) here. The verdict depends on the table size being
chosen so that the song IDs spread out, and the data set actively conspires against that:

**All eight song IDs are multiples of 105.** Any table size sharing a factor with 105
sends several keys to the same slot. The experiment makes the consequence measurable:

| m | α | Collisions | Longest cluster | Avg probes | Verdict |
|:-:|:--:|:----------:|:---------------:|:----------:|:--------|
| 21 | 0.381 | 7 | 8 | 3.25 | **bad** — all keys hash to slot 0 |
| 12 | 0.667 | 4 | 2 | 1.88 | acceptable — near the 0.7 threshold |
| 11 | 0.727 | 0 | 4 | 1.62 | **best** — every successful search costs 1 probe |

The m = 21 table is the clearest lesson in the whole assignment: it is the *emptiest* of
the three (α = 0.381) and the *slowest* (3.25 probes). Load factor alone does not predict
search performance — the longest cluster L does. A hash table that looks half empty can
still behave almost linearly if the keys are structured.

The rule that resolves this is standard and cheap: **choose m prime, with m > n.**
m = 11 is coprime with 105, so `105k mod 11` is distinct for every k, and the table
degrades to the theoretical O(1) best case.

## Secondary findings

1. **Zero collisions at insertion does not guarantee cheap searches.** In the m = 11
   table every key landed on its own home slot, yet the layout formed two clusters of 4
   adjacent slots, because the IDs are consecutive multiples of 105 and therefore hash to
   consecutive slots. Query `111` still cost 5 probes. Clustering is a property of the
   final layout, not of the insertion events.

2. **Failed searches are the real cost driver.** All four absent queries cost 8
   comparisons under linear search — the maximum — because the array must be exhausted.
   Under hashing they cost 1–3 probes, but only because they happened to land near an
   empty slot. A missing key that hashes into the middle of a long cluster costs O(L),
   the same worst case as a present key. In a real music app (searching for a track that
   was deleted, or mistyped) absent keys are common, which favours hashing further.

3. **Both routines are provably correct here.** At every table size the hash table
   returned the same found/not-found verdict as the array for all 8 queries, so the
   speed-up comes from doing less work, not from answering differently.

## Final recommendation

| Decision | Recommendation |
|---|:---|
| Use hashing? | **Yes** |
| Table size | prime, m > n — use **11 or 13** for 8 songs |
| Collision resolution | linear probing is fine; switch to **double hashing** if α approaches 0.7 |
| Growth policy | rebuild at m ≈ 2n when the library doubles (O(n) rebuild restores O(1) lookup) |
| Guard against | never pick m that shares a factor with 105 — avoid 3, 5, 7, 15, 21, 35 |
| Alternative considered | sorted array + **binary search**, O(log n) ≈ 3 comparisons for n = 8, but needs the array kept sorted on every insert; hashing wins for an insert-heavy, lookup-heavy workload |

Hashing is suitable for this application. It delivers a 4.00× measured reduction in
operations, holds every successful lookup at a single probe, and — unlike linear search —
stays constant as the song library grows. The whole benefit rests on one decision: use a
prime table size that is coprime with the song-ID pattern.
