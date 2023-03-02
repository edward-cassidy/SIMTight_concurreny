#include <NoCL.h>
#include <Rand.h>
#include <Pebbles/CSRs/CycleCount.h>


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


struct VecAdd : Kernel {
  int len;
  int *a, *b, *result;

  void kernel() {
    for (int i = threadIdx.x; i < len; i += blockDim.x)
      result[i] = a[i] + b[i];
  }
};


int main()
{
  puts("start");
  putchar('\n');

  
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
  VecAdd k;

  // Use a single block of threads
  k.blockDim.x = SIMTWarps * SIMTLanes;

  // Assign parameters
  k.len = N;
  k.a = a;
  k.b = b;
  k.result = result;



  /////////////////////////////////////////////////////////////////////////////////
   // Are we in simulation?
  

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
  seed = 1;
  for (int i = 0; i < height; i++)
    for (int j = 0; j < width; j++)
      matIn[i][j] = i*256 + j;

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



  /////////////////////////////////////////////////////////////////////////////////
  //Setting static priorities of kernels
  k.priority = 0;
  k2.priority = 2;

  // Invoke kernel

  k.name = 0; //0 is VecAdd
  k2.name = 1; //1 is Transpose
  
  Kernel *arr[2] = {&k, &k2};
  mapping_func(&k2);
  mapping_func(&k);
  int before = pebblesCycleCount();
  scheduler(arr, 2);
  int after = pebblesCycleCount();

  



  // Check result

  bool ok = true;
  for (int i = 0; i < N; i++)
    ok = ok && result[i] == a[i] + b[i];

  // Display result
  puts("Self test VecADD: ");
  puts(ok ? "PASSED" : "FAILED");
  putchar('\n');


  // Check result
  ok = true;
  bool check = false;
  for (int i = 0; i < width; i++){
    for (int j = 0; j < height; j++){
      ok = ok && matOut[i][j] == matIn[j][i];
    }
  }
  

  // Display result
  puts("Self test Matrix: ");
  puts(ok ? "PASSED" : "FAILED");
  putchar('\n');

  puts("Cycle taken by VecAdd: ");
  puthex(k.cycle_count - before);
  putchar('\n');


  puts("Cycle taken by Matrix: ");
  puthex(k2.cycle_count - before);
  putchar('\n');

  puts("Total cycle taken: ");
  puthex(after - before);
  putchar('\n');


  return 0;
 
}



