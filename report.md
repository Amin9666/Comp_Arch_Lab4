# CSE 3031 Computer Architecture  
## Lab Project 4: Dynamic Branch Predictors  
**Student:** Amin  
**Due:** May 5, 2026  

---

## Introduction

Branch prediction is a critical technique in modern processors that allows the pipeline to speculatively execute instructions without waiting for a branch outcome to be resolved. Accurate branch prediction reduces pipeline stalls and improves instruction-level parallelism. In this lab, two dynamic branch predictors are implemented and evaluated: the **2-bit Saturating Counter (2SC)** predictor and the **GShare** predictor.

### 2-Bit Saturating Counter (2SC) Predictor

The 2-bit saturating counter predictor maintains a Branch History Table (BHT) indexed by bits from the branch's Program Counter (PC). Each entry in the table is a 2-bit saturating counter that represents one of four states:

| Value | State                |
|-------|----------------------|
| 0     | Strongly Not Taken   |
| 1     | Weakly Not Taken     |
| 2     | Weakly Taken         |
| 3     | Strongly Taken       |

A prediction of **Taken** is made when the counter value is ≥ 2; otherwise **Not Taken**. On training, the counter is incremented (saturating at 3) if the branch was taken, or decremented (saturating at 0) if not taken.

**Configuration:** 14-bit PC index → 2¹⁴ = 16,384 entries × 2 bits = **32,768 bits (32K bits)**.

### GShare Predictor

The GShare predictor uses a Global History Register (GHR) that tracks the outcomes of the most recent N branch instructions. The BHT index is computed by XOR-ing the GHR with bits from the PC. This correlation between global history and the PC allows GShare to distinguish between branches at the same PC address that behave differently based on the path taken to reach them.

**Index formula:** `index = (GHR ^ PC) & 0x3FFF`

**Configuration:** 14-bit GHR XOR 14-bit PC → 2¹⁴ = 16,384 entries × 2 bits + 14-bit GHR = **(32K + 14) bits**.

All entries and the GHR are initialized to 0 (Strongly Not Taken, no history).

---

## Method

### Implementation

Both predictors implement the same interface (`init_predictor()`, `make_prediction()`, `train_predictor()`) defined in the provided header template.

- **predictor_2sc.h / predictor_2sc.C** — 2-bit saturating counter predictor
- **predictor_gshare.h / predictor_gshare.C** — GShare predictor

The makefile was extended to build two separate executables (`predictor_2sc` and `predictor_gshare`) from the same `main.C` and `traceread.C` driver code.

### Testing Plan

To verify correctness of the predictors, the following test strategy was used:

1. **Unit-level sanity checks:**
   - *Always-not-taken trace:* All branches not taken. The 2SC predictor should converge to predicting Not Taken after a warm-up, and the gshare predictor should similarly converge. Both should show near-0% misprediction rate at steady state.
   - *Always-taken trace:* All branches taken. After the initial warm-up (at most 2 wrong predictions per entry), both predictors should reach near-0% misprediction rate.
   - *Alternating trace:* Branches alternate taken/not-taken. The 2SC predictor with 2-bit counters will oscillate between states 1 and 2 (WN↔WT), mispredicting every other branch → ~50% misprediction. GShare, using global correlation, should do better if the alternating pattern correlates across branches.

2. **Trace-level evaluation:** Six synthetic trace files are used as benchmarks:
   - `DIST-INT-1`, `DIST-INT-2` — integer workload traces (biased toward taken, varied bias per PC)
   - `DIST-FP-1`, `DIST-FP-2` — floating-point workload traces (highly biased, predictable)
   - `DIST-MIXED` — mixed workload (heterogeneous bias, many PCs)
   - `DIST-CORR` — correlated trace (branch outcome depends on recent global history)

3. **Correctness check for GShare GHR update:** The GHR is updated in `train_predictor()` (after the prediction), using the actual outcome. The prediction uses the GHR state at the time of the branch, which matches how hardware would pipeline the update.

### How to Build and Run

```bash
# Build all executables
make

# Run on a trace file (binary format)
./predictor_2sc /path/to/trace.trace

# Run on a bzip2-compressed trace (original format)
bunzip2 -c -k ../traces/DIST-INT-1.bz2 | ./predictor_2sc
```

---

## Results

### Misprediction Rates (%)

The table below shows the misprediction rate (%) for each predictor on each trace, along with the arithmetic mean.

| Trace       | 2-bit Sat. Counter (%) | GShare (%) |
|-------------|------------------------|------------|
| DIST-INT-1  | 28.703                 | 41.098     |
| DIST-INT-2  | 28.685                 | 41.470     |
| DIST-FP-1   | 17.989                 | 27.364     |
| DIST-FP-2   | 14.634                 | 22.666     |
| DIST-MIXED  | 35.393                 | 48.610     |
| DIST-CORR   | 49.429                 | 11.393     |
| **Mean**    | **29.139**             | **32.100** |

> **Note:** These results are from synthetic traces. On real SPEC benchmark traces, gshare typically achieves 5–10% lower misprediction rates than 2SC because real programs contain many correlated branches that the global history exploits effectively. The synthetic `DIST-CORR` trace demonstrates this advantage clearly (11.4% for gshare vs. 49.4% for 2SC).

### Performance Comparison Graph

```
Misprediction Rate (%) by Trace
60 |
   |                                    ■ (49.4)
50 |                                              ■ (48.6)
   |
40 |         □ (41.1)  □ (41.5)
   |
30 | ■ (28.7) ■ (28.7)          □ (27.4)
   |                    ■ (18.0)          □ (22.7)
20 |                    ■ (14.6)
   |                                                        □ (11.4)
10 |
   |
 0 +----+----+----+----+----+----+----+----+----+----+----+----> Trace
     INT-1  INT-2  FP-1  FP-2  MIXED  CORR

  ■ = 2-bit Saturating Counter     □ = GShare
```

*(See bar chart below for clearer visualization)*

**Bar Chart — Misprediction Rate per Trace:**

```
           DIST-INT-1  DIST-INT-2  DIST-FP-1   DIST-FP-2   DIST-MIXED  DIST-CORR
    60% ┤
    50% ┤                                                   ░░░░  ████
    40% ┤  ████  ████
    30% ┤  ░░░░  ░░░░  ████
    20% ┤               ░░░░  ████  ░░░░
    10% ┤                     ░░░░               ░░░░
     0% ┤
         ────────────────────────────────────────────────────────────────

  ████ = GShare     ░░░░ = 2-bit Saturating Counter
```

### Discussion

- The **2SC predictor** performs well on simple, locally biased branches (FP traces: 14–18%) but fails on correlated patterns (CORR: 49%).
- The **GShare predictor** excels when branch outcomes are strongly correlated with recent global history (CORR: 11%), but can underperform 2SC on workloads where the XOR-indexing causes aliasing between unrelated branches (INT and MIXED traces).
- In general, for real program traces containing a mix of loop-back branches and correlated conditional sequences, GShare is expected to outperform 2SC by leveraging path information encoded in the GHR.

---

## Conclusion

Two dynamic branch predictors were successfully implemented and evaluated:

1. **2-bit Saturating Counter:** Simple, effective for locally biased branches. Memory budget: 32K bits. Average misprediction on synthetic traces: **29.1%**.
2. **GShare:** Uses global branch history to capture correlations. Memory budget: 32K + 14 bits. Average misprediction on synthetic traces: **32.1%** overall, but superior on correlated workloads (**11.4%** vs. 49.4%).

The results confirm the theoretical analysis: GShare's advantage manifests strongly when global history correlates with branch behavior, while 2SC is more robust in its simplicity. Real-world SPEC benchmarks typically favor GShare by 5–10 percentage points due to the prevalence of correlated branch patterns in real programs.

---

## References

[1] McFarling, S. *Combining Branch Predictors*. WRL Technical Note TN-36, 1993. Good description of local, correlating, gshare, and combining predictors.
