#include <NoCL.h>
#include <Rand.h>

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
  int N = 3000;
  simt_aligned int a[N], b[N], result[N];

  // Initialise inputs
  uint32_t seed2 = 1;
  for (int i = 0; i < N; i++) {
    a[i] = rand15(&seed2);
    b[i] = rand15(&seed2);
  }

  // Instantiate kernel
  VecAdd vecadd;

  // Use a single block of threads
  vecadd.blockDim.x = SIMTWarps * SIMTLanes;

  // Assign parameters
  vecadd.len = N;
  vecadd.a = a;
  vecadd.b = b;
  vecadd.result = result;
  vecadd.blockIdx.x = 0;
  vecadd.blockIdx.y = 0;



  // Are we in simulation?
  bool isSim = getchar();

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
  uint32_t seed = 1;
  for (int i = 0; i < height; i++)
    for (int j = 0; j < width; j++)
      matIn[i][j] = i*256 + j;

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

  //Assign thread IDs


  k.blockIdx.x = 0;
  k.blockIdx.y = 0;


  // Assign parameters
  k.in = matIn;
  k.out = matOut;

  puts("before mapping");
  mapping_func<Transpose<SIMTLanes>>(&k);
  mapping_func<VecAdd>(&vecadd);
  Kernel *array[2] = {&k, &vecadd};
  puts("Before Scheduler");
  scheduler(array, 2);
  puts("After Scheduler");
  
 

  /*
  // Invoke kernel
  mapping_func<Transpose<SIMTLanes>>(&k);
  int counter = 0;
  puthex(k.blockIdx.x);
  putchar('\n');
  puthex(k.blockIdx.y);
  putchar('\n');
  puthex(k.gridDim.x);
  putchar('\n');
  puthex(k.gridDim.y);
  putchar('\n');
  while (k.blockIdx.y < k.gridDim.y) {
    while (k.blockIdx.x < k.gridDim.x) {
      counter++;
      puthex(k.blockIdx.x);
      putchar('\n');
      puthex(k.blockIdx.y);
      putchar('\n');
      puts("strange");
      putchar('\n');
      go_func(&k);
      puthex(k.blockIdx.x);
      putchar('\n');
      puthex(k.blockIdx.y);
      putchar('\n');
      k.blockIdx.x += k.map.numXBlocks;

    }
    k.blockIdx.x = 0;
    k.blockIdx.y += k.map.numYBlocks;
  }
  */



  // Check result
  bool ok = true;
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
  
  ok = true;
  for (int i = 0; i < N; i++)
    ok = ok && result[i] == a[i] + b[i];

  // Display result
  puts("Self test VecAdd: ");
  puts(ok ? "PASSED" : "FAILED");
  putchar('\n');
  


  return 0;
}
