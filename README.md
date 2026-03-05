# Hyperion
### High-Performance Low-Latency Matching Engine

Hyperion is a deterministic, ultra-low latency order matching engine project in modern C++. It aims for microsecond-level execution, focusing on minimizing jitter and maximizing throughput through mechanical sympathy and zero-allocation memory management.

---

## Design Philosophy

The architecture of Hyperion is built around the principle of minimizing CPU cycles and cache misses. Every component is designed to operate within a pre-allocated memory space to avoid the non-deterministic latency of the heap manager during the critical path of an order match.

### Core Architectural Pillars
* **Zero-Allocation Execution:** All data structures (Order Books, Price Levels) are pre-allocated at startup. The hot path contains zero calls to `malloc` or `new`.
* **Lock-Free Concurrency:** Utilizes atomic operations and single-writer ring buffers (Disruptor-style) to handle market data and order entry without mutex contention.
* **Cache Locality:** Data structures are cache-aligned and laid out contiguously in memory to maximize L1/L2 hit rates and prevent false sharing.
* **Object Pooling:** Efficient reuse of order objects to maintain stable memory pressure and predictable execution times.

---

## Technical Specifications

* **Language:** C++20 (utilizing concepts and constexpr for compile-time optimizations).
* **Data Structures:** Custom-built intrusive doubly-linked lists and static-sized hash maps for O(1) order lookup and removal.
* **Network Handling:** (Planned) Integration with kernel-bypass techniques (Solarflare OpenOnload / DPDK) for sub-microsecond packet-to-engine latency.
* **Time Precision:** High-resolution timestamping using TSC (Time Stamp Counter) for nanosecond-level profiling.

---

## Build & Performance Testing

### Build Requirements
* **Compiler:** GCC 11+ or Clang 13+
* **Build System:** CMake 3.20+
* **Optimization Flags:** `-O3 -march=native -fomit-frame-pointer`

### Compilation
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)