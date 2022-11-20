#include <iostream>
#include <mpi.h>
#include <vector>
#include <cmath>

#include <chrono>
#include <thread>

const int RAND_MAX_VALUE = 100;
const int ROWS_A = 8;
const int COLUMNS_A = 6;
const int ROWS_C = 6;
const int COLUMNS_C = 8;

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
    int c[COLUMNS_A][ROWS_A];       // transponed matrix
    int tag_from = 100;

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

    // printf("\x1B[36m Calculated on %d process: \n", rank);
    // for (int i = 0; i < COLUMNS_A; i++) {
    //     for (int j = 0; j < rows; j++) {
    //         printf("%3d ", c1[i][j]);
    //     }
    //     printf("\n");
    // }
    // printf("\033[0m");
    
    MPI_Send(&c1,  rows * COLUMNS_A, MPI_INT, 0, tag_from + rank, MPI_COMM_WORLD);

    if (rank == 0) {

        for(int k = 0; k < size; k++) {
            MPI_Status status;
            MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            int count;
            MPI_Get_count(&status, MPI_INT, &count);
            
            int receivedCols = count / COLUMNS_A;
            int startColumn = status.MPI_SOURCE * rows;

            int recieved[COLUMNS_A][receivedCols];
            MPI_Recv(&recieved, count, MPI_INT,status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);
            
            // printf("\x1B[33m  Got message from %d process, contains %d elements: \n", status.MPI_SOURCE, count);
            // printf("  Gotta insert in C: from %d column \n", startColumn);
            // for (int i = 0; i < COLUMNS_A; i++) {
            //     for (int j = 0; j < receivedCols; j++) {
            //         printf("%3d ", recieved[i][j]);
            //     }
            //     printf("\n");
            // }
            // printf("\033[0m");

            int p = 0;
            for (int i = 0; i < COLUMNS_A; i++) {
                for (int j = startColumn; j < startColumn + rows; j++) {
                    c[i][j] = recieved[i][p++];
                }
                p = 0;
            }
        }


        printf("Transposed matrix! Result: \n");

        for (int i = 0; i < ROWS_C; i++) {
            for (int j = 0; j < COLUMNS_C; j++) {
                printf("%3d ", c[i][j]);
            }
            printf("\n");
        }
        printf("\n");
    } else {
        // std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    }

    MPI_Finalize();
    return 0;
}
