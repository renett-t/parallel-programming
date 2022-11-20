#include <iostream>
#include <mpi.h>
#include <vector>
#include <cmath>

#include <chrono>
#include <thread>

const int RAND_MAX_VALUE = 100;
const int N = 120;

using namespace std;

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    int x[N];           // initial vector x
    int x1[N];          // received on each process - part of vector x

    int MAIN_PROCESS = 0;

    if (rank == MAIN_PROCESS) {
        srand(time(nullptr));
        
        for (int i = 0; i < N; i++) {
            x[i] = rand() % RAND_MAX_VALUE - rand() % RAND_MAX_VALUE;
        }
        
        printf("\x1B[36m Initialized vector (%d elements) on process №%d\n", N, rank);
        for (int i = 0; i < N; i++) {
            printf("%3d ", x[i]);
        }
        printf("\033[0m\n");
    }

    int elemsCount = N / size + (N % size != 0);

    MPI_Scatter(&x, elemsCount, MPI_INT, &x1, elemsCount, MPI_INT, MAIN_PROCESS, MPI_COMM_WORLD);

    int localAbs = 0;
    for (int i = 0; i < elemsCount && rank * elemsCount + i < N; i++) {
        localAbs += abs(x1[i]);
    }

    int result = 0;
    MPI_Reduce(&localAbs, &result, 1, MPI_INT, MPI_SUM, MAIN_PROCESS, MPI_COMM_WORLD);

    if (rank == MAIN_PROCESS) {
        printf("\x1B[33m Got result ||x|| = %d \n\033[0m", result);
    }

    MPI_Finalize();
    return 0;
}
