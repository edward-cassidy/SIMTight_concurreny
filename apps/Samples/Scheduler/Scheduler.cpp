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


int main()
{
  puts("start");
  putchar('\n');
  int width = 256;
  int height = 64;
  
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
  putchar('\n');
  mapping_func<Transpose<SIMTLanes>>(&k);
  Kernel *array[1] = {&k};
  puts("Before Scheduler");
  putchar('\n');
  
  scheduler(array, 1);
  puts("After Scheduler");
  putchar('\n');

  bool ok = true;
  bool check = false;
  for (int i = 0; i < width; i++){
    for (int j = 0; j < height; j++){
      ok = ok && matOut[i][j] == matIn[j][i];
    }
  }
    

  // Display result
  puts("Self test: ");
  puts(ok ? "PASSED" : "FAILED");
  putchar('\n');
  return 0;
 
}



