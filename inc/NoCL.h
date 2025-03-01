// CUDA-like library for compute kernels

#ifndef _NOCL_H_
#define _NOCL_H_

#include <Config.h>
#include <MemoryMap.h>
#include <Pebbles/Common.h>
#include <Pebbles/UART/IO.h>
#include <Pebbles/Instrs/Fence.h>
#include <Pebbles/Instrs/Atomics.h>
#include <Pebbles/Instrs/CacheMgmt.h>
#include <Pebbles/Instrs/SIMTDevice.h>
#include <Pebbles/CSRs/Sim.h>
#include <Pebbles/CSRs/Hart.h>
#include <Pebbles/CSRs/UART.h>
#include <Pebbles/CSRs/SIMTHost.h>
#include <Pebbles/CSRs/SIMTDevice.h>
#include <Pebbles/CSRs/CycleCount.h>

#if EnableCHERI
#include <cheriintrin.h>
#endif

// Arrays should be aligned to support coalescing unit
#define nocl_aligned __attribute__ ((aligned (SIMTLanes * 4)))

// Utility functions
// =================

// Return input where only first non-zero bit is set, starting from LSB
inline unsigned firstHot(unsigned x) {
  return x & (~x + 1);
}

// Is the given value a power of two?
inline bool isOneHot(unsigned x) {
  return x > 0 && (x & ~firstHot(x)) == 0;
}

// Compute logarithm (base 2) 
inline unsigned log2floor(unsigned x) {
  unsigned count = 0;
  while (x > 1) { x >>= 1; count++; }
  return count;
}

// Swap the values of two variables
template <typename T> INLINE void swap(T& a, T& b)
  { T tmp = a; a = b; b = tmp; }

// Data types
// ==========

// Dimensions
struct Dim3 {
  int x, y, z;
  Dim3() : x(1), y(1), z(1) {};
  Dim3(int xd) : x(xd), y(1), z(1) {};
  Dim3(int xd, int yd) : x(xd), y(yd), z(1) {};
  Dim3(int xd, int yd, int zd) : x(xd), y(yd), z(zd) {};
};

// 1D arrays
template <typename T> struct Array {
  T* base;
  int size;
  Array() {}
  Array(T* ptr, int n) : base(ptr), size(n) {}
  INLINE T& operator[](int index) const {
    return base[index];
  }
};

// 2D arrays
template <typename T> struct Array2D {
  T* base;
  int size0, size1;
  Array2D() {}
  Array2D(T* ptr, int n0, int n1) :
    base(ptr), size0(n0), size1(n1) {}
  INLINE const Array<T> operator[](int index) const {
    Array<T> a; a.base = &base[index * size1]; a.size = size1; return a;
  }
};

// 3D arrays
template <typename T> struct Array3D {
  T* base;
  int size0, size1, size2;
  Array3D() {}
  Array3D(T* ptr, int n0, int n1, int n2) :
    base(ptr), size0(n0), size1(n1), size2(n2) {}
  INLINE const Array2D<T> operator[](int index) const {
    Array2D<T> a; a.base = &base[index * size1 * size2];
    a.size0 = size1; a.size1 = size2; return a;
  }
};

// For shared local memory allocation
// Memory is allocated/released using a stack
// TODO: constraint bounds when CHERI enabled
struct SharedLocalMem {
  // This points to the top of the stack (which grows upwards)
  char* top;

  // Allocate memory on shared memory stack (static)
  template <int numBytes> void* alloc() {
    void* ptr = (void*) top;
    constexpr int bytes =
      (numBytes & 3) ? (numBytes & ~3) + 4 : numBytes;
    top += bytes;
    return ptr;
  }

  // Allocate memory on shared memory stack (dynamic)
  INLINE void* alloc(int numBytes) {
    void* ptr = (void*) top;
    int bytes = (numBytes & 3) ? (numBytes & ~3) + 4 : numBytes;
    top += bytes;
    return ptr;
  }



  // Typed allocation
  template <typename T> T* alloc(int n) {
    return (T*) alloc(n * sizeof(T));
  }

  // Allocate 1D array with static size
  template <typename T, int dim1> T* array() {
    return (T*) alloc<dim1 * sizeof(T)>();
  }

  // Allocate 2D array with static size
  template <typename T, int dim1, int dim2> auto array() {
    return (T (*)[dim2]) alloc<dim1 * dim2 * sizeof(T)>();
  }

  // Allocate 3D array with static size
  template <typename T, int dim1, int dim2, int dim3> auto array() {
    return (T (*)[dim2][dim3]) alloc<dim1 * dim2 * dim3 * sizeof(T)>();
  }

  // Allocate 1D array with dynamic size
  template <typename T> Array<T> array(int n) {
    Array<T> a; a.base = (T*) alloc(n * sizeof(T));
    a.size = n; return a;
  }

  
  // Allocate 2D array with dynamic size
  template <typename T> Array2D<T> array(int n0, int n1) {
    Array2D<T> a; a.base = (T*) alloc(n0 * n1 * sizeof(T));
    a.size0 = n0; a.size1 = n1; return a;
  }

  template <typename T> Array3D<T>
    array(int n0, int n1, int n2) {
      Array3D<T> a; a.base = (T*) alloc(n0 * n1 * n2 * sizeof(T));
      a.size0 = n0; a.size1 = n1; a.size2 = n2; return a;
    }
};

// Mapping between SIMT threads and CUDA thread/block indices
struct KernelMapping {
  // Use these to map SIMT thread id to thread X/Y coords within block
  unsigned threadXMask, threadYMask;
  unsigned threadXShift, threadYShift;

  // Number of blocks handled by all threads in X/Y dimensions
  unsigned numXBlocks, numYBlocks;

  // Use these to map SIMT thread id to block X/Y coords within grid
  unsigned blockXMask, blockYMask;
  unsigned blockXShift, blockYShift;

  // Amount of shared local memory per block
  unsigned localBytesPerBlock;
};

// Parameters that are available to any kernel
// All kernels inherit from this
struct Kernel {
  // Grid and block dimensions
  Dim3 gridDim, blockDim;

  // Block and thread indexes
  Dim3 blockIdx, threadIdx;

  // Shared local memory
  SharedLocalMem shared;

  // Mapping between SIMT threads and CUDA thread/block indices
  KernelMapping map;

  int entry_point;

  int cycle_count;

  //There are 3 priorities 0, 1 and 2, 0 being the highest
  int priority;

  int name;

};


//Queue that is used by the scheduler
class FixedQueue
{
    public:

        Kernel **arr;
        int head;
        int tail;
        int size;

        FixedQueue(Kernel **array, int size_): head(0), tail(0), arr(array), size(size_){}

        Kernel * pop(){
            int temp = head;
            head = (head + 1)%size;
            return arr[temp];
        }

        void enqueue(Kernel *k){
            arr[tail] = k;
            tail = (tail+1)%size;
        }
        
};


// Kernel invocation
// =================

// SIMT main function
template <typename K> __attribute__ ((noinline)) void _noclSIMTMain_() {
  pebblesSIMTPush();

  // Get pointer to kernel closure
  #if EnableCHERI
    void* almighty = cheri_ddc_get();
    K* kernelPtr = (K*) cheri_address_set(almighty,
                          pebblesKernelClosureAddr());
  #else
    K* kernelPtr = (K*) pebblesKernelClosureAddr();
  #endif
  K k = *kernelPtr;

  // Set thread index
  k.threadIdx.x = pebblesHartId() & k.map.threadXMask;
  k.threadIdx.y = (pebblesHartId() >> k.map.threadXShift) & k.map.threadYMask;
  k.threadIdx.z = 0;

  // Unique id for thread block within SM
  unsigned blockIdxWithinSM = pebblesHartId() >> k.map.blockXShift;

  // Initialise block indices
  unsigned blockXOffset = (pebblesHartId() >> k.map.blockXShift)
                            & k.map.blockXMask;
  unsigned blockYOffset = (pebblesHartId() >> k.map.blockYShift)
                            & k.map.blockYMask;
  k.blockIdx.x += blockXOffset;
  k.blockIdx.y += blockYOffset;
  
  // Invoke kernel with static priority
  pebblesSIMTConverge();
  for (int i = 0; i < k.priority+1; i++){
    if(k.blockIdx.y < k.gridDim.y ){
      if(k.blockIdx.x < k.gridDim.x){
        uint32_t localBase = LOCAL_MEM_BASE +
                 k.map.localBytesPerBlock * blockIdxWithinSM;
        #if EnableCHERI
        // TODO: constrain bounds
        void* almighty = cheri_ddc_get();
        k.shared.top = (char*) cheri_address_set(almighty, localBase);
        #else
          k.shared.top = (char*) localBase;
        #endif
        k.kernel();
        pebblesSIMTConverge();
        pebblesSIMTLocalBarrier();
        k.blockIdx.x += k.map.numXBlocks;
      }else{
        pebblesSIMTConverge();
        k.blockIdx.x = blockXOffset;
        k.blockIdx.y += k.map.numYBlocks;
        i--;
      }
    }
  }
    
  // Issue a fence to ensure all data has reached DRAM
  pebblesFence();

  // Terminate warp
  pebblesWarpTerminateSuccess();
}

/*while (k.blockIdx.y < k.gridDim.y) {
    while (k.blockIdx.x < k.gridDim.x) {
      uint32_t localBase = LOCAL_MEM_BASE +
                 k.map.localBytesPerBlock * blockIdxWithinSM;
      #if EnableCHERI
        // TODO: constrain bounds
        void* almighty = cheri_ddc_get();
        k.shared.top = (char*) cheri_address_set(almighty, localBase);
      #else
        k.shared.top = (char*) localBase;
      #endif
      k.kernel();
      pebblesSIMTConverge();
      pebblesSIMTLocalBarrier();
      k.blockIdx.x += k.map.numXBlocks;
    }
    pebblesSIMTConverge();
    k.blockIdx.x = blockXOffset;
    k.blockIdx.y += k.map.numYBlocks;
  }*/


// SIMT entry point
  template <typename K> __attribute__ ((noinline))
  void _noclSIMTEntry_() {
    // Stack top
    uint32_t top = 0;

    // Set stack pointer
    #if EnableCHERI
      uint32_t base = top - (1 << SIMTLogBytesPerStack);
      // Constrain bounds
      asm volatile("cspecialr csp, ddc\n"
                   "csetaddr csp, csp, %0\n"
                   "csetbounds csp, csp, %1\n"
                   "csetaddr csp, csp, %2\n"
                   : : "r"(base), "r"(1 << SIMTLogBytesPerStack), "r"(top-8));
    #else
      asm volatile("mv sp, %0\n" : : "r"(top-8));
    #endif
    // Invoke main function
    _noclSIMTMain_<K>();
  }


// Mapping function
template <typename K> __attribute__ ((noinline))
  void mapping_func(K* k) {
    unsigned threadsPerBlock = k->blockDim.x * k->blockDim.y;
    unsigned threadsUsed = threadsPerBlock * k->gridDim.x * k->gridDim.y;

    // Limitations for simplicity (TODO: relax)
    assert(k->blockDim.z == 1,
      "NoCL: blockDim.z != 1 (3D thread blocks not yet supported)");
    assert(k->gridDim.z == 1,
      "NoCL: gridDim.z != 1 (3D grids not yet supported)");
    assert(isOneHot(k->blockDim.x) && isOneHot(k->blockDim.y),
      "NoCL: blockDim.x or blockDim.y is not a power of two");
    assert(threadsPerBlock >= SIMTLanes,
      "NoCL: warp size does not divide evenly into block size");
    assert(threadsPerBlock <= SIMTWarps * SIMTLanes,
      "NoCL: block size is too large (exceeds SIMT thread count)");
    assert(threadsUsed >= SIMTWarps * SIMTLanes,
      "NoCL: unused SIMT threads (more SIMT threads than CUDA threads)");

    // Map hardware threads to CUDA thread&block indices
    // -------------------------------------------------

    // Block dimensions are all powers of two
    k->map.threadXMask = k->blockDim.x - 1;
    k->map.threadYMask = k->blockDim.y - 1;
    k->map.threadXShift = log2floor(k->blockDim.x);
    k->map.threadYShift = log2floor(k->blockDim.y);
    k->map.blockXShift = k->map.threadXShift + k->map.threadYShift;

    // Allocate blocks in grid X dimension
    unsigned logThreadsLeft = SIMTLogLanes + SIMTLogWarps - k->map.blockXShift;
    unsigned gridXLogBlocks = (1 << logThreadsLeft) <= k->gridDim.x
      ? logThreadsLeft : log2floor(k->gridDim.x);
    k->map.numXBlocks = 1 << gridXLogBlocks;
    k->map.blockXMask = k->map.numXBlocks - 1;
    logThreadsLeft -= gridXLogBlocks;

    // Allocate hardware threads in grid Y dimension
    k->map.blockYShift = k->map.blockXShift + gridXLogBlocks;
    unsigned gridYLogBlocks = (1 << logThreadsLeft) <= k->gridDim.y
      ? logThreadsLeft : log2floor(k->gridDim.y);
    k->map.numYBlocks = 1 << gridYLogBlocks;
    k->map.blockYMask = k->map.numYBlocks - 1;

    // Limitations for simplicity (TODO: relax)
    assert(k->gridDim.x % k->map.numXBlocks == 0,
      "gridDim.x is not a multiple of threads available in X dimension");
    assert(k->gridDim.y % k->map.numYBlocks == 0,
      "gridDim.y is not a multiple of threads available in Y dimension");

    // Set base of shared local memory (per block)
    unsigned blocksPerSM = (SIMTWarps * SIMTLanes) / threadsPerBlock;
    unsigned localBytes = 4 << (SIMTLogSRAMBanks + SIMTLogWordsPerSRAMBank);
    k->map.localBytesPerBlock = localBytes / blocksPerSM;

    // End of mapping
    // 

    k->entry_point = (uint32_t) _noclSIMTEntry_<K>;
  }

int go_func(Kernel *k) {

  unsigned threadsPerBlock = k->blockDim.x * k->blockDim.y;
  unsigned threadsUsed = threadsPerBlock * k->gridDim.x * k->gridDim.y;

  // Set number of warps per block
    // (for fine-grained barrier synchronisation)
    unsigned warpsPerBlock = threadsPerBlock >> SIMTLogLanes;
    while (!pebblesSIMTCanPut()) {}
    pebblesSIMTSetWarpsPerBlock(warpsPerBlock);

    // Set address of kernel closure
    #if EnableCHERI
      uint32_t kernelAddr = cheri_address_get(k);
    #else
      uint32_t kernelAddr = (uint32_t) k;
    #endif
    while (!pebblesSIMTCanPut()) {}
    pebblesSIMTSetKernel(kernelAddr);

    // Flush cache
    pebblesCacheFlushFull();

    // Start kernel on SIMT core
    #if EnableCHERI
      void (*entryFun)() = _noclSIMTEntry_<K>;
      uint32_t entryAddr = cheri_address_get(entryFun);
    #else
      uint32_t entryAddr = (uint32_t) k->entry_point;
    #endif
    while (!pebblesSIMTCanPut()) {}
    pebblesSIMTStartKernel(entryAddr);

    // Wait for kernel response
    while (!pebblesSIMTCanGet()) {}
    return pebblesSIMTGet();
}


// Trigger SIMT kernel execution from CPU

template <typename K> __attribute__ ((noinline))
  int noclRunKernel(K* k) {
    unsigned threadsPerBlock = k->blockDim.x * k->blockDim.y;
    unsigned threadsUsed = threadsPerBlock * k->gridDim.x * k->gridDim.y;

    // Limitations for simplicity (TODO: relax)
    assert(k->blockDim.z == 1,
      "NoCL: blockDim.z != 1 (3D thread blocks not yet supported)");
    assert(k->gridDim.z == 1,
      "NoCL: gridDim.z != 1 (3D grids not yet supported)");
    assert(isOneHot(k->blockDim.x) && isOneHot(k->blockDim.y),
      "NoCL: blockDim.x or blockDim.y is not a power of two");
    assert(threadsPerBlock >= SIMTLanes,
      "NoCL: warp size does not divide evenly into block size");
    assert(threadsPerBlock <= SIMTWarps * SIMTLanes,
      "NoCL: block size is too large (exceeds SIMT thread count)");
    assert(threadsUsed >= SIMTWarps * SIMTLanes,
      "NoCL: unused SIMT threads (more SIMT threads than CUDA threads)");

    // Map hardware threads to CUDA thread&block indices
    // -------------------------------------------------

    // Block dimensions are all powers of two
    k->map.threadXMask = k->blockDim.x - 1;
    k->map.threadYMask = k->blockDim.y - 1;
    k->map.threadXShift = log2floor(k->blockDim.x);
    k->map.threadYShift = log2floor(k->blockDim.y);
    k->map.blockXShift = k->map.threadXShift + k->map.threadYShift;

    // Allocate blocks in grid X dimension
    unsigned logThreadsLeft = SIMTLogLanes + SIMTLogWarps - k->map.blockXShift;
    unsigned gridXLogBlocks = (1 << logThreadsLeft) <= k->gridDim.x
      ? logThreadsLeft : log2floor(k->gridDim.x);
    k->map.numXBlocks = 1 << gridXLogBlocks;
    k->map.blockXMask = k->map.numXBlocks - 1;
    logThreadsLeft -= gridXLogBlocks;

    // Allocate hardware threads in grid Y dimension
    k->map.blockYShift = k->map.blockXShift + gridXLogBlocks;
    unsigned gridYLogBlocks = (1 << logThreadsLeft) <= k->gridDim.y
      ? logThreadsLeft : log2floor(k->gridDim.y);
    k->map.numYBlocks = 1 << gridYLogBlocks;
    k->map.blockYMask = k->map.numYBlocks - 1;

    // Limitations for simplicity (TODO: relax)
    assert(k->gridDim.x % k->map.numXBlocks == 0,
      "gridDim.x is not a multiple of threads available in X dimension");
    assert(k->gridDim.y % k->map.numYBlocks == 0,
      "gridDim.y is not a multiple of threads available in Y dimension");

    // Set base of shared local memory (per block)
    unsigned blocksPerSM = (SIMTWarps * SIMTLanes) / threadsPerBlock;
    unsigned localBytes = 4 << (SIMTLogSRAMBanks + SIMTLogWordsPerSRAMBank);
    k->map.localBytesPerBlock = localBytes / blocksPerSM;

    // End of mapping
    // --------------

    // Set number of warps per block
    // (for fine-grained barrier synchronisation)
    unsigned warpsPerBlock = threadsPerBlock >> SIMTLogLanes;
    while (!pebblesSIMTCanPut()) {}
    pebblesSIMTSetWarpsPerBlock(warpsPerBlock);

    // Set address of kernel closure
    #if EnableCHERI
      uint32_t kernelAddr = cheri_address_get(k);
    #else
      uint32_t kernelAddr = (uint32_t) k;
    #endif
    while (!pebblesSIMTCanPut()) {}
    pebblesSIMTSetKernel(kernelAddr);

    // Flush cache
    pebblesCacheFlushFull();

    // Start kernel on SIMT core
    #if EnableCHERI
      void (*entryFun)() = _noclSIMTEntry_<K>;
      uint32_t entryAddr = cheri_address_get(entryFun);
    #else
      uint32_t entryAddr = (uint32_t) _noclSIMTEntry_<K>;
    #endif
    while (!pebblesSIMTCanPut()) {}
    pebblesSIMTStartKernel(entryAddr);

    // Wait for kernel response
    while (!pebblesSIMTCanGet()) {}
    return pebblesSIMTGet();
  }

// Ask SIMT core for given performance stat
inline void printStat(const char* str, uint32_t statId)
{
  while (!pebblesSIMTCanPut()) {}
  pebblesSIMTAskStats(statId);
  while (!pebblesSIMTCanGet()) {}
  unsigned numCycles = pebblesSIMTGet();
  puts(str); puthex(numCycles); putchar('\n');
}

// Trigger SIMT kernel execution from CPU, and dump performance stats
template <typename K> __attribute__ ((noinline))
  int noclRunKernelAndDumpStats(K* k) {
    unsigned ret = noclRunKernel(k);

    // Check return code
    if (ret == 1) puts("Kernel failed\n");
    if (ret == 2) puts("Kernel failed due to exception\n");

    // Get number of cycles taken
    printStat("Cycles: ", STAT_SIMT_CYCLES);

    // Get number of instructions executed
    printStat("Instrs: ", STAT_SIMT_INSTRS);

    // Get number of pipeline bubbles due to suspended warp being scheduled
    printStat("Susps: ", STAT_SIMT_SUSP_BUBBLES);

    // Get number of pipeline retries
    printStat("Retries: ", STAT_SIMT_RETRIES);

    #if SIMTEnableRegFileScalarisation
      // Get max number of vector registers used
      printStat("MaxVecRegs: ", STAT_SIMT_MAX_VEC_REGS);
      #if SIMTEnableScalarUnit
        // Get number of instrs executed on scalar unit
        printStat("ScalarisedInstrs: ", STAT_SIMT_SCALARISABLE_INSTRS);
        // Get number of scalar pipeline suspension bubbles
        printStat("ScalarSusps: ", STAT_SIMT_SCALAR_SUSP_BUBBLES);
        // Get number of scalar pipeline abortions (mispredictions)
        printStat("ScalarAborts: ", STAT_SIMT_SCALAR_ABORTS);
      #else
        // Get potential scalarisable instructions
        printStat("ScalarisableInstrs: ", STAT_SIMT_SCALARISABLE_INSTRS);
      #endif
    #endif

    #if SIMTEnableCapRegFileScalarisation
      // Get max number of vector registers used
      printStat("MaxCapVecRegs: ", STAT_SIMT_MAX_CAP_VEC_REGS);
    #endif

    // Get number of DRAM accesses
    printStat("DRAMAccs: ", STAT_SIMT_DRAM_ACCESSES);

    return ret;
  }

// Explicit convergence
INLINE void noclPush() { pebblesSIMTPush(); }
INLINE void noclPop() { pebblesSIMTPop(); }
INLINE void noclConverge() { pebblesSIMTConverge(); }

// Barrier synchronisation
INLINE void __syncthreads() {
  pebblesSIMTConverge();
  pebblesSIMTLocalBarrier();
}



//Implements Scheduler with static priority
__attribute__ ((noinline)) int scheduler(Kernel **arr, int size){
    
    FixedQueue queue(arr , size);
    puts("Creating Queue");
    putchar('\n');
    for (int i = 0; i < size; i++)
    {
        queue.enqueue(arr[i]);
        arr[i]->blockIdx.x = 0;
        arr[i]->blockIdx.y = 0;
    }

    puts("Finshed creating Queue");
    putchar('\n');
    bool kernel_finished = false;
    
    while (true)
    {
        kernel_finished = false;
        Kernel *k = queue.pop();
        if (k->name == 0){
          puts("Running kerne VecAdd");
          putchar('\n');
        }else{
          puts("Running kerne Matrix");
          putchar('\n');
        }

        if (k->name == 0){
          puts("Running actual kernel VecAdd");
          putchar('\n');
          go_func(k);
        }else{
          puts("Running actual kerne Matrix");
          putchar('\n');
          go_func(k);
        }
       

        for (int i = 0; i < k->priority+1; i++){
          if(k->blockIdx.y < k->gridDim.y ){
            if(k->blockIdx.x < k->gridDim.x){
              k->blockIdx.x += k->map.numXBlocks;
            }else{
              k->blockIdx.x = 0;
              k->blockIdx.y += k->map.numYBlocks;
              i--;
            }
          }else{
            kernel_finished = true;
            k->cycle_count = pebblesCycleCount();
          }
        }
    
        if(!kernel_finished){
          queue.enqueue(k);
        }else if(kernel_finished && queue.head == queue.tail){
            break;  
        }
    }
    return 0;
}




/*
// TODO: Resolve issue with __heapBase symbol on CHERI toolchain.

// Minimal heap allocator
// ======================

// Use the address of this symbol to determine the base of the heap
// for dynamic memory allocation
extern unsigned __heapBase;

// Next available address on heap
uintptr_t __heapPointer = 0;

// Bare minimal allocator
// TODO: check for heap overflow
void* noclMalloc(unsigned numBytes) {
  // Initialise heap pointer on first use
  if (__heapPointer == 0) __heapPointer = (uintptr_t) &__heapBase;

  // All allocations are aligned on SIMTLanes*4 byte boundaries
  unsigned alignBits = SIMTLogLanes+2;
  unsigned alignMask = (1 << alignBits) - 1;

  // Realign heap pointer if necessary
  if ((__heapPointer & alignMask) != 0)
    __heapPointer = (__heapPointer & ~alignMask) + (1 << alignBits);

  // Create pointer to new allocation
  void* ptr;
  #if EnableCHERI
    // TODO: constrain bounds
    void* almighty = cheri_ddc_get();
    ptr = cheri_address_set(almighty, __heapPointer);
  #else
    ptr = (void*) __heapPointer;
  #endif

  // Perform allocation
  __heapPointer += numBytes;

  return ptr;
}
*/

#endif
