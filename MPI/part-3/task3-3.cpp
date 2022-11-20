#include <iostream>
#include <mpi.h>
#include <vector>
#include <cmath>

#include <chrono>
#include <thread>

const int RAND_MAX_VALUE = 10;
const int ROWS_A = 5;
const int COLUMNS_A = 4;

using namespace std;

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    int a[ROWS_A][COLUMNS_A];           // initial matrix a
    int b[COLUMNS_A];                   // initial vector b
    int result[ROWS_A];              // result vector

    int MAIN_PROCESS = 0;

    if (rank == MAIN_PROCESS) {
        srand(time(nullptr));
        
        for (int i = 0; i < ROWS_A; i++) {
            for (int j = 0; j < COLUMNS_A; j++) {
                a[i][j] = rand() % RAND_MAX_VALUE;
            }
        }
        for (int j = 0; j < COLUMNS_A; j++) {
            b[j] = rand() % RAND_MAX_VALUE;
        }
        
        printf("\x1B[36m Initialized matrix and vector on process №%d; A: \n", rank);
        for (int i = 0; i < ROWS_A; i++) {
            for (int j = 0; j < COLUMNS_A; j++) {
                printf("%3d ", a[i][j]);
            }
            printf("\n");
        }
        printf("b:\n");
        for (int i = 0; i < COLUMNS_A; i++) {
            printf("%3d ", b[i]);
        }
    
        printf("\033[0m\n");
    }

    int rowsCount = ROWS_A / size + (ROWS_A % size != 0);

    int a1[rowsCount][COLUMNS_A];           // received on each process - part of matrix a

    MPI_Bcast(&b, COLUMNS_A, MPI_INT, MAIN_PROCESS, MPI_COMM_WORLD);
    MPI_Scatter(&a, rowsCount * COLUMNS_A, MPI_INT, &a1, rowsCount * COLUMNS_A, MPI_INT, MAIN_PROCESS, MPI_COMM_WORLD);

    int localRes[rowsCount];
    for (int i = 0; i < rowsCount && rank * rowsCount + i < ROWS_A; i++) {
        localRes[i] = 0;
        for (int j = 0; j < COLUMNS_A; j++) {
            localRes[i] += a1[i][j] * b[j];
        }
    }

    MPI_Gather(&localRes, rowsCount, MPI_INT, &result[rank * rowsCount], rowsCount, MPI_INT, MAIN_PROCESS, MPI_COMM_WORLD);
    
    if (rank == MAIN_PROCESS) {
        printf("\x1B[33m Got result: \n");
        for (int i = 0; i < ROWS_A; i++) {
            printf("%2d ", result[i]);
        }
        printf("\n\033[0m");
    }

    MPI_Finalize();
    return 0;
}
