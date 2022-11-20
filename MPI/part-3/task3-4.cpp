#include <iostream>
#include <mpi.h>
#include <cmath>

const int RAND_MAX_VALUE = 100;
const int M = 5;
const int N = 4;

using namespace std;

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    int a[M][N];             // initial matrix a
    int result;

    int MAIN_PROCESS = 0;

    if (rank == MAIN_PROCESS) {
        srand(time(nullptr));
        
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                a[i][j] = rand() % RAND_MAX_VALUE - rand() % RAND_MAX_VALUE;
            }
        }
        
        printf("\x1B[36m Initialized matrix (%d x %d) on process №%d:\n", M, N, rank);
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                printf("%3d ", a[i][j]);
            }
            printf("\n");
        }    
        printf("\033[0m\n");
    }

    int rowsCount = M / size + (M % size != 0);

    int a1[rowsCount][N];           // received on each process - part of matrix a

    MPI_Scatter(&a, rowsCount * N, MPI_INT, &a1, rowsCount * N, MPI_INT, MAIN_PROCESS, MPI_COMM_WORLD);

    int localMax = 0;
    for (int i = 0; i < rowsCount && rank * rowsCount + i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (localMax < abs(a1[i][j]))
                localMax = abs(a1[i][j]);
        }
    }

    MPI_Reduce(&localMax, &result, 1, MPI_INT, MPI_MAX, MAIN_PROCESS, MPI_COMM_WORLD);

    if (rank == MAIN_PROCESS) {
        printf("\x1B[33m Got result: %d \033[0m \n", result);
    }

    MPI_Finalize();
    return 0;
}
