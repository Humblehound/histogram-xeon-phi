
// System includes
#include <stdio.h>
#include "BmpImage.h"

// CUDA runtime
#include <cuda_runtime.h>
#include <helper_cuda.h>

using namespace std;

const int BIN_COUNT=256;
const int THREADS_PER_BLOCK=256;

int BLOCK_COUNT=8000;

void histogram_cpu(int *host_data, size_t data_size, int* histogram){
    for(int i=0;i<data_size;i++){
        histogram[host_data[i]]++;
    }
}
    
__global__ void histogram(const int *d_data, int *d_results, size_t dataSize, int threadCount, int pixelsPerThread)
{
       __shared__ unsigned int local_results[256];
       local_results[threadIdx.x] = 0;
       __syncthreads();

      int id = blockDim.x * blockIdx.x + threadIdx.x;
      int startId = pixelsPerThread*id;
      int endId = startId+pixelsPerThread;

      if(endId > dataSize){
        endId = dataSize;
      }

      for(int i=startId;i<endId;i++){
        atomicAdd(&local_results[d_data[i]], 1);        
      }
      __syncthreads();

      atomicAdd(&(d_results[threadIdx.x]), local_results[threadIdx.x] );
 }

int main(int argc, char **argv)
{

    int *host_data, *cpu_data, *host_results, *cpu_results;
    int *d_data, *d_results;
    cudaEvent_t start, stop;

    BmpImage* image = new BmpImage();
    image->Load("sample.bmp");

    printf("Image size: %dx%d\n", image->GetWidth(), image->GetHeight());
    printf("Data size : %d Bytes\n", image->GetSize());
    vector<int> pixelLuminosity = image->GetLuminosityVector();;

    size_t dataSize = pixelLuminosity.size();
    int arraySize = dataSize * sizeof(int);

    host_data = (int*) malloc(arraySize);
    cpu_data = (int*) malloc(arraySize);
    host_results = (int*) calloc(BIN_COUNT, sizeof(int));
    cpu_results = (int*) calloc(BIN_COUNT, sizeof(int));

    std::copy(pixelLuminosity.begin(), pixelLuminosity.end(), host_data);
    std::copy(pixelLuminosity.begin(), pixelLuminosity.end(), cpu_data);

    histogram_cpu(cpu_data, dataSize, cpu_results);

    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    checkCudaErrors(cudaMalloc((void **)&d_results, BIN_COUNT*sizeof(int)));
    checkCudaErrors(cudaMalloc((void **)&d_data, arraySize));
    checkCudaErrors(cudaMemset(d_results, 0, BIN_COUNT*sizeof(int)));
    checkCudaErrors(cudaMemcpy(d_data, host_data, arraySize, cudaMemcpyHostToDevice));

    int threadCount = BLOCK_COUNT * THREADS_PER_BLOCK;
    int pixelsPerThread = (dataSize/threadCount) + 1;

    printf("CUDA kernel launch with %d blocks\n", BLOCK_COUNT);
    printf("A total of %d threads launched\n", threadCount);
    printf("Pixels per thead: %d\n", pixelsPerThread);

    cudaEventRecord(start);
    histogram<<<BLOCK_COUNT, THREADS_PER_BLOCK>>>(d_data, d_results, dataSize, threadCount, pixelsPerThread);
    cudaEventRecord(stop);
    getLastCudaError("histogram execution failed\n");
    checkCudaErrors(cudaMemcpy(host_results, d_results, BIN_COUNT*sizeof(int), cudaMemcpyDeviceToHost));

    int diff = 0;

    for(int i=0;i<BIN_COUNT;i++){
        if(cpu_results[i] != host_results[i]){
            diff+=cpu_results[i]-host_results[i];
        }
    }
    if(diff != 0){
        printf("Histogram calculation error, total sum difference: %d\n", diff);
    }

    cudaEventSynchronize(stop);
    float milliseconds = 0;
    cudaEventElapsedTime(&milliseconds, start, stop);
    printf("Elapsed time: %4.2fms", milliseconds);
    printf("Done\n");
    exit(0);
}