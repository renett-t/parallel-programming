#include <iostream>
#include <mpi.h>
#include <vector>
#include <cmath>

const int RAND_MAX_VALUE = 10;
const int MATRIX_SIZE = 5;

using namespace std;

void print_matrix(int matrix[MATRIX_SIZE][MATRIX_SIZE]) {
   for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    int rank, size;
    int sender = 0;
    int tag_A = 100;
    int tag_B = 300;
    int tag_C = 400;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    if (rank == sender) {
        srand(time(nullptr));
        int a[MATRIX_SIZE][MATRIX_SIZE];
        int b[MATRIX_SIZE][MATRIX_SIZE];

        for (int i = 0; i < MATRIX_SIZE; i++) {
            for (int j = 0; j < MATRIX_SIZE; j++) {
                a[i][j] = rand() % RAND_MAX_VALUE;
                b[i][j] = rand() % RAND_MAX_VALUE;
            }
        }
        
        printf("Initialized matrixes.\nA:\n");
        print_matrix(a);
        printf("Matrix B:\n");
        print_matrix(b);
    
        int block = MATRIX_SIZE / size + bool(MATRIX_SIZE % size != 0);

        for (int i = 0, dest = 1; i < MATRIX_SIZE; i += block, dest++) {
            MPI_Send(&(b[0][0]), MATRIX_SIZE * MATRIX_SIZE, MPI_INT, dest, tag_B + dest, MPI_COMM_WORLD);
            int count = block < MATRIX_SIZE - i ? block : MATRIX_SIZE - i;
            MPI_Send(&(a[i][0]), count * MATRIX_SIZE, MPI_INT, dest, tag_A + dest, MPI_COMM_WORLD);
        }

        int c[MATRIX_SIZE][MATRIX_SIZE];
        for (int i = 0, dest = 1; i < MATRIX_SIZE; i += block, dest++) {
            int count = block < MATRIX_SIZE - i ? block :  - i;
            MPI_Recv(&(c[i][0]), count * MATRIX_SIZE, MPI_INT, dest, tag_A + dest, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        int BLOCK_SIZE = MATRIX_SIZE / (size - 1) + (MATRIX_SIZE % (size - 1) != 0); // rows of A to send

        int row = 0;
        for (int receiver = 1; receiver < size; receiver++) {
            int elemCount = std::min(BLOCK_SIZE, MATRIX_SIZE - ((receiver - 1) * BLOCK_SIZE));
            while (row < row + elemCount) {
                MPI_Send(&a[row][0], elemCount * MATRIX_SIZE, MPI_INT, receiver, tag_A + receiver, MPI_COMM_WORLD);
                MPI_Send(&b[0][0], MATRIX_SIZE * MATRIX_SIZE, MPI_INT, receiver, tag_B + receiver, MPI_COMM_WORLD);
                row++;
            }
       }

        printf("\nGot result! Matrix C: \n");
        print_matrix(c);
    }
    if (rank != sender) {
        MPI_Status statusA;
        int countA;
        MPI_Probe(sender, tag_A + rank, MPI_COMM_WORLD, &statusA);
        MPI_Get_count(&statusA, MPI_INT, &countA);

        MPI_Status statusB;
        int countB;
        MPI_Probe(sender, tag_B + rank, MPI_COMM_WORLD, &statusB);
        MPI_Get_count(&statusB, MPI_INT, &countB);

        int rowsA = countA / MATRIX_SIZE;
        int a[rowsA][MATRIX_SIZE];
        int b[MATRIX_SIZE][MATRIX_SIZE];
        MPI_Recv(&a[0][0], countA, MPI_INT, sender, statusA.MPI_TAG, MPI_COMM_WORLD, &statusA);
        MPI_Recv(&b[0][0], countB, MPI_INT, sender, statusB.MPI_TAG, MPI_COMM_WORLD, &statusB);

        int c[rowsA][MATRIX_SIZE];
        for (int i = 0; i < rowsA; i++) {
            for (int j = 0; j < MATRIX_SIZE; j++) {
                c[i][j] = 0;
                for (int k = 0; k < MATRIX_SIZE; k++) {
                    c[i][j] += a[i][k] * b[k][j];
                }
            }
        }

        MPI_Send(&c[0][0], countA, MPI_INT, sender, tag_A + rank, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
