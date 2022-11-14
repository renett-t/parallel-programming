#include <iostream>
#include <mpi.h>
#include <vector>
#include <cmath>

const int RAND_MAX_VALUE = 100;
const int ROWS_A = 4 ;
const int COLUMNS_A = 4;
const int ROWS_C = 4;
const int COLUMNS_C = 4;

using namespace std;

void print_matrix_A(int matrix[ROWS_A][COLUMNS_A]) {
   for (int i = 0; i < ROWS_A; i++) {
        for (int j = 0; j < COLUMNS_A; j++) {
            printf("%3d ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}
void print_matrix_B(int matrix[ROWS_C][COLUMNS_C]) {
   for (int i = 0; i < ROWS_C; i++) {
        for (int j = 0; j < COLUMNS_C; j++) {
            printf("%3d ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void print_matrix(int **m, int row, int col) {
    int i, j = 0;
    for (i=0;i<row;i++) {
        for (j=0;j<col;j++) {
            printf("%d", m[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    int a[ROWS_A][COLUMNS_A];       // initial matrix
    int c[ROWS_C][COLUMNS_C];       // transponed matrix

    int MAIN_PROCESS = 0;

    if (rank == MAIN_PROCESS) {
        srand(time(nullptr));
        
        for (int i = 0; i < ROWS_A; i++) {
            for (int j = 0; j < COLUMNS_A; j++) {
                a[i][j] = rand() % RAND_MAX_VALUE;
            }
        }
        
        printf("Initialized matrix on Process №%d\n", rank);
        print_matrix_A(a);
    }

    int rows = ROWS_A / size;
    int a1[rows][COLUMNS_A];
    int c1[COLUMNS_A][rows];
    int startRow = rank * rows;

    MPI_Scatter(&a[startRow][0], rows * COLUMNS_A, MPI_INT, &a1, rows * COLUMNS_A, MPI_INT, MAIN_PROCESS, MPI_COMM_WORLD);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < COLUMNS_A; j++) {
            c1[j][i] = a1[i][j];
        }
    }

    MPI_Gather(&c1, rows * COLUMNS_A, MPI_INT, &c, rows * COLUMNS_A, MPI_INT, MAIN_PROCESS, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Transposed matrix! Result: \n");

        for (int i = 0; i < ROWS_C; i++) {
            for (int j = 0; j < COLUMNS_C; j++) {
                printf("%3d ", c[i][j]);
            }
            printf("\n");
        }
        printf("\n");
    }

    MPI_Finalize();
    return 0;
}
