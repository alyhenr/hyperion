# Product Requirements Document (PRD): Project Hyperion
**Version:** 0.1.0
**Description:** Deterministic, Ultra-Low Latency Limit Order Book (LOB) and Matching Engine.

## 1. High-Level Perspective (Business & User)

### 1.1 Objective
Design and implement an ultra-low latency, deterministic order matching engine capable of processing millions of orders per second. The engine must provide strictly deterministic matching logic with nanosecond-scale latency, suitable for high-frequency algorithmic trading (HFT) and market-making operations.

### 1.2 Target Audience
* **Algorithmic Trading Systems:** Automated clients requiring deterministic, microsecond-to-nanosecond execution guarantees.
* **Market Makers:** Liquidity providers who rely on instantaneous queue-position updates and high-throughput quote revisions.

### 1.3 Core Capabilities
* **Order Management:** Support for parsing, validating, inserting, modifying, and canceling Limit and Market orders.
* **Execution Logic:** Strict Price-Time priority (FIFO). First order at a given price level is the first to be matched.
* **Market Data Publishing:** Real-time broadcast of L2/L3 order book states and trade execution tick data.
* **Resilience:** Graceful handling of malformed orders or network spikes without degrading the performance of the matching core.

### 1.4 Non-Functional Requirements
* **Determinism:** The tail latency (99.9th percentile) must remain virtually identical to the median latency. Jitter is strictly unacceptable.
* **Throughput:** Capable of sustaining >1,000,000 operations per second on a single thread.

---

## 2. Low-Level Perspective (Engineering & Technical)

### 2.1 Technology Stack
* **Language:** C++20/C++23 (leveraging templates, `constexpr`, and concepts for compile-time optimization).
* **Build System:** CMake.
* **Testing:** Google Test (GTest) for correctness, Google Benchmark for micro-benchmarking.
* **Profiling:** Linux `perf`, Valgrind (Callgrind/Cachegrind) for CPU cache miss analysis.

### 2.2 Architectural Constraints
* **Zero Runtime Allocation:** No `new`, `delete`, `malloc`, or `free` on the critical path. All memory for orders, price levels, and queues must be pre-allocated in custom Object Pools at startup.
* **Data Locality:** Data structures must be strictly aligned to 64-byte boundaries to prevent false sharing and optimize CPU cache line utilization (L1/L2/L3 cache hits).
* **Single-Threaded Core:** The matching engine itself will execute entirely on a single, isolated CPU core (core pinning). No mutexes, spinlocks, or system calls are permitted inside the matching loop.
* **Thread Boundaries:** Network I/O (ingress/egress) will run on separate threads. Communication with the matching core will occur exclusively via wait-free, Lock-Free Single-Producer Single-Consumer (SPSC) Ring Buffers.
* **Kernel Bypass (Architecture-Ready):** The network abstraction must be designed to eventually swap standard POSIX sockets for custom userspace networking or hardware-accelerated packet processing (e.g., DPDK, Solarflare).

### 2.3 Key Data Structures
1. **Memory Pool:** A contiguous block of pre-allocated memory yielding $O(1)$ time complexity for object acquisition and release.
2. **Limit Order Book (LOB):**
   * *Fast Path (Top of Book):* Arrays or specialized flat maps mapped to distinct price ticks.
   * *Queue:* Intrusive doubly-linked lists to manage orders at specific price levels without external node allocation.
3. **SPSC Ring Buffer:** A lock-free queue utilizing C++ `std::atomic` with strict `std::memory_order_acquire` and `std::memory_order_release` semantics.