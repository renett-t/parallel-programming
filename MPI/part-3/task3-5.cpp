#include <iostream>
#include <mpi.h>
#include <vector>
#include <cmath>

#include <chrono>
#include <thread>

const int RAND_MAX_VALUE = 10;
const int M = 5;
const int N = 4;

using namespace std;

void print_matrix(int matrix[M][N]) {
   for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            printf("%3d ", matrix[i][j]);
        }            
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    int a[M][N];
    int b[M][N];
    int c[M][N];

    int MAIN_PROCESS = 0;

    if (rank == MAIN_PROCESS) {
        srand(time(nullptr));
        
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                a[i][j] = rand() % RAND_MAX_VALUE;
                b[i][j] = rand() % RAND_MAX_VALUE;
            }
        }
        
        printf("\x1B[36m Initialized matrixes on process №%d; A: \n", rank);
        print_matrix(a);
        printf("B: \n");
        print_matrix(b);
        printf("\033[0m\n");
    }

    int rowsCount = M / size + (M % size != 0);

    int a1[rowsCount][N];
    int b1[rowsCount][N];

    MPI_Scatter(&a, rowsCount * N, MPI_INT, &a1, rowsCount * N, MPI_INT, MAIN_PROCESS, MPI_COMM_WORLD);
    MPI_Scatter(&b, rowsCount * N, MPI_INT, &b1, rowsCount * N, MPI_INT, MAIN_PROCESS, MPI_COMM_WORLD);

    int c1[rowsCount][N];
    for (int i = 0; i < rowsCount && rank * rowsCount + i < M; i++) {
        for (int j = 0; j < N; j++) {
            c1[i][j] = a1[i][j] * b1[i][j];
        }
    }

    MPI_Gather(&c1, rowsCount * N, MPI_INT, &c[rank * rowsCount][0], rowsCount * N, MPI_INT, MAIN_PROCESS, MPI_COMM_WORLD);
    
    if (rank == MAIN_PROCESS) {
        printf("\x1B[33m Got result: \n");
        print_matrix(c);
        printf("\n \033[0m");
    }

    MPI_Finalize();
    return 0;
}
