# PmergeMe (Ford–Johnson / Merge-Insertion) — Visual Guide

This README explains the exact algorithm shape used in this project (`vector` and `deque` versions).

---

## 1) Big Picture (1-screen version)

```text
Input numbers
   │
   ├─ Pair groups and compare only the LAST element (representative)
   │
   ├─ In each pair: smaller-rep group goes left (b), larger-rep goes right (a)
   │
   ├─ Recurse on doubled group size (sorts winners by representative)
   │
   ├─ Build main chain: [b1, a1, a2, a3, ...]
   │
   ├─ Insert remaining losers [b2, b3, ...] in Jacobsthal order
   │   using bounded binary search (search only up to paired winner)
   │
   ├─ Insert straggler (if odd group count)
   │
   └─ Rebuild array in chain order
```

---

## 2) Vocabulary (quick)

- **Group (size g)**: contiguous block treated as one unit.
- **Representative**: the group’s last value (`group[groupSize - 1]`).
- **Pair**: two neighboring groups.
- **Loser (`b`)**: pair’s smaller representative.
- **Winner (`a`)**: pair’s larger representative.
- **Pend**: losers waiting for insertion (`b2..bk`).
- **Chain**: main ordered list of group-start indices.

Visual:

```text
g=1: [x] [y] [z]
g=2: [x x] [y y] [z z]
g=4: [x x x x] [y y y y]
```

---

## 3) What one recursion level does

Assume `groupSize = g`.

### A) Pair and normalize

```text
Before pairs: [G0][G1][G2][G3][G4]...
Compare reps of (G0,G1), (G2,G3), ...
If rep(left) > rep(right) -> swap whole groups.

Result per pair: [b_i][a_i] with rep(b_i) <= rep(a_i)
```

### B) Recurse with `groupSize = 2g`

```text
This sorts the winners (a1, a2, ...) by representative,
while each winner still carries its matched loser next to it.
```

### C) Build structures after recursion

```text
chain = [b1, a1, a2, a3, ...]
pend  = [b2, b3, b4, ...]
```

### D) Insert pend in Jacobsthal order

For each `b_k`, binary search only in prefix ending before `a_k`.

### E) Optional straggler

If odd number of groups existed, insert final leftover group into full chain.

### F) Materialize

Copy groups to a temp buffer and rewrite current segment in `chain` order.

---

## 4) Minimal worked trace

Input:

```text
[7 3 9 4 8 2 5]
```

Pairing (`g=1`):

```text
(7,3)->(3,7)  (9,4)->(4,9)  (8,2)->(2,8)  straggler: 5
=> [3 7 | 4 9 | 2 8 | 5]
   b1 a1  b2 a2  b3 a3
```

After recursive winner ordering:

```text
[b1 a1 | b3 a3 | b2 a2 | 5]
```

Build:

```text
chain = [b1, a1, a2, a3]
pend  = [b2, b3]
```

Insert pend in Jacobsthal order (for this size -> `b3`, then `b2`), then insert straggler.

Final:

```text
[2 3 4 5 7 8 9]
```

---

## 5) Jacobsthal section (why it exists)

### 5.1 Sequence

```text
J(0)=0, J(1)=1
J(n)=J(n-1)+2*J(n-2)

0, 1, 1, 3, 5, 11, 21, 43, ...
```

### 5.2 What the code builds

The code does **block insertion in reverse** using Jacobsthal boundaries.

For `pairCount = 8` (pairs indexed `1..8`):

```text
processed = 1
J = 3  -> insert 2..3 in reverse: 3,2
J = 5  -> insert 4..5 in reverse: 5,4
J = 11 -> insert 6..8 in reverse: 8,7,6

Order: 3,2,5,4,8,7,6
```

Equivalent 0-based pair index order (as used internally) is the shifted variant.

### 5.3 Exactly how Jacobsthal decides how many items are pushed

In code, each Jacobsthal step pushes this many pending items:

```text
pushCount = blockEnd - processed
```

because the loop is:

```text
for (i = blockEnd; i > processed; --i)
   push_back(i - 1)
```

So Jacobsthal values do not directly push one value each. They define block boundaries,
and each block contributes multiple pushes.

### 5.4 16-element showcase (exact push counts)

Take 16 input values (example):

```text
[16 12 18 1 8 3 10 2 17 13 19 14 11 5 15 7]
```

At `groupSize = 1`:

```text
groupCount = 16
pairCount  = 8
pend size  = pairCount - 1 = 7   (these are b2..b8)
```

Now Jacobsthal windowing in this implementation:

```text
processed = 1
J = 3  -> blockEnd = 3  -> pushCount = 3 - 1 = 2  -> push: 3,2
J = 5  -> blockEnd = 5  -> pushCount = 5 - 3 = 2  -> push: 5,4
J = 11 -> blockEnd = 8* -> pushCount = 8 - 5 = 3  -> push: 8,7,6

Total pushes = 2 + 2 + 3 = 7 = pend size
(* capped to pairCount)
```

So for 16 elements, Jacobsthal boundaries generate exactly 7 insertion indices,
which is exactly the number of pending losers to insert.

### 5.5 Why this helps

Each loser `b_k` is guaranteed `< a_k`, so search area is bounded to prefix before `a_k`.
Jacobsthal ordering chooses insertion order so these prefixes are close to sizes of form:

```text
2^m - 1
```

Binary search on `2^m - 1` needs exactly `m` comparisons in the worst case.
That makes comparison counts near-optimal/minimal for this strategy.

---

## 6) Recursion on a global scale

Think of recursion as levels where `groupSize` doubles each time:

```text
Level 0: groupSize = 1   -> compare/swap inside pairs of 1
Level 1: groupSize = 2   -> compare/swap inside pairs of 2
Level 2: groupSize = 4   -> compare/swap inside pairs of 4
...
```

At each level, only the representative (last element of each group) is compared.
If a group has a smaller representative, that full group moves to the loser side (`b`),
otherwise to the winner side (`a`).

Global effect across all levels:

- Winners become recursively ordered at coarser and coarser group sizes.
- Losers are not discarded; they are reinserted later with bounded binary search.
- Any odd leftover group (straggler) is postponed and inserted at the end of that level.

When recursion unwinds, each level rebuilds its segment in chain order, so local ordering
constraints established at deeper levels are preserved while pending groups are inserted.

## 7) Why vector and deque can have different time

The algorithmic steps are the same, but container internals differ:

- `vector`: contiguous memory, cache-friendly indexing.
- `deque`: segmented memory, weaker locality.
- Same comparisons does **not** mean same wall-clock time.

So it is normal that `deque` can be slower even on small inputs.

---

## 8) Code map

- `buildJacobsthalInsertionOrder` / `buildJacobsthalInsertionOrderDeque`
- `fordJohnsonRecurse` / `fordJohnsonRecurseDeque`
- `PmergeMe::parseArgs`
- `PmergeMe::run` (prints before/after and timings)

---

## 9) Output format

```text
Before: ...
After:  ...
Time to process a range of N elements with std::vector : X us
Time to process a range of N elements with std::deque  : Y us
```

---

## 10) Resources

- https://hsm.stackexchange.com/questions/18457/who-first-showed-the-famous-best-worst-case-complexity-on-sorting-algorithms
- https://arxiv.org/abs/1905.09656?utm_source=chatgpt.com
- https://www.sciencedirect.com/science/article/abs/pii/S0020019006002742?via%3Dihub
- https://www.jstor.org/stable/2308750
