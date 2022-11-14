#include <iostream>
#include <mpi.h>
#include <vector>
#include <cmath>

const int RAND_MAX_VALUE = 10;
const int MATRIX_SIZE = 5;

using namespace std;

void print_matrix(int matrix[MATRIX_SIZE * MATRIX_SIZE]) {
   for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            printf("%d ", matrix[i * MATRIX_SIZE + j]);
        }
        printf("\n");
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    int rank, size;
    int tag_A = 100;
    int tag_B = 200;
    int tag_C = 300;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    int sender = 0;
    if (rank == sender) {
        srand(time(nullptr));
        int n = MATRIX_SIZE * MATRIX_SIZE;
        int a[n];
        int b[n];

        for (int i = 0; i < n; i++) {
            a[i] = rand() % RAND_MAX_VALUE;
            b[i] = rand() % RAND_MAX_VALUE;
        }

        print_matrix(a);
        print_matrix(b);

        int BLOCK_SIZE = n / (size - 1) + (n % (size - 1) != 0);

        for (int receiver = 1; receiver < size; receiver++) {
            int block_size = std::min(BLOCK_SIZE, n - ((receiver - 1) * BLOCK_SIZE));
            int k = (receiver - 1) * BLOCK_SIZE;
            printf("Sending %d elements (from %d to %d) to process №%d\n", block_size, k, k + block_size - 1, receiver);

            MPI_Send(&a[k], block_size, MPI_INT, receiver, tag_A + receiver, MPI_COMM_WORLD);
            MPI_Send(&b[k], block_size, MPI_INT, receiver, tag_B + receiver, MPI_COMM_WORLD);
        }
        printf("Process %d sent all parts of vectors x and y. Waiting for results from another processes!.\n", sender);

        int c[n];
        int waitingCount = size - 1;
        while(waitingCount > 0) {
            MPI_Status status;
            MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            int count;
            MPI_Get_count(&status, MPI_INT, &count);
            int fromProcess = status.MPI_TAG - tag_C;
            printf("Received answer from %d %d process in main process.\n", status.MPI_SOURCE, fromProcess);
            int received[count];
            MPI_Recv(&received, count, MPI_INT, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);

            int i = 0;
            int zIndex = (fromProcess - 1) * BLOCK_SIZE;
            for (int j = zIndex; j < zIndex + count; j++, i++) {
                c[j] = received[i];
            }
            waitingCount--;
        }

        printf("\nGot result! Matrix C: \n");
        print_matrix(c);
    }
    if (rank != sender) {

        MPI_Status status;
        MPI_Probe(sender, tag_A + rank, MPI_COMM_WORLD, &status);
        int count;
        MPI_Get_count(&status, MPI_INT, &count);

        int a[count];
        int b[count];
        MPI_Recv(&a, count, MPI_INT, sender, status.MPI_TAG, MPI_COMM_WORLD, &status);
        MPI_Recv(&b, count, MPI_INT, sender, tag_B + rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("\x1B[33m    Received elements in process №%d. Elements: A: ", rank);
        for (int i = 0; i < count; i++) {
            printf("%d ", a[i]);
        }
        printf("; B: ");
        for (int i = 0; i < count; i++) {
            printf("%d ", b[i]);
        }
        printf("\033[0m\n");

        int c[count];
        for (int i = 0; i < count; i++) {
            c[i] = a[i] * b[i];
        }
        printf("\x1B[33m    ! Calculated part of C in process №%d. Elements: ", rank);
        for (int i = 0; i < count; i++) {
            printf("%d ", c[i]);
        }
        printf("\033[0m\n");

        MPI_Send(&c[0], count, MPI_INT, sender, tag_C + rank, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
