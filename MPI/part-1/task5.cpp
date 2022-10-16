#include <iostream>
#include <mpi.h>

const int RAND_MAX_VALUE = 10;
const int ARRAY_SIZE = 9;
const int BLOCK_SIZE = 3;
const int A = 2;
const int B = 3;

int main(int argc, char *argv[]) {
    int rank, size;
    int tag = 1;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    int sender = 0;
    if (rank == sender) {
        srand(time(NULL));
        int x[ARRAY_SIZE];
        int y[ARRAY_SIZE];

        for (int i = 0; i < ARRAY_SIZE; i++) {
            x[i] = rand() % RAND_MAX_VALUE;
            y[i] = rand() % RAND_MAX_VALUE;
        }
        printf("Initialized vectors x and y.\n X: ");
        for (int i = 0; i < ARRAY_SIZE; i++) {
            printf("%d ", x[i]);
        }
        printf("\n Y: ");
        for (int i = 0; i < ARRAY_SIZE; i++) {
            printf("%d ", y[i]);
        }
        printf("\n\n");

        int receiver = 1;
        for (int k = 0; k < ARRAY_SIZE; k += BLOCK_SIZE) {
            int block_size = std::min(BLOCK_SIZE, ARRAY_SIZE - k);
            printf("    sending %d elements (from %d to %d) to process №%d\n", block_size, k, k + block_size - 1, receiver);
            MPI_Send(&x[k], block_size, MPI_INT, receiver, tag, MPI_COMM_WORLD);
            MPI_Send(&x[k], block_size, MPI_INT, receiver, tag, MPI_COMM_WORLD);
            receiver++;
        }
        printf("Process %d sent all parts of vectors x and y. Waiting for results from another processes!.\n", sender);

        int z[ARRAY_SIZE];
        receiver = 1;
        for (int k = 0; k < size; k+= BLOCK_SIZE) {
            MPI_Status status;
            MPI_Probe(receiver, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            int count;
            MPI_Get_count(&status, MPI_INT, &count);
            MPI_Recv(&z[k], count, MPI_INT, sender, status.MPI_TAG, MPI_COMM_WORLD, &status);
        }

        printf("\n Got result! Z: ");
        for (int i = 0; i < ARRAY_SIZE; i++) {
            printf("%d ", z[i]);
        }
        printf("\n\n");
    }
    if (rank != sender) {
        MPI_Status status;
        MPI_Probe(sender, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        int count;
        MPI_Get_count(&status, MPI_INT, &count);
        int x[count];
        int y[count];
        MPI_Recv(&x, count, MPI_INT, sender, status.MPI_TAG, MPI_COMM_WORLD, &status);
        MPI_Recv(&y, count, MPI_INT, sender, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int z[count];
        for (int i = 0; i < count; i++) {
            z[i] = A * x[i] + B * y[i];
        }
        printf("    Calculated part of z in process №%d. Elements: ", rank);
        for (int i = 0; i < count; i++) {
            printf("%d ", z[i]);
        }
        printf("\n");

        MPI_Send(&y[0], count, MPI_INT, sender, tag, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
