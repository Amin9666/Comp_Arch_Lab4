# Computer Architecture (CSE3031) - Final Exam Study Guide

## Table of Contents
1. [Performance Metrics](#performance-metrics)
2. [Instruction Set Architecture (ISA)](#instruction-set-architecture)
3. [Pipelining](#pipelining)
4. [Branch Prediction](#branch-prediction)
5. [Memory Hierarchy & Cache](#memory-hierarchy--cache)
6. [Virtual Memory](#virtual-memory)
7. [I/O Systems](#io-systems)
8. [Key Formulas](#key-formulas)
9. [Homework Problem Types](#homework-problem-types)

---

## Performance Metrics

### Key Concepts
- **Execution Time**: Time to complete a task
- **Throughput**: Tasks completed per unit time
- **CPU Time** = IC × CPI × Clock Cycle Time
  - IC = Instruction Count
  - CPI = Cycles Per Instruction
  - Clock Cycle Time = 1 / Clock Frequency

### Important Formulas
```
CPU Time = (Instruction Count × CPI) / Clock Rate

Speedup = Performance_new / Performance_old
        = Execution Time_old / Execution Time_new

MIPS = Instruction Count / (Execution Time × 10^6)
     = Clock Rate / (CPI × 10^6)

Amdahl's Law:
Speedup_overall = 1 / [(1 - P) + (P / S)]
  where P = fraction enhanced, S = speedup of enhanced portion
```

### Common Problem Types
- Calculate CPI given instruction mix and individual CPIs
- Compare performance of different processors
- Apply Amdahl's Law for parallelization speedup
- Calculate MIPS and compare systems

### Example Problem Structure
```
Given: 
- Processor A: 2 GHz, CPI = 1.5
- Processor B: 3 GHz, CPI = 2.0
- Same instruction count

Find: Which is faster and by how much?

Solution:
CPU_Time_A = IC × 1.5 / (2 × 10^9)
CPU_Time_B = IC × 2.0 / (3 × 10^9)
Speedup = CPU_Time_B / CPU_Time_A
```

---

## Instruction Set Architecture

### MIPS Instruction Formats
```
R-Type: | op(6) | rs(5) | rt(5) | rd(5) | shamt(5) | funct(6) |
I-Type: | op(6) | rs(5) | rt(5) | immediate(16)              |
J-Type: | op(6) | address(26)                                 |
```

### Register Types
- **$zero ($0)**: Always 0
- **$v0-$v1**: Return values
- **$a0-$a3**: Arguments
- **$t0-$t9**: Temporaries (caller-saved)
- **$s0-$s7**: Saved registers (callee-saved)
- **$sp**: Stack pointer
- **$ra**: Return address

### Common Instructions
```assembly
# Arithmetic
add  $t0, $t1, $t2    # $t0 = $t1 + $t2
addi $t0, $t1, 100    # $t0 = $t1 + 100
sub  $t0, $t1, $t2    # $t0 = $t1 - $t2

# Logical
and  $t0, $t1, $t2
or   $t0, $t1, $t2
sll  $t0, $t1, 4      # Shift left logical by 4

# Memory
lw   $t0, 0($t1)      # Load word: $t0 = Memory[$t1 + 0]
sw   $t0, 0($t1)      # Store word: Memory[$t1 + 0] = $t0

# Branch
beq  $t0, $t1, label  # Branch if equal
bne  $t0, $t1, label  # Branch if not equal
j    label            # Jump
jal  label            # Jump and link (function call)
jr   $ra              # Jump register (return)
```

### RISC vs CISC
| RISC | CISC |
|------|------|
| Simple instructions | Complex instructions |
| Fixed instruction length | Variable instruction length |
| Load/Store architecture | Memory-to-memory operations |
| Many registers | Fewer registers |
| Hardwired control | Microcode control |
| Example: MIPS, ARM | Example: x86 |

---

## Pipelining

### 5-Stage Pipeline
1. **IF** - Instruction Fetch
2. **ID** - Instruction Decode / Register Read
3. **EX** - Execute / Address Calculation
4. **MEM** - Memory Access
5. **WB** - Write Back

### Pipeline Hazards

#### 1. Structural Hazards
- **Cause**: Hardware resource conflict
- **Solution**: Duplicate hardware or stall

#### 2. Data Hazards
**Types:**
- **RAW (Read After Write)**: True dependency
  ```assembly
  add $t0, $t1, $t2    # Write $t0
  sub $t3, $t0, $t4    # Read $t0 (hazard!)
  ```
- **WAR (Write After Read)**: Anti-dependency (rare in simple pipelines)
- **WAW (Write After Write)**: Output dependency (rare in simple pipelines)

**Solutions:**
- **Forwarding (Bypassing)**: Send data directly from pipeline stage to stage
- **Stalling (Pipeline Bubble)**: Insert NOPs
- **Compiler scheduling**: Reorder independent instructions

#### 3. Control Hazards
- **Cause**: Branch instructions
- **Solutions**:
  - **Predict Not Taken**: Assume branch not taken, flush if wrong
  - **Predict Taken**: Assume branch taken
  - **Branch Delay Slot**: Execute next instruction regardless
  - **Branch Prediction**: Use history to predict (see next section)

### Pipeline Performance
```
Ideal CPI = 1 (one instruction per cycle)

Actual CPI = Ideal CPI + Stall cycles per instruction

Pipeline Speedup ≈ Number of stages (ideal case)
                 = Non-pipelined time / Pipelined time

Throughput = Instructions / Time
```

### Example Problem
```
Calculate CPI with data hazards:
- 30% loads (2 stall cycles if next instruction uses result)
- 20% of instructions use load result immediately

Stall cycles = 0.30 × 0.20 × 2 = 0.12
CPI = 1 + 0.12 = 1.12
```

---

## Branch Prediction

### Static Prediction
- **Always Not Taken**: Simple, works well for loops
- **Always Taken**: Good for backward branches
- **BTFN**: Backward Taken, Forward Not taken

### Dynamic Prediction

#### 1-Bit Predictor
- **State**: Taken (T) or Not Taken (NT)
- **Problem**: Mispredicts twice per loop iteration
```
State changes on every misprediction
```

#### 2-Bit Counter (Saturating Counter)
- **States**: Strongly NT (00), Weakly NT (01), Weakly T (10), Strongly T (11)
- **Advantage**: Requires two mispredictions to change strong prediction
- **Implementation**: Your Lab 4 has this! (predictor_2sc.C)

```
States:
00 (Strongly Not Taken) → Predict Not Taken
01 (Weakly Not Taken)   → Predict Not Taken
10 (Weakly Taken)       → Predict Taken
11 (Strongly Taken)     → Predict Taken

Increment on Taken, Decrement on Not Taken (saturate at 00 and 11)
```

#### Branch History Table (BHT)
- **Structure**: Array of 2-bit counters
- **Indexing**: Use lower bits of PC
```
Index = (PC >> 2) & (Table_Size - 1)
```

#### GShare Predictor
- **Combines**: PC and Global History Register (GHR)
- **Implementation**: Your Lab 4 has this! (predictor_gshare.C)
```
Index = (PC >> 2) XOR Global_History_Register
```
- **Advantage**: Captures correlation between branches

### Branch Prediction Metrics
```
Accuracy = Correct Predictions / Total Branches

Misprediction Rate = 1 - Accuracy

Branch Penalty = Misprediction Rate × Branch Penalty Cycles
```

---

## Memory Hierarchy & Cache

### Memory Hierarchy Levels
```
Registers (fastest, smallest)
    ↓
L1 Cache (separate I-cache and D-cache)
    ↓
L2 Cache (unified)
    ↓
L3 Cache (unified, shared)
    ↓
Main Memory (DRAM)
    ↓
Disk (slowest, largest)
```

### Cache Basics

#### Key Concepts
- **Hit**: Data found in cache
- **Miss**: Data not in cache
- **Hit Rate**: Hits / Total Accesses
- **Miss Rate**: 1 - Hit Rate
- **Hit Time**: Time to access cache
- **Miss Penalty**: Time to fetch from lower level

#### Address Breakdown
```
| Tag | Index | Block Offset |
```
- **Block Offset**: Selects byte within block (log₂(Block_Size) bits)
- **Index**: Selects cache set (log₂(Number_of_Sets) bits)
- **Tag**: Remaining bits for comparison

### Cache Mapping Types

#### 1. Direct Mapped
- **Formula**: Block Address mod Number of Blocks
- **Pros**: Simple, fast
- **Cons**: High conflict misses
```
Index = (Block Address) % Number_of_Cache_Blocks
Set contains: 1 block
```

#### 2. Fully Associative
- **Any block can go anywhere**
- **Pros**: No conflict misses
- **Cons**: Expensive, slow comparison
```
Number of comparators = Number of blocks
```

#### 3. N-Way Set Associative
- **Compromise between direct and fully associative**
```
Number of Sets = Total Cache Blocks / N
Index = (Block Address) % Number_of_Sets
Each set contains: N blocks
```

### Replacement Policies
- **LRU** (Least Recently Used): Replace block not used for longest time
- **FIFO** (First In First Out): Replace oldest block
- **Random**: Random replacement
- **LFU** (Least Frequently Used): Replace least frequently accessed

### Write Policies

#### Write Hit
- **Write Through**: Write to cache AND memory
  - Pro: Memory always consistent
  - Con: Slow (every write goes to memory)
- **Write Back**: Write only to cache, mark dirty
  - Pro: Fast, reduces memory traffic
  - Con: Complex, need dirty bit

#### Write Miss
- **Write Allocate**: Load block into cache, then write
  - Usually paired with Write Back
- **No Write Allocate**: Write directly to memory
  - Usually paired with Write Through

### Cache Performance
```
AMAT (Average Memory Access Time) = Hit Time + Miss Rate × Miss Penalty

CPU Time = (CPU execution cycles + Memory stall cycles) × Cycle Time

Memory Stall Cycles = IC × Miss Rate × Miss Penalty

Miss Rate Types:
- Compulsory (Cold): First access
- Capacity: Cache too small
- Conflict: Multiple blocks map to same location
- Coherence: Multi-processor consistency (advanced)
```

### Example Cache Problem
```
Given:
- 32 KB cache, 64-byte blocks, 4-way set associative
- 32-bit addresses

Calculate:
1. Number of blocks = 32KB / 64B = 512 blocks
2. Number of sets = 512 / 4 = 128 sets
3. Index bits = log₂(128) = 7 bits
4. Block offset bits = log₂(64) = 6 bits
5. Tag bits = 32 - 7 - 6 = 19 bits

Address breakdown: | 19-bit Tag | 7-bit Index | 6-bit Offset |
```

---

## Virtual Memory

### Key Concepts
- **Virtual Address**: Address used by program
- **Physical Address**: Actual memory location
- **Page**: Fixed-size memory block (typically 4 KB)
- **Page Table**: Maps virtual pages to physical frames
- **TLB (Translation Lookaside Buffer)**: Cache for page table entries

### Address Translation
```
Virtual Address: | Virtual Page Number | Page Offset |
Physical Address: | Physical Frame Number | Page Offset |

Page Offset stays the same!
```

### Page Table Entry (PTE)
```
| Valid | Dirty | Reference | Protection | Physical Frame Number |
```
- **Valid bit**: Page in physical memory?
- **Dirty bit**: Page modified?
- **Reference bit**: Recently accessed? (for replacement)
- **Protection bits**: Read/Write/Execute permissions

### Page Faults
1. **Process**: Virtual page not in physical memory
2. **Handler**: OS loads page from disk
3. **Penalty**: Very expensive (~millions of cycles)

### TLB (Translation Lookaside Buffer)
- **Fully associative** cache of page table entries
- **Fast** address translation
```
TLB Hit: Virtual → Physical (1 cycle)
TLB Miss: Access page table, then translate
```

### Performance Calculation
```
Effective Access Time (EAT) = TLB Hit Rate × (TLB Time + Memory Time)
                             + TLB Miss Rate × (TLB Time + Page Table Time + Memory Time)

With page faults:
EAT = (1 - Page Fault Rate) × Memory Access Time
    + Page Fault Rate × Page Fault Service Time
```

### Page Replacement Algorithms
- **FIFO**: Replace oldest page
- **LRU**: Replace least recently used
- **Optimal**: Replace page not used for longest time (theoretical)
- **Clock (Second Chance)**: FIFO with reference bit

### Multi-level Page Tables
```
Purpose: Reduce page table size for sparse address spaces

2-Level Example:
Virtual Address: | Level 1 Index | Level 2 Index | Offset |

Process:
1. Use L1 Index to find L2 page table
2. Use L2 Index to find physical frame
3. Add offset for final address
```

---

## I/O Systems

### I/O Methods

#### 1. Programmed I/O (Polling)
- **CPU continuously checks** device status
- **Wastes** CPU cycles
```c
while (device_status != READY);
read_data();
```

#### 2. Interrupt-Driven I/O
- **Device signals CPU** when ready
- **CPU can do other work** while waiting
- **Steps**:
  1. CPU initiates I/O
  2. CPU continues other work
  3. Device interrupts when ready
  4. CPU handles interrupt (context switch)

#### 3. Direct Memory Access (DMA)
- **DMA controller** transfers data directly to/from memory
- **CPU only involved** at start and end
- **Best for large transfers**
```
Process:
1. CPU tells DMA: source, destination, size
2. DMA performs transfer
3. DMA interrupts CPU when done
```

### I/O Performance
```
I/O Time = Seek Time + Rotational Latency + Transfer Time

Throughput = Data Size / Total Time

With DMA:
CPU Time saved = Transfer Time - DMA Setup Time
```

### Bus Types
- **Processor-Memory Bus**: Short, high speed, custom
- **I/O Bus**: Longer, industry standard (PCIe, USB)
- **Backplane Bus**: System bus

### Disk Performance
```
Access Time = Seek Time + Rotational Latency + Transfer Time

Seek Time: Move head to correct track
Rotational Latency: Wait for sector to rotate under head
  Average = (1/2) × Rotation Time
Transfer Time: Read/Write data
  = Data Size / Transfer Rate
```

---

## Key Formulas - Quick Reference

### Performance
```
CPU Time = IC × CPI × Clock Cycle Time
Speedup = Time_old / Time_new
MIPS = Clock Rate / (CPI × 10^6)
Amdahl's Law: Speedup = 1 / [(1-P) + P/S]
```

### Pipeline
```
Pipeline CPI = Ideal CPI + Pipeline Stall Cycles
Speedup = Pipeline Depth (ideal)
Throughput = 1 / CPI (instructions per cycle)
```

### Cache
```
AMAT = Hit Time + Miss Rate × Miss Penalty
Total Bits = (Block Size + Tag + Valid + Dirty) × Number of Blocks

Number of Index Bits = log₂(Number of Sets)
Number of Offset Bits = log₂(Block Size)
Number of Tag Bits = Address Bits - Index Bits - Offset Bits
```

### Virtual Memory
```
Page Size = 2^(Offset Bits)
Number of Pages = Virtual Address Space / Page Size
EAT = (1 - Miss Rate) × Hit Time + Miss Rate × Miss Penalty
```

### Branch Prediction
```
Accuracy = Correct Predictions / Total Branches
Penalty = Miss Rate × Penalty Cycles
```

---

## Homework Problem Types

### Type 1: Performance Comparison
**Pattern**: Given two processors with different specs, which is faster?
- **Key**: Calculate CPU Time for both
- **Watch for**: Same instruction count, different CPIs and clock rates
- **Formula**: CPU Time = IC × CPI / Clock Rate

### Type 2: Amdahl's Law
**Pattern**: What speedup can we achieve by improving X?
- **Key**: Identify fraction improved (P) and speedup of that fraction (S)
- **Watch for**: Overall speedup always limited by unimproved portion
- **Formula**: Speedup = 1 / [(1-P) + P/S]

### Type 3: Pipeline Hazards
**Pattern**: Given instruction sequence, identify hazards and stalls
- **Key**: Track register dependencies through pipeline stages
- **Watch for**: RAW hazards, forwarding paths, load-use hazards
- **Solution**: Draw pipeline diagram or calculate stall cycles

### Type 4: Cache Design
**Pattern**: Given cache specs, calculate addressing bits
- **Key**: Break address into Tag | Index | Offset
- **Watch for**: Set associativity affects number of sets
- **Steps**:
  1. Calculate number of blocks
  2. Calculate number of sets
  3. Calculate bits for each field

### Type 5: Cache Performance
**Pattern**: Calculate AMAT or compare cache designs
- **Key**: AMAT = Hit Time + Miss Rate × Miss Penalty
- **Watch for**: Multi-level caches (use average access time)
- **Compare**: Lower AMAT is better

### Type 6: Virtual Memory Translation
**Pattern**: Translate virtual address to physical address
- **Key**: Use page table to map VPN to PFN, keep offset same
- **Watch for**: Page faults (valid bit = 0)
- **TLB**: Check TLB first for fast translation

### Type 7: Branch Prediction
**Pattern**: Given branch history, predict outcomes
- **Key**: Follow state machine for 2-bit predictor
- **Watch for**: Initial state, GHR updates for GShare
- **Accuracy**: Count correct predictions / total branches

### Type 8: Instruction Encoding
**Pattern**: Encode/decode MIPS instructions
- **Key**: Know R/I/J type formats
- **Watch for**: Opcodes, register numbers, immediate values
- **Convert**: Assembly ↔ Binary ↔ Hexadecimal

---

## Study Tips

### High-Yield Topics (Most Likely on Exam)
1. ✅ **Performance calculations** (CPI, MIPS, Speedup)
2. ✅ **Pipeline hazards** and forwarding
3. ✅ **Cache addressing** and performance
4. ✅ **Branch prediction** (especially 2-bit counter)
5. ✅ **Virtual memory** translation

### Problem-Solving Strategy
1. **Read carefully**: Identify what's given and what's asked
2. **Draw diagrams**: Pipeline stages, cache structure, memory layout
3. **Show work**: Partial credit for correct approach
4. **Check units**: Seconds vs cycles, KB vs KiB
5. **Verify**: Does answer make sense?

### Common Mistakes to Avoid
- ❌ Confusing CPI with IPC (IPC = 1/CPI)
- ❌ Mixing up cache hit time vs miss penalty
- ❌ Forgetting to convert between seconds and cycles
- ❌ Wrong units (GHz → Hz requires ×10^9)
- ❌ Confusing virtual and physical addresses
- ❌ Forgetting page offset stays the same in translation

### Last-Minute Review Checklist
- [ ] Can I calculate CPI given instruction mix?
- [ ] Can I identify and resolve pipeline hazards?
- [ ] Can I break down cache address into Tag|Index|Offset?
- [ ] Can I calculate AMAT for cache?
- [ ] Can I translate virtual to physical address?
- [ ] Do I know 2-bit predictor state machine?
- [ ] Can I calculate speedup with Amdahl's Law?
- [ ] Do I remember MIPS instruction formats?

---

## Notes Section

### From Homework 1
*[Fill in key problems and concepts]*

### From Homework 2
*[Fill in key problems and concepts]*

### From Homework 3
*[Fill in key problems and concepts]*

### From Homework 4
*[Fill in key problems and concepts]*

### From Labs
*[Fill in key implementations and learnings]*
- Lab 4: Implemented 2-bit saturating counter and GShare branch predictors

### From Midterm Review
*[Fill in key topics covered]*

---

## Quick Reference Tables

### MIPS Instruction Opcodes (Common)
| Instruction | Opcode | Type | Function |
|-------------|--------|------|----------|
| add         | 0      | R    | 32       |
| sub         | 0      | R    | 34       |
| and         | 0      | R    | 36       |
| or          | 0      | R    | 37       |
| slt         | 0      | R    | 42       |
| lw          | 35     | I    | -        |
| sw          | 43     | I    | -        |
| beq         | 4      | I    | -        |
| bne         | 5      | I    | -        |
| addi        | 8      | I    | -        |
| j           | 2      | J    | -        |
| jal         | 3      | J    | -        |

### Powers of 2 (Quick Reference)
```
2^10 = 1,024 ≈ 1K
2^20 = 1,048,576 ≈ 1M
2^30 = 1,073,741,824 ≈ 1G
```

### Typical Latencies
```
L1 Cache: ~1-4 cycles
L2 Cache: ~10-20 cycles
Main Memory: ~100-200 cycles
Disk: ~10,000,000 cycles
```

---

**Good luck on your final exam! 🎓**

*Remember: The professor uses homework questions, so review those problems carefully!*
