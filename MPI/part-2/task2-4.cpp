#include <iostream>
#include <mpi.h>
#include <vector>
#include <cmath>

const int RAND_MAX_VALUE = 10;
const int ROWS_A = 4;
const int COLUMNS_A = 4;
const int COLUMNS_B = 4;
const int METADATA_SIZE = 2;

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
void print_matrix_B(int matrix[COLUMNS_A][COLUMNS_B]) {
   for (int i = 0; i < COLUMNS_A; i++) {
        for (int j = 0; j < COLUMNS_B; j++) {
            printf("%3d ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    int rank, size;
    int tag_META = 50;
    int tag_A = 200;
    int tag_B = 300;
    int tag_C = 400;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        
    int a[ROWS_A][COLUMNS_A];
    int b[COLUMNS_A][COLUMNS_B];
    int c[ROWS_A][COLUMNS_B];

    int MAIN_PROCESS = 0;

    if (rank == MAIN_PROCESS) {
        srand(time(nullptr));
        
        for (int i = 0; i < ROWS_A; i++) {
            for (int j = 0; j < COLUMNS_A; j++) {
                a[i][j] = rand() % RAND_MAX_VALUE;
            }
        }
        for (int i = 0; i < COLUMNS_A; i++) {
            for (int j = 0; j < COLUMNS_B; j++) {
                b[i][j] = rand() % RAND_MAX_VALUE;
            }
        }
        
        printf("Initialized matrixes.\nA:\n");
        print_matrix_A(a);
        printf("Matrix B:\n");
        print_matrix_B(b);

        double start = MPI_Wtime();

        int workers = size - 1;
        int rows_block = ROWS_A / workers + (ROWS_A % workers != 0);

        for (int dest = 1; dest < size; dest++) {
            MPI_Send(&(b[0][0]), COLUMNS_A * COLUMNS_B, MPI_INT, dest, tag_B + dest, MPI_COMM_WORLD);

            int startRow = (dest - 1) * rows_block;
            int rowsCount = std::min(rows_block, ROWS_A - ((dest - 1) * rows_block));
            int data[METADATA_SIZE] = {startRow, rowsCount};
            
            // printf("Sending %d rows to process №%d, offset = %d\n", rowsCount, dest, startRow);
            MPI_Send(&(a[startRow][0]), rowsCount * COLUMNS_A, MPI_INT, dest, tag_A + dest, MPI_COMM_WORLD);
        }

        printf("Sent all data, waiting for results~ \n");
        for (int from = 1; from < size; from++) {
            
            int startRow = (from - 1) * rows_block;
            int rowsCount = std::min(rows_block, ROWS_A - ((from - 1) * rows_block));
            // printf("  Received %d rows from process №%d, offset = %d\n", rowsCount, from, startRow);

            MPI_Recv(&(c[startRow][0]), rowsCount * COLUMNS_B, MPI_INT,from, tag_C + from, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        printf("\nGot final result ! Matrix C: \n");
        for (int i = 0; i < ROWS_A; i++) {
            printf("\n"); 
            for (int j = 0; j < COLUMNS_B; j++) 
            // printf("%6.2f   ", c[i][j]);
            printf("%3d   ", c[i][j]);
        }
        printf("\n");
    } else {
        MPI_Recv(&b, COLUMNS_A * COLUMNS_B, MPI_INT, MAIN_PROCESS, tag_B + rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // printf(" received B in process %d\n", rank);

        MPI_Status status;
        MPI_Probe(MAIN_PROCESS, tag_A + rank, MPI_COMM_WORLD, &status);
        int count;
        MPI_Get_count(&status, MPI_INT, &count);
        int rowsCount = count / COLUMNS_A;

        MPI_Recv(&a, rowsCount * COLUMNS_A, MPI_INT, MAIN_PROCESS, tag_A + rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // printf(" received A in process %d\n", rank);

        for (int i = 0; i < rowsCount; i++)
            for (int j = 0; j < COLUMNS_B; j++) {
                c[i][j] = 0;
                for (int k = 0; k < COLUMNS_A; k++) {
                    c[i][j] += a[i][k] * b[k][j];
                }
        }

        MPI_Send(&c[0][0], rowsCount * COLUMNS_B, MPI_INT, MAIN_PROCESS, tag_C + rank, MPI_COMM_WORLD);
                // printf(" sent result from process %d\n", rank);

    }

    MPI_Finalize();
    return 0;
}
