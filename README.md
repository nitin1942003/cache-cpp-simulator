# Cache Simulator

A C++ cache simulator that models an 8-way set-associative cache with 64 sets. This project simulates realistic memory access patterns, tracking cache hits and misses in real-time.

## Features
- **8-Way Set-Associative Cache**: 64 sets with 8 lines per set.
- **Read/Write Operations**: Simulates random read and write requests, tracking hits and misses.
- **Random Address Generation**: Generates 40-bit addresses using a normal distribution with configurable mean and standard deviation.
- **Real-Time Statistics**: Displays access counts and hit rate every second during the simulation.

## How It Works
The simulator generates random 40-bit memory addresses and either reads from or writes to the cache, updating hit/miss counters accordingly. Addresses are generated with a specified mean and standard deviation, simulating realistic access patterns.

## Usage
1. **Compile the Code**:
   ```bash
   g++ -o cache_i cache_i.cpp
