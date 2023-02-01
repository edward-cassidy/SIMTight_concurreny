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
    /* code */
    return 0;
}
