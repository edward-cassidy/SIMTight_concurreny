#include <NoCL.h>
#include <Rand.h>
#include <Pebbles/CSRs/CycleCount.h>

int seed_type = 235;
bool isSim = false;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Size of arrays being sorted
#define LOCAL_SIZE_LIMIT 1024

// Sort two key/value pairs
inline void twoSort(unsigned *keyA, unsigned* valA,
                    unsigned *keyB, unsigned* valB, unsigned dir)
{
  if ((*keyA > *keyB) == dir) {
    unsigned t;
    t = *keyA; *keyA = *keyB; *keyB = t;
    t = *valA; *valA = *valB; *valB = t;
  }
  noclConverge();
}

// Monolithic bitonic sort kernel for short arrays fitting into local mem
struct BitonicSortLocal : Kernel {
  unsigned *d_DstKey_arg;
  unsigned *d_DstVal_arg;
  unsigned *d_SrcKey_arg;
  unsigned *d_SrcVal_arg;
  unsigned arrayLength;
  unsigned sortDir;

  void kernel() {
    unsigned* l_key = shared.array<unsigned, LOCAL_SIZE_LIMIT>();
    unsigned* l_val = shared.array<unsigned, LOCAL_SIZE_LIMIT>();

    // Offset to the beginning of subbatch and load data
    unsigned* d_SrcKey =
      d_SrcKey_arg + blockIdx.x * LOCAL_SIZE_LIMIT + threadIdx.x;
    unsigned* d_SrcVal =
      d_SrcVal_arg + blockIdx.x * LOCAL_SIZE_LIMIT + threadIdx.x;
    unsigned* d_DstKey =
      d_DstKey_arg + blockIdx.x * LOCAL_SIZE_LIMIT + threadIdx.x;
    unsigned* d_DstVal =
      d_DstVal_arg + blockIdx.x * LOCAL_SIZE_LIMIT + threadIdx.x;
    l_key[threadIdx.x + 0] = d_SrcKey[0];
    l_val[threadIdx.x + 0] = d_SrcVal[0];
    l_key[threadIdx.x + (LOCAL_SIZE_LIMIT / 2)] =
      d_SrcKey[(LOCAL_SIZE_LIMIT / 2)];
    l_val[threadIdx.x + (LOCAL_SIZE_LIMIT / 2)] =
      d_SrcVal[(LOCAL_SIZE_LIMIT / 2)];

    for (unsigned size = 2; size < arrayLength; size <<= 1) {
      // Bitonic merge
      unsigned dir = ((threadIdx.x & (size / 2)) != 0);
      for (unsigned stride = size / 2; stride > 0; stride >>= 1) {
        __syncthreads();
        unsigned pos = 2 * threadIdx.x - (threadIdx.x & (stride - 1));
        twoSort(
          &l_key[pos + 0], &l_val[pos + 0],
          &l_key[pos + stride], &l_val[pos + stride], dir);
      }
    }

    // dir == sortDir for the last bitonic merge step
    {
      for(unsigned stride = arrayLength / 2; stride > 0; stride >>= 1){
        __syncthreads();
        unsigned pos = 2 * threadIdx.x - (threadIdx.x & (stride - 1));
        twoSort(
          &l_key[pos + 0], &l_val[pos + 0],
          &l_key[pos + stride], &l_val[pos + stride], sortDir);
      }
    }

    __syncthreads();
    d_DstKey[0] = l_key[threadIdx.x + 0];
    d_DstVal[0] = l_val[threadIdx.x + 0];
    d_DstKey[(LOCAL_SIZE_LIMIT / 2)] =
      l_key[threadIdx.x + (LOCAL_SIZE_LIMIT / 2)];
    d_DstVal[(LOCAL_SIZE_LIMIT / 2)] =
      l_val[threadIdx.x + (LOCAL_SIZE_LIMIT / 2)];
  }
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Kernel for computing 256-bin histograms
struct Histogram : Kernel {
  int len;
  unsigned char* input;
  int* bins;

  void kernel() {
    // Store histogram bins in shared local memory
    int* histo = shared.array<int, 256>();

    // Initialise bins
    for (int i = threadIdx.x; i < 256; i += blockDim.x)
      histo[i] = 0;

    __syncthreads();

    // Update bins
    for (int i = threadIdx.x; i < len; i += blockDim.x)
      atomicAdd(&histo[input[i]], 1);

    __syncthreads();

    // Write bins to global memory
    for (int i = threadIdx.x; i < 256; i += blockDim.x)
      bins[i] = histo[i];
  }
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Matrix multiplication C = A * B
// (wA is A's width and wB is B's width)
template <int BlockSize> struct MatMul : Kernel {
  int *A, *B, *C;
  int wA, wB;

  void kernel() {
    // Declaration of the shared memory array As used to
    // store the sub-matrix of A
    auto As = shared.array<int, BlockSize, BlockSize>();
  
    // Declaration of the shared memory array Bs used to
    // store the sub-matrix of B
    auto Bs = shared.array<int, BlockSize, BlockSize>();

    // Block index
    int bx = blockIdx.x;
    int by = blockIdx.y;

    // Thread index
    int tx = threadIdx.x;
    int ty = threadIdx.y;

    // Index of the first sub-matrix of A processed by the block
    int aBegin = wA * BlockSize * by;

    // Index of the last sub-matrix of A processed by the block
    int aEnd   = aBegin + wA - 1;

    // Step size used to iterate through the sub-matrices of A
    int aStep  = BlockSize;

    // Index of the first sub-matrix of B processed by the block
    int bBegin = BlockSize * bx;

    // Step size used to iterate through the sub-matrices of B
    int bStep  = BlockSize * wB;

    // Csub is used to store the element of the block sub-matrix
    // that is computed by the thread
    int Csub = 0;

    // Loop over all the sub-matrices of A and B
    // required to compute the block sub-matrix
    for (int a = aBegin, b = bBegin;
         a <= aEnd;
         a += aStep, b += bStep) {

      // Load the matrices from device memory
      // to shared memory; each thread loads
      // one element of each matrix
      As[ty][tx] = A[a + wA * ty + tx];
      Bs[ty][tx] = B[b + wB * ty + tx];

      // Synchronize to make sure the matrices are loaded
      __syncthreads();

      // Multiply the two matrices together;
      // each thread computes one element
      // of the block sub-matrix
      for (int k = 0; k < BlockSize; ++k) {
        Csub += As[ty][k] * Bs[k][tx];
      }

      // Synchronize to make sure that the preceding
      // computation is done before loading two new
      // sub-matrices of A and B in the next iteration
      __syncthreads();
    }

    // Write the block sub-matrix to device memory;
    // each thread writes one element
    int c = wB * BlockSize * by + BlockSize * bx;
    C[c + wB * ty + tx] = Csub;
  }
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////



template <int BlockSize> struct MatVecMul : Kernel {
  int width, height;
  int *mat, *vecIn, *vecOut;
  
  void kernel() {
    // Partial dot products stored in shared local memory
    int* partial = shared.array<int, BlockSize>();

    for (int y = blockIdx.x; y < height; y += gridDim.x) {
      // Row processed by this block
      int* row = mat + y * width;

      // Compute partial dot products
      int sum = 0;
      for (int x = threadIdx.x; x < width; x += blockDim.x)
        sum += row[x] * vecIn[x];
      partial[threadIdx.x] = sum;
      __syncthreads();

      // Final local reduction
      for (int i = blockDim.x >> 1; i > 0; i >>= 1)  {
        if (threadIdx.x < i)
          partial[threadIdx.x] += partial[threadIdx.x + i];
        __syncthreads();
      }

      // Write dot product to global memory
      if (threadIdx.x == 0) vecOut[y] = partial[0];

      noclConverge();
    }
  }
};



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Slightly modified from original to make testing easier
// Kernel for vector summation
template <int BlockSize> struct Reduce : Kernel {
  int len;
  int acc;
  int *in, *sum;
  
  void kernel() {
    int* block = shared.array<int, BlockSize>();

    // Sum global memory
    block[threadIdx.x] = 0;
    for (int i = threadIdx.x; i < len; i += blockDim.x)
      block[threadIdx.x] += in[i];

    __syncthreads();

    // Sum shared local memory
    for(int i = blockDim.x >> 1; i > 0; i >>= 1)  {
      if (threadIdx.x < i)
        block[threadIdx.x] += block[threadIdx.x + i];
      __syncthreads();
    }

    // Write sum to global memory
    if (threadIdx.x == 0) *sum = block[0];
  }
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <int BlockSize> struct Scan : Kernel {
  int len;
  int *in, *out;

  void kernel() {
    // Shared arrays
    int* tempIn = shared.array<int, BlockSize>();
    int* tempOut = shared.array<int, BlockSize>();

    // Shorthand for local thread id
    int t = threadIdx.x;

    for (int x = 0; x < len; x += blockDim.x) {
      // Load data
      tempOut[t] = in[x+t];
      __syncthreads();

      // Local scan
      for (int offset = 1; offset < blockDim.x; offset <<= 1) {
        swap(tempIn, tempOut);
        if (t >= offset)
          tempOut[t] = tempIn[t] + tempIn[t - offset];
        else
          tempOut[t] = tempIn[t];
        __syncthreads();
      }

      // Store data
      int acc = x > 0 ? out[x-1] : 0;
      out[x+t] = tempOut[t] + acc;
    }
  }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Kernel for sparse matrix vector multipliation on ELLPACK format
// One thread per matrix row
struct SparseMatVecMul : Kernel {
  int num_rows;
  int num_cols;
  int num_cols_per_row;
  int* indices;
  int* data;
  int* x;
  int* y;

  void kernel() {
    int row = blockDim.x * blockIdx.x + threadIdx.x;
    if (row < num_rows) {
      int dot = 0;
      for (int n = 0; n < num_cols_per_row; n++) {
        int col = indices[num_rows * n + row];
        int val = data[num_rows * n + row];
        dot += val * x[col];
      }
      y[row] = dot;
    }
  }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Kernel for matrix transposition
// One sub-square at a time
template <int SquareSize> struct Transpose : Kernel {
  Array2D<int> in, out;
  
  void kernel() {
    auto square = shared.array<int, SquareSize, SquareSize+1>();
    
    // Origin of square within matrix
    int originX = blockIdx.x * blockDim.x;
    int originY = blockIdx.y * blockDim.x;
    
    // Load square
    for (int y = threadIdx.y; y < blockDim.x; y += blockDim.y)
      square[y][threadIdx.x] = in[originY + y][originX + threadIdx.x];
    
    __syncthreads();
    
    // Store square
    for (int y = threadIdx.y; y < blockDim.x; y += blockDim.y)
      out[originX + y][originY + threadIdx.x] = square[threadIdx.x][y];
  }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Kernel for adding vectors
struct VecAdd : Kernel {
  int len;
  int *a, *b, *result;

  void kernel() {
    for (int i = threadIdx.x; i < len; i += blockDim.x)
      result[i] = a[i] + b[i];
  }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

VecAdd vecAddCreate(int* a,int* b,int* result){
  // Vector size for benchmarking
  int N = isSim ? 3000 : 1000000;

  // Initialise inputs
  uint32_t seed = seed_type;
  for (int i = 0; i < N; i++) {
    a[i] = rand15(&seed);
    b[i] = rand15(&seed);
  }
  

  // Instantiate kernel
  VecAdd k;

  // Use a single block of threads
  k.blockDim.x = SIMTWarps * SIMTLanes;

  // Assign parameters
  k.len = N;
  k.a = a;
  k.b = b;
  k.result = result;
  return k;
}

Transpose<SIMTLanes> transposeCreate(int* matInData, int* matOutData)
{
  
  // Matrix size for benchmarking
  int width = isSim ? 256 : 512;
  int height = isSim ? 64 : 512;

  // Friendly array wrappers
  Array2D<int> matIn(matInData, height, width);
  Array2D<int> matOut(matOutData, width, height);

  // Initialise inputs
  uint32_t seed = seed_type;
  for (int i = 0; i < height; i++)
    for (int j = 0; j < width; j++)
      matIn[i][j] = rand15(&seed);

  // Number of loop iterations per block.  The number of iterations
  // times the block Y dimension must equal the block X dimension.
  const int itersPerBlock = 4;

  // Instantiate kernel
  Transpose<SIMTLanes> k;

  // Set block/grid dimensions
  k.blockDim.x = SIMTLanes;
  k.blockDim.y = SIMTLanes / itersPerBlock;
  k.gridDim.x = width / k.blockDim.x;
  k.gridDim.y = height / (itersPerBlock * k.blockDim.y);

  // Assign parameters
  k.in = matIn;
  k.out = matOut;

  return k;
}

SparseMatVecMul sparseMatVecMulCreate(int* dataT, int* indicesT,int* vecIn, int* vecOut, int* data, int* indices)
{
  
  // Vector and matrix dimensions for benchmarking
  // Should be powers of two
  int width = isSim ? 256 : 2048;
  int height = isSim ? 64 : 16384;

  // Sparsity of matrix (power of two)
  int sparsity = 8;
  int samplesPerRow = width / sparsity;

  // Initialise inputs
  uint32_t seed = seed_type;
  for (int i = 0; i < width; i++)
    vecIn[i] = rand15(&seed) & 0xff;
  for (int r = 0; r < height; r++) {
    vecOut[r] = 0;
    int offset = rand15(&seed) & (2*sparsity - 1);
    int n = 0;
    while (n < samplesPerRow) {
      data[r*samplesPerRow + n] = rand15(&seed) & 0xff;
      indices[r*samplesPerRow + n] = offset;
      n++;
      offset += rand15(&seed) & (2*sparsity-1);
      if (offset >= width) break;
    }
    while (n < samplesPerRow) {
      data[r*samplesPerRow + n] = 0;
      indices[r*samplesPerRow + n] = 0;
      n++;
    }
  }

  // Get matrix in column-major order
  for (int r = 0; r < height; r++)
    for (int n = 0; n < samplesPerRow; n++) {
      dataT[n * height + r] = data[r * samplesPerRow + n];
      indicesT[n * height + r] = indices[r * samplesPerRow + n];
    }

  // Instantiate kernel
  SparseMatVecMul k;

  // One thread per row
  int groups = height / SIMTLanes;
  k.blockDim.x = SIMTLanes;
  k.gridDim.x = groups < SIMTWarps ? SIMTWarps : groups;

  // Assign parameters
  k.num_rows = height;
  k.num_cols = width;
  k.num_cols_per_row = samplesPerRow;
  k.indices = indicesT;
  k.data = dataT;
  k.x = vecIn;
  k.y = vecOut;

  return k;
}

Scan<SIMTWarps * SIMTLanes> scanCreate(int* in, int* out)
{
  
  // Vector size for benchmarking
  // Should divide evenly by SIMT thread count
  int N = isSim ? 4096 : 1024000;

  // Initialise inputs
  uint32_t seed = seed_type;
  for (int i = 0; i < N; i++) {
    in[i] = rand15(&seed);
  }

  // Instantiate kernel
  Scan<SIMTWarps * SIMTLanes> k;

  // Use a single block of threads
  k.blockDim.x = SIMTWarps * SIMTLanes;

  // Assign parameters
  k.len = N;
  k.in = in;
  k.out = out;

  return k;
}

Reduce<SIMTWarps * SIMTLanes> reduceCreate(int* in, int* sum)
{

  // Vector size for benchmarking
  int N = isSim ? 3000 : 1000000;

  // Initialise inputs
  uint32_t seed = seed_type;
  int acc = 0;
  for (int i = 0; i < N; i++) {
    int r = rand15(&seed);
    in[i] = r;
    acc += r;
  }

  // Instantiate kernel
  Reduce<SIMTWarps * SIMTLanes> k;

  // Use a single block of threads
  k.blockDim.x = SIMTWarps * SIMTLanes;

  // Assign parameters
  k.len = N;
  k.in = in;
  k.sum = sum;
  k.acc = acc;

  return k;
}


MatVecMul<SIMTLanes> matVecMulCreate(int* mat, int* vecIn, int* vecOut)
{

  // Vector and matrix dimensions for benchmarking
  int width = isSim ? 128 : 1024;
  int height = isSim ? 64 : 1024;

  // Initialise inputs
  uint32_t seed = seed_type;
  for (int j = 0; j < width; j++)
    vecIn[j] = rand15(&seed) & 0xff;
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++)
      mat[i*width+j] = rand15(&seed) & 0xff;
  }

  // Instantiate kernel
  MatVecMul<SIMTLanes> k;

  // One block of threads per matrix row
  k.blockDim.x = SIMTLanes;
  k.gridDim.x = SIMTWarps;

  // Assign parameters
  k.width = width;
  k.height = height;
  k.mat = mat;
  k.vecIn = vecIn;
  k.vecOut = vecOut;

  return k;
}

MatMul<SIMTLanes> matMulCreate(int* matA, int* matB, int* matC, int* matCheck)
{
  
  // Matrix dimensions for benchmarking
  // (Must be a multiple of SIMTLanes)
  int size = isSim ? 64 : 256;


  // Initialise matrices
  uint32_t seed = seed_type;
  for (int i = 0; i < size; i++)
    for (int j = 0; j < size; j++) {
      matA[i*size+j] = rand15(&seed) & 0xff;
      matB[i*size+j] = rand15(&seed) & 0xff;
      matCheck[i*size+j] = 0;
    }

  // Instantiate kernel
  MatMul<SIMTLanes> k;

  // One block of threads per matrix tile
  k.blockDim.x = SIMTLanes;
  k.blockDim.y = SIMTLanes;
  k.gridDim.x = size / SIMTLanes;
  k.gridDim.y = size / SIMTLanes;

  // Assign parameters
  k.wA = size;
  k.wB = size;
  k.A = matA;
  k.B = matB;
  k.C = matC;

  return k;
}


Histogram histogramCreate(unsigned char* input, int* bins)
{
  // Vector size for benchmarking
  int N = isSim ? 3000 : 1000000;

  // Initialise inputs
  uint32_t seed = seed_type;
  for (int i = 0; i < N; i++)
    input[i] = rand15(&seed) & 0xff;

  // Instantiate kernel
  Histogram k;

  // Use single block of threads
  k.blockDim.x = SIMTLanes * SIMTWarps;

  // Assign parameters
  k.len = N;
  k.input = input;
  k.bins = bins;

  return k;
}

BitonicSortLocal bitonicSortLocalCreate(unsigned int* srcKeys,unsigned int* srcVals,unsigned int* dstKeys,unsigned int* dstVals)
{

  // Array size and number of arrays for benchmarking
  int N = LOCAL_SIZE_LIMIT;
  int batch = isSim ? 4 : 64;

  // Initialise inputs
  uint32_t seed = seed_type;
  for (int i = 0; i < N*batch; i++) {
    srcKeys[i] = rand15(&seed);
    srcVals[i] = rand15(&seed);
  }

  // Instantiate kernel
  BitonicSortLocal k;

  // Use a single block of threads per array
  k.blockDim.x = LOCAL_SIZE_LIMIT / 2;
  k.gridDim.x = batch;

  // Assign parameters
  k.d_DstKey_arg = dstKeys;
  k.d_DstVal_arg = dstVals;
  k.d_SrcKey_arg = srcKeys;
  k.d_SrcVal_arg = srcVals;
  k.arrayLength = N;
  k.sortDir = 1;

  return k;
}

int vecAddTest(VecAdd k)
{
  // Check result
  bool ok = true;
  for (int i = 0; i < k.len; i++)
    ok = ok && k.result[i] == k.a[i] + k.b[i];

  // Display result
  puts("VecAdd Self test: ");
  puts(ok ? "PASSED" : "FAILED");
  putchar('\n');

  return 0;
}

int transposeTest(Transpose<SIMTLanes> k)
{

  // Matrix size for benchmarking
  int width = isSim ? 256 : 512;
  int height = isSim ? 64 : 512;

  // Check result
  bool ok = true;
  for (int i = 0; i < width; i++)
    for (int j = 0; j < height; j++)
      ok = ok && k.out[i][j] == k.in[j][i];

  // Display result
  puts("Transpose Self test: ");
  puts(ok ? "PASSED" : "FAILED");
  putchar('\n');

  return 0;
}

int sparseMatVecMulTest(SparseMatVecMul k, int* data, int* indices)
{
  // Check result
  bool ok = true;
  for (int r = 0; r < k.num_rows; r++) {
    int sum = 0;
    for (int n = 0; n < k.num_cols_per_row; n++) {
      int i = r*k.num_cols_per_row + n;
      if (data[i] != 0) sum += data[i] * k.x[indices[i]];
    }
    ok = ok && sum == k.y[r];
  }

  // Display result
  puts("SparseMatVecMul Self test: ");
  puts(ok ? "PASSED" : "FAILED");
  putchar('\n');

  return 0;
}
int scanTest(Scan<SIMTWarps * SIMTLanes> k)
{

  // Check result
  bool ok = true;
  int acc = 0;
  for (int i = 0; i < k.len; i++) {
    acc += k.in[i];
    ok = ok && k.out[i] == acc;
  }

  // Display result
  puts("Scan Self test: ");
  puts(ok ? "PASSED" : "FAILED");
  putchar('\n');

  return 0;
}

int reduceTest(Reduce<SIMTWarps * SIMTLanes> k)
{
  

  // Check result
  bool ok = *(k.sum) == k.acc;

  // Display result
  puts("Reduce Self test: ");
  puts(ok ? "PASSED" : "FAILED");
  putchar('\n');

  return 0;
}


int matVecMulTest(MatVecMul<SIMTLanes> k)
{
  // Check result
  bool ok = true;
  for (int i = 0; i < k.height; i++) {
    int sum = 0;
    for (int j = 0; j < k.width; j++)
      sum += k.mat[i*k.width+j] * k.vecIn[j];
    ok = ok && sum == k.vecOut[i];
  }

  // Display result
  puts("MatVecMul Self test: ");
  puts(ok ? "PASSED" : "FAILED");
  putchar('\n');

  return 0;
}

int matMulTest(MatMul<SIMTLanes> k, int* matCheck)
{
  // Check result
  bool ok = true;
  int size = k.wA;
  for (int i = 0; i < size; i++)
    for (int j = 0; j < size; j++)
      for (int x = 0; x < size; x++)
        matCheck[i*size+j] += k.A[i*size+x] * k.B[x*size+j];
  for (int i = 0; i < size; i++)
    for (int j = 0; j < size; j++)
      ok = ok && matCheck[i*size+j] == k.C[i*size+j];

  // Display result
  puts("MatMul Self test: ");
  puts(ok ? "PASSED" : "FAILED");
  putchar('\n');

  return 0;
}

int histogramTest(Histogram k)
{
  // Check result
  bool ok = true;
  int goldenBins[256];
  for (int i = 0; i < 256; i++) goldenBins[i] = 0;
  for (int i = 0; i < k.len; i++) goldenBins[k.input[i]]++;
  for (int i = 0; i < 256; i++)
    ok = ok && k.bins[i] == goldenBins[i];

  // Display result
  puts("Histogram Self test: ");
  puts(ok ? "PASSED" : "FAILED");
  putchar('\n');

  return 0;
}

int bitonicSortLocalTest(BitonicSortLocal k)
{
  // Check result
  bool ok = true;
  for (int b = 0; b < k.gridDim.x; b++)
    for (int i = 0; i < k.arrayLength-1; i++)
      ok = ok && k.d_DstKey_arg[b*k.arrayLength+i] <= k.d_DstKey_arg[b*k.arrayLength+i+1];

  // Display result
  puts("BitonicSortLocal Self test: ");
  puts(ok ? "PASSED" : "FAILED");
  putchar('\n');

  return 0;
}





int main()
{

  // Are we in simulation?
  bool isSim = getchar();

  // Vector Input and creation
  int N = isSim ? 3000 : 1000000;
  simt_aligned int a[N], b[N], result[N];
  VecAdd va = vecAddCreate(a,b,result);


  // Matrix size for benchmarking
  int width = isSim ? 256 : 512;
  int height = isSim ? 64 : 512;
  nocl_aligned int matInData[width*height];
  nocl_aligned int matOutData[width*height];
  Transpose<SIMTLanes> tp = transposeCreate(matInData, matOutData);

  // Vector and matrix dimensions for benchmarking
  width = isSim ? 256 : 2048;
  height = isSim ? 64 : 16384;
  int sparsity = 8;
  int samplesPerRow = width / sparsity;
  simt_aligned int data[samplesPerRow * height],
                   indices[samplesPerRow * height],
                   dataT[samplesPerRow * height],
                   indicesT[samplesPerRow * height],
                   vecIn[width*2], vecOut[height];
  SparseMatVecMul smvm = sparseMatVecMulCreate(dataT,indicesT,vecIn,vecOut,data,indices);

  // Vector size for benchmarking
  N = isSim ? 4096 : 1024000;
  simt_aligned int in[N], out[N];
  Scan<SIMTWarps * SIMTLanes> sn = scanCreate(in,out);


  // Vector size for benchmarking
  N = isSim ? 3000 : 1000000;
  simt_aligned int reduceIn[N];
  int sum;
  Reduce<SIMTWarps * SIMTLanes> rd = reduceCreate(reduceIn, &sum);

  // Vector and matrix dimensions for benchmarking
  width = isSim ? 128 : 1024;
  height = isSim ? 64 : 1024;
  simt_aligned int mat[height*width], mvmVecIn[width], mvmVecOut[height];

  MatVecMul<SIMTLanes> mvm = matVecMulCreate(mat,mvmVecIn, mvmVecOut);

  // Matrix dimensions for benchmarking
  // (Must be a multiple of SIMTLanes)
  int size = isSim ? 64 : 256;
  simt_aligned int matA[size*size], matB[size*size],
                   matC[size*size], matCheck[size*size];
  MatMul<SIMTLanes> mm = matMulCreate(matA,matB,matC,matCheck);

  // Vector size for benchmarking
  N = isSim ? 3000 : 1000000;
  nocl_aligned unsigned char input[N];
  nocl_aligned int bins[256];

  Histogram hg = histogramCreate(input,bins);


  // Array size and number of arrays for benchmarking
  N = LOCAL_SIZE_LIMIT;
  int batch = isSim ? 4 : 64;
  simt_aligned unsigned srcKeys[N*batch], srcVals[N*batch];
  simt_aligned unsigned dstKeys[N*batch], dstVals[N*batch];

  BitonicSortLocal bsl = bitonicSortLocalCreate(srcKeys,srcVals,dstKeys,dstVals);


  bool timing_overall = true;
  uint64_t before;
  uint64_t after;
  
  noclMapping(&va);
  noclMapping(&tp);
  noclMapping(&mm);
  noclMapping(&bsl);
  noclMapping(&smvm);
  noclMapping(&sn);
  noclMapping(&rd);
  noclMapping(&mvm);
  noclMapping(&hg);

  Kernel * arr[9] = {&va,&tp,&mm,&bsl,&smvm,&sn,&rd,&mvm,&hg};
  
  
  uint64_t latency;
  uint64_t compute_times;

  for (int i = 0; i < 9; i++)
  {
    arr[i]->kernelID = i;
    arr[i]->priority = 1;
  }

  arr[1]->priority = 8;
  arr[2]->priority = 8;
  arr[3]->priority = 4;
  
  
  before = pebblesCycleCount();
  Output output = noclScheduler(arr, 9, 8);
  latency = output.final_time - before;
  
  
  /*
  compute_times = output.final_time - output.intitial_time;
  puts("Latency:");
  putchar('\n');
  puthex64(latency);
  putchar('\n');
  

  puts("Compute Times:");
  putchar('\n');
  puthex64(compute_times);
  putchar('\n');
  */
  
  
  
  vecAddTest(va);
  transposeTest(tp);
  matMulTest(mm, matCheck);
  bitonicSortLocalTest(bsl);
  sparseMatVecMulTest(smvm,data,indices);
  scanTest(sn);
  reduceTest(rd);
  matVecMulTest(mvm);
  histogramTest(hg);
  

  return 0;
}
