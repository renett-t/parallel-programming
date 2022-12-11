#include <mpi.h>
#include <iostream>
#include <random>

const int N = 8;
const int RAND_MAX_VALUE = 100;
const int MAIN_PROCESS = 0;
const int TAG = 42;

int main(int argc, char** argv) {

    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == MAIN_PROCESS) {
        MPI_Datatype type;
        MPI_Datatype row;

        MPI_Type_contiguous(8, MPI_INT, &row);
        MPI_Type_commit(&row);
        MPI_Type_vector(4, 1, 2, row, &type);
        MPI_Type_commit(&type);

        int a[N][N];
        printf("\x1B[36m Matrix A: %d\n", rank);
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                a[i][j] = rand() % RAND_MAX_VALUE;
                printf("%d ", a[i][j]);
            }
            printf("\n");
        }
        printf("\033[0m\n");

        MPI_Send(&(a[0][0]), 1, type, 1, TAG, MPI_COMM_WORLD);
        MPI_Send(&(a[1][0]), 1, type, 2, TAG, MPI_COMM_WORLD);
    }
    else if (rank % 2 == 1) {

        int b[4][8];
        MPI_Recv(&(b[0][0]), 4 * 8, MPI_INT, MAIN_PROCESS, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("\x1B[33m Matrix B[4][8]: %d\n", rank);
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 8; ++j) {
                printf("%d ", b[i][j]);
            }
            printf("\n");
        }
        printf("\033[0m\n");
    }
    else {
        int c[4][8];
        MPI_Recv(&(c[0][0]), 4 * 8, MPI_INT, MAIN_PROCESS, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("\x1B[32m Matrix C[4][8]: %d\n", rank);
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 8; ++j) {
                printf("%d ", c[i][j]);
            }
            printf("\n");
        }
        printf("\033[0m\n");
    }

    MPI_Finalize();

    return 0;
}