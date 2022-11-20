#include <iostream>
#include <mpi.h>
#include <vector>
#include <cmath>

#include <chrono>
#include <thread>

const int RAND_MAX_VALUE = 10;
const int N = 5;

using namespace std;

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    int x[N];           // initial vector x
    int y[N];           // initial vector y

    int MAIN_PROCESS = 0;

    if (rank == MAIN_PROCESS) {
        srand(time(nullptr));
        
        for (int i = 0; i < N; i++) {
            x[i] = rand() % RAND_MAX_VALUE - rand() % RAND_MAX_VALUE;
            y[i] = rand() % RAND_MAX_VALUE - rand() % RAND_MAX_VALUE;
        }
        
        printf("\x1B[36m Initialized vectors (%d elements) on process №%d;\n X: ", N, rank);
        for (int i = 0; i < N; i++) {
            printf("%3d ", x[i]);
        }
        printf("\n Y: ");
        for (int i = 0; i < N; i++) {
            printf("%3d ", y[i]);
        }
        printf("\033[0m\n");
    }

    int elemsCount = N / size + (N % size != 0);

    int x1[elemsCount];          // received on each process - part of vector x
    int y1[elemsCount];          // received on each process - part of vector y

    MPI_Scatter(&x, elemsCount, MPI_INT, &x1, elemsCount, MPI_INT, MAIN_PROCESS, MPI_COMM_WORLD);
    MPI_Scatter(&y, elemsCount, MPI_INT, &y1, elemsCount, MPI_INT, MAIN_PROCESS, MPI_COMM_WORLD);

    int localMultiplication = 0;
    for (int i = 0; i < elemsCount && rank * elemsCount + i < N; i++) {
        localMultiplication += x1[i] * y1[i];
    }

    int result = 0;
    MPI_Reduce(&localMultiplication, &result, 1, MPI_INT, MPI_SUM, MAIN_PROCESS, MPI_COMM_WORLD);

    if (rank == MAIN_PROCESS) {
        printf("\x1B[33m Got result = %d \n\033[0m", result);
    }

    MPI_Finalize();
    return 0;
}
