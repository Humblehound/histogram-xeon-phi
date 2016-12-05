#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <omp.h>
#include "BmpImage.h"


const int BIN_COUNT = 256;

bool VALIDATE = true;

void histogram_cpu(int *host_data, size_t data_size, int* histogram){
    for(int i=0;i<data_size;i++){
        histogram[host_data[i]]++;
    }
}

int main(int argc, char **argv) {

    int *cpu_data, *cpu_results, *device_data, *device_results;

    BmpImage *image = new BmpImage();
    image->Load("sample8.bmp");
    printf("Image size: %dx%d\n", image->GetWidth(), image->GetHeight());
    printf("Data size : %d Bytes\n", image->GetSize());
    std::vector<int> pixelLuminosity = image->GetLuminosityVector();

    size_t dataSize = pixelLuminosity.size();
    size_t arraySize = dataSize * sizeof(int);

    cpu_data = (int *) malloc(arraySize);
    device_data = (int *) malloc(arraySize);
    cpu_results = (int *) calloc(BIN_COUNT, sizeof(int));
    device_results = (int *) calloc(BIN_COUNT, sizeof(int));

    std::copy(pixelLuminosity.begin(), pixelLuminosity.end(), cpu_data);
    std::copy(pixelLuminosity.begin(), pixelLuminosity.end(), device_data);

    double elapsed = 0;
    double start = 0;

    #pragma offload target(mic) in(dataSize) in(device_data:length(dataSize)) out(device_results:length(256)) in(start) out(elapsed)
    {
        start = omp_get_wtime();
        #pragma omp parallel for num_threads(1)
        for(int i=0;i<dataSize;i++){
            #pragma omp atomic
            device_results[device_data[i]]++;
        }
        elapsed = omp_get_wtime() - start;
    }

    printf("Elapsed: %f\n", elapsed);

    if(VALIDATE){
        histogram_cpu(cpu_data, dataSize, cpu_results);

        int diff = 0;
        for(int i=0;i<BIN_COUNT;i++){
            if(cpu_results[i] != device_results[i]){
                diff+=cpu_results[i]-device_results[i];
            }
        }

        if(diff != 0){
            printf("Histogram calculation error, total sum difference: %d\n", diff);
        }else{
            printf("All good!\n");
        }
    }

}