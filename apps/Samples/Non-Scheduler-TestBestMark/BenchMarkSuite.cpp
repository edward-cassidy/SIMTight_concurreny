#include <NoCL.h>
#include <Rand.h>
#include <Pebbles/CSRs/CycleCount.h>

struct VecAdd : Kernel {
  int len;
  int *a, *b, *result;

  void kernel() {
    for (int i = threadIdx.x; i < len; i += blockDim.x)
      result[i] = a[i] + b[i];
  }
};

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

int main()
{
  // Are we in simulation?
  bool isSim = getchar();

  // Vector size for benchmarking
  int N = isSim ? 3000 : 1000000;

  // Input and output vectors
  simt_aligned int a[N], b[N], result[N];

  // Initialise inputs
  uint32_t seed = 1;
  for (int i = 0; i < N; i++) {
    a[i] = rand15(&seed);
    b[i] = rand15(&seed);
  }

  // Instantiate kernel
  VecAdd k1;

  // Use a single block of threads
  k1.blockDim.x = SIMTWarps * SIMTLanes;

  // Assign parameters
  k1.len = N;
  k1.a = a;
  k1.b = b;
  k1.result = result;

    //////////////////////////////////////////////////////////////////////////////////

  // Matrix size for benchmarking
  int width = isSim ? 256 : 512;
  int height = isSim ? 64 : 512;

  // Input and output matrix data
  nocl_aligned int matInData[width*height];
  nocl_aligned int matOutData[width*height];

  // Friendly array wrappers
  Array2D<int> matIn(matInData, height, width);
  Array2D<int> matOut(matOutData, width, height);

  // Initialise inputs
  for (int i = 0; i < height; i++)
    for (int j = 0; j < width; j++)
      matIn[i][j] = rand15(&seed);

  // Number of loop iterations per block.  The number of iterations
  // times the block Y dimension must equal the block X dimension.
  const int itersPerBlock = 4;

  // Instantiate kernel
  Transpose<SIMTLanes> k2;

  // Set block/grid dimensions
  k2.blockDim.x = SIMTLanes;
  k2.blockDim.y = SIMTLanes / itersPerBlock;
  k2.gridDim.x = width / k2.blockDim.x;
  k2.gridDim.y = height / (itersPerBlock * k2.blockDim.y);

  // Assign parameters
  k2.in = matIn;
  k2.out = matOut;

  /////////////////////////////////////////////////////////////////////////////////////

  // Invoke kernel
  int beforeVecAdd = pebblesCycleCount();
  noclRunKernel(&k1);
  int AfterVecAdd = pebblesCycleCount();
  noclRunKernel(&k2);
  int AfterTranpose = pebblesCycleCount();


  // Check result
  bool ok = true;
  for (int i = 0; i < width; i++)
    for (int j = 0; j < height; j++)
      ok = ok && matOut[i][j] == matIn[j][i];

  
  // Display result
  puts("Matrix Self test: ");
  puts(ok ? "PASSED" : "FAILED");
  putchar('\n');

  

  // Check result
  ok = true;
  for (int i = 0; i < N; i++)
    ok = ok && result[i] == a[i] + b[i];

  // Display result
  puts("Self test VecAdd: ");
  puts(ok ? "PASSED" : "FAILED");
  putchar('\n');

  puts("Before VecAdd: ");
  puthex(beforeVecAdd);
  putchar('\n');
  puts("After VecAdd, Before Transpose: ");
  puthex(AfterVecAdd);
  putchar('\n');
  puts("After Transpose:");
  puthex(AfterTranpose);
  putchar('\n');
  puts("Cycles taken by VecAdd: ");
  puthex(AfterVecAdd - beforeVecAdd);
  putchar('\n');
  puts("Cycles taken by Transpose: ");
  puthex(AfterTranpose - AfterVecAdd);
  putchar('\n');

  return 0;
}
