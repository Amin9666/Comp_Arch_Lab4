# CSE3031 Final Exam Cheat Sheet: Comprehensive Q&A

**Source basis:** HW1–HW4 + Midterm Review topics reflected in repository study material.  
**Focus:** Memory hierarchy, virtual memory, advanced memory, pipelining, ILP, fundamentals, ISA.

---

## A) Fundamentals & Performance

### 1) What is the CPU performance equation?
**Answer:**  
\[
\text{CPU Time} = \text{IC} \times \text{CPI} \times \text{Cycle Time}
= \frac{\text{IC} \times \text{CPI}}{\text{Clock Rate}}
\]

### 2) How do you compare two processors when instruction count is the same?
**Answer:**  
Compute CPU time for each using \( \text{IC} \times \text{CPI}/\text{Clock Rate} \). Smaller CPU time is faster.

### 3) What is speedup?
**Answer:**  
\[
\text{Speedup} = \frac{\text{Time}_{\text{old}}}{\text{Time}_{\text{new}}}
\]
Speedup > 1 means improvement.

### 4) What is Amdahl’s Law and what does it mean?
**Answer:**  
\[
\text{Speedup}_{\text{overall}} = \frac{1}{(1-P)+P/S}
\]
\(P\): enhanced fraction, \(S\): speedup of enhanced part. Unimproved part limits total speedup.

### 5) What is MIPS and why can it be misleading?
**Answer:**  
\[
\text{MIPS} = \frac{\text{Instruction Count}}{\text{Execution Time} \times 10^6}
= \frac{\text{Clock Rate}}{\text{CPI} \times 10^6}
\]
It ignores instruction complexity and can mislead across ISAs/programs.

---

## B) ISA & Encoding

### 6) What are MIPS R/I/J instruction formats?
**Answer:**  
- R-type: `op rs rt rd shamt funct`  
- I-type: `op rs rt immediate`  
- J-type: `op address`

### 7) What is the role of load/store in MIPS?
**Answer:**  
Arithmetic/logical ops use registers only; memory is accessed via `lw`/`sw`.

### 8) How do you verify fixed-length encoding feasibility for mixed instruction types?
**Answer:**  
Account for required field bits per format, reserve opcode space, and verify total encoding space is enough.

---

## C) Pipelining & ILP

### 9) What are the 5 classic pipeline stages?
**Answer:**  
IF, ID, EX, MEM, WB.

### 10) What is ideal pipeline CPI and ideal speedup?
**Answer:**  
Ideal CPI ≈ 1. Ideal speedup ≈ number of stages (for long, balanced pipelines).

### 11) What is actual CPI in pipelined execution?
**Answer:**  
\[
\text{CPI}_{\text{actual}} = \text{CPI}_{\text{ideal}} + \text{stall cycles per instruction}
\]

### 12) What are structural hazards?
**Answer:**  
Two stages need same hardware resource simultaneously; fix by stalling or duplicating resources.

### 13) What are data hazards (RAW, WAR, WAW)?
**Answer:**  
- RAW: read before needed write completes (true dependency)  
- WAR/WAW: typically not hazards in simple in-order 5-stage pipelines

### 14) When does forwarding solve hazards?
**Answer:**  
Forwarding resolves many RAW hazards by bypassing values from later stages to earlier consumers.

### 15) Why does load-use often still stall?
**Answer:**  
Loaded data is usually available too late for the immediate next instruction even with forwarding.

### 16) What are control hazards?
**Answer:**  
Hazards from branches/jumps; mitigated by prediction, flush, delay slots, or earlier resolution.

### 17) How does a 1-bit branch predictor behave on loops?
**Answer:**  
Often mispredicts at loop exit and re-entry (typically two mistakes per loop run pattern).

### 18) How does a 2-bit saturating counter predictor work?
**Answer:**  
States: 00 Strongly Not Taken (SNT), 01 Weakly Not Taken (WNT), 10 Weakly Taken (WT), 11 Strongly Taken (ST).  
Predict taken in 10/11; not-taken in 00/01.  
Increment on taken, decrement on not-taken, saturating at ends.

### 19) What is a BHT (Branch History Table)?
**Answer:**  
Array of predictor states indexed by PC bits (commonly lower bits after alignment shift).

### 20) What is GShare indexing?
**Answer:**  
\[
\text{Index} = (PC>>2) \oplus GHR
\]
It mixes branch address and global history to capture correlation.

### 21) How do you compute predictor accuracy and penalty?
**Answer:**  
\[
\text{Accuracy} = \frac{\text{Correct}}{\text{Total}},\quad
\text{Mispredict Rate} = 1-\text{Accuracy}
\]
Penalty contribution = mispredict rate × branch penalty cycles.

---

## D) Memory Hierarchy & Cache

### 22) What is AMAT?
**Answer:**  
\[
\text{AMAT} = \text{Hit Time} + \text{Miss Rate} \times \text{Miss Penalty}
\]

### 23) How do you compute Tag/Index/Offset bits?
**Answer:**  
- Offset = log2(block size)  
- Sets = blocks / associativity  
- Index = log2(sets)  
- Tag = address bits − index − offset

### 24) Direct-mapped vs fully associative vs set-associative?
**Answer:**  
- Direct-mapped: 1 location per block (fast, more conflicts)  
- Fully associative: anywhere (few conflicts, expensive lookup)  
- N-way set-assoc: compromise

### 25) What are cache hit, miss, hit rate, miss rate?
**Answer:**  
Hit: found in cache. Miss: not found.  
Hit rate = hits/total, miss rate = 1 − hit rate.

### 26) Why can larger block size both help and hurt?
**Answer:**  
Helps spatial locality, but may increase miss penalty and reduce number of cache blocks.

### 27) LRU vs FIFO cache replacement?
**Answer:**  
LRU evicts least recently used block; FIFO evicts oldest inserted block.

### 28) Write-through vs write-back?
**Answer:**  
Write-through updates lower memory on each write; write-back defers until eviction (dirty bit needed).

### 29) Write-allocate vs no-write-allocate?
**Answer:**  
Write-allocate loads block into cache on write miss; no-write-allocate writes directly to lower memory.

### 30) Why is L1 often split and lower levels unified?
**Answer:**  
Split L1 supports simultaneous I-fetch and D-access; unified lower levels share capacity dynamically.

### 31) What are compulsory, conflict, and capacity misses?
**Answer:**  
Compulsory: first touch. Conflict: mapping collisions. Capacity: cache too small for working set.

---

## E) Virtual Memory

### 32) How is virtual address translated to physical address?
**Answer:**  
VPN selects mapping (TLB/page table), PFN is returned, and page offset is copied unchanged.

### 33) What does the TLB do?
**Answer:**  
Caches recent page-table translations to speed up address translation.

### 34) Does TLB miss imply page fault?
**Answer:**  
No. TLB miss may still find a valid mapping in page table; page fault only if page is not resident.

### 35) What is in a page table entry (PTE)?
**Answer:**  
Typically valid, dirty, reference/access, protection bits, and physical frame number.

### 36) What is a page fault and why expensive?
**Answer:**  
Referenced page not in memory; OS must fetch from disk, causing very large latency.

### 37) What is Effective Access Time with TLB?
**Answer:**  
\[
EAT = h(TLB + Mem) + (1-h) \times (TLB + PT + Mem)
\]
where \(h\) is TLB hit rate.
Here, TLB = TLB access time, Mem = memory access time, PT = page-table access time.

### 38) FIFO vs LRU page replacement?
**Answer:**  
FIFO evicts oldest resident page; LRU evicts least recently used page.

### 39) Why use multilevel page tables?
**Answer:**  
To reduce memory overhead for sparse virtual address spaces.

---

## F) High-Probability “Do-by-Hand” Question Types

### 40) Given cache parameters, what are index/tag/offset and number of sets?
**Answer:**  
Use: blocks = cache/block, sets = blocks/ways, then log2 for bit fields.

### 41) Given hit rate/hit time/miss penalty, what is AMAT?
**Answer:**  
Plug directly into \( \text{AMAT}=\text{Hit Time}+\text{Miss Rate}\times\text{Miss Penalty} \).

### 42) Given instruction sequence, where are stalls/forwarding needed?
**Answer:**  
Track RAW dependencies by stage timing; load-use usually inserts bubble(s).

### 43) Given branch outcomes, what are 2-bit predictor states and accuracy?
**Answer:**  
Simulate state transitions per branch and count correct predictions.

### 44) Given virtual address + page table (and maybe TLB), find physical address or fault?
**Answer:**  
Split VPN/offset, check TLB then PTE validity, map to PFN, keep offset unchanged.

### 45) Given enhancement fraction and local speedup, compute total speedup?
**Answer:**  
Use Amdahl’s Law.

---

## Ultra-Compact Formula Box
- CPU Time = IC × CPI × Cycle Time = (IC × CPI)/Clock Rate  
- Speedup = Told / Tnew  
- Amdahl: \(1 / ((1-P)+P/S)\)  
- Pipeline CPI = 1 + stall/instruction (ideal baseline)  
- AMAT = Hit Time + Miss Rate × Miss Penalty  
- Offset bits = log2(Block Size)  
- Index bits = log2(Number of Sets)  
- Tag bits = Address bits − Index − Offset  
- EAT (TLB) = \(h(TLB+Mem)+(1-h)(TLB+PT+Mem)\)
