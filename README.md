# CSC 422: Introduction to Parallel and Distributed Programming

This repository contains my coursework for CSC 422, showcasing hands-on experience with parallel and distributed computing paradigms. The projects demonstrate practical implementations of threading, synchronization, and distributed systems concepts.

## 📚 Course Overview

This course explores fundamental concepts in parallel and distributed programming, including:
- **Thread-based parallelism** using POSIX threads (pthreads)
- **Synchronization mechanisms** and concurrent data structures
- **Distributed systems** programming with Message Passing Interface (MPI)
- **Performance analysis** and optimization techniques

---

## 🚀 Projects

### Program 1: Parallel Column Sort Algorithm

**Location:** `CSC422/Prog1/`

**Objective:** Implement and compare sequential vs. multi-threaded column sort algorithms.

**Key Concepts:**
- **Threading with pthreads**: Spawning and managing multiple worker threads
- **Parallel sorting**: Distributing column sorting work across threads
- **Performance measurement**: Comparing execution times between sequential and parallel implementations
- **Work distribution**: Partitioning workload among threads efficiently

**Implementation:**
- `seqColumnSort.c` - Sequential baseline implementation
- `threadColumnSort.c` - Parallel implementation using pthreads

**What I Learned:**
- How to decompose a sorting algorithm for parallel execution
- The overhead vs. speedup tradeoffs in parallel programming
- Thread creation, synchronization, and joining patterns
- Performance analysis techniques for parallel algorithms

---

### Program 2: Concurrent Memory Allocator

**Location:** `CSC422/Prog2/`

**Objective:** Develop a thread-safe memory allocator supporting different concurrency models.

**Key Concepts:**
- **Thread-safe data structures**: Implementing concurrent free lists
- **Synchronization primitives**: Mutexes and thread-local storage
- **Coarse-grained vs. fine-grained locking**: Different approaches to thread safety
- **Memory management**: Custom allocation and deallocation strategies

**Implementation:**
- `myMalloc.c` - Core allocator with three modes:
  - Sequential (mode 0)
  - Coarse-grained locking (mode 1)
  - Fine-grained locking with thread-local storage (mode 2)
- `myMalloc-helper.c` - Supporting functions and chunk management
- `Makefile` - Build configuration

**What I Learned:**
- Different strategies for making concurrent data structures thread-safe
- Tradeoffs between lock granularity and performance
- Thread-local storage for reducing contention
- Managing shared resources in multi-threaded environments
- The complexity of building correct concurrent systems

---

### Program 3: Distributed Hash Table (DHT)

**Location:** `CSC422/Prog3/`

**Objective:** Implement a distributed hash table using MPI for inter-process communication.

**Key Concepts:**
- **Message Passing Interface (MPI)**: Distributed computing communication model
- **Distributed data structures**: Hash table distributed across multiple processes
- **Asynchronous communication**: Non-blocking sends and receives
- **Process coordination**: Managing multiple independent processes

**Implementation:**
- `dht.py` - Main DHT implementation with storage nodes
- `command.py` - Command node for testing DHT operations
- `dht_globals.py` - Shared constants and message tags

**Operations Supported:**
- `ADD` - Add new storage nodes to the DHT
- `PUT` - Store key-value pairs
- `GET` - Retrieve values by key
- `REMOVE` - Remove nodes from the DHT

**What I Learned:**
- How to structure distributed applications using MPI
- Message-based communication patterns in distributed systems
- Coordinating multiple processes to maintain a coherent data structure
- The challenges of distributed state management
- Python's mpi4py library for parallel computing

---

## 💻 Technologies Used

- **C Programming**: Low-level systems programming with manual memory management
- **POSIX Threads (pthreads)**: Multi-threading library for shared-memory parallelism
- **Python**: High-level language for distributed systems implementation
- **MPI (mpi4py)**: Standard for parallel computing across distributed processes
- **GCC/Make**: Compilation and build tools

---

## 🎯 Key Takeaways

Through these projects, I gained practical experience in:

1. **Parallel Algorithm Design**: Understanding how to decompose problems for parallel execution
2. **Concurrency Control**: Implementing thread-safe code with proper synchronization
3. **Performance Optimization**: Balancing parallelism overhead with computational speedup
4. **Distributed Systems**: Building systems that span multiple processes/machines
5. **Debugging Parallel Code**: Identifying and resolving race conditions and deadlocks
6. **Scalability Analysis**: Understanding how performance scales with thread/process count

---

## 📊 Skills Demonstrated

- ✅ Multi-threaded programming with pthreads
- ✅ Mutex-based synchronization and critical sections
- ✅ Thread-local storage and lock-free techniques
- ✅ Message passing with MPI
- ✅ Performance measurement and analysis
- ✅ Concurrent data structure design
- ✅ Distributed system coordination
- ✅ C and Python systems programming

---

## 📝 Note

This repository represents academic coursework completed for CSC 422. The focus is on demonstrating understanding of parallel and distributed programming concepts rather than production-ready implementations. Each project includes PDF specifications outlining the detailed requirements and learning objectives.

---

## 🔗 Course Information

**Course:** CSC 422 - Introduction to Parallel and Distributed Programming  
**Institution:** [Your Institution]  
**Semester:** Spring 2025

---

*This repository showcases my journey in learning parallel and distributed programming, from basic threading concepts to complex distributed systems.*
