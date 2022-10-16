#include <iostream>
#include <mpi.h>

const int RAND_MAX_VALUE = 100;
const int ARRAY_SIZE = 5;

int main(int argc, char *argv[]) {
    int rank, size;
    int tag = 1;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    int sender = 1;
    if (rank == sender) {
        srand(time(NULL));
        int a[ARRAY_SIZE];
        for (int i = 0; i < ARRAY_SIZE; i++) {
            a[i] = rand() % RAND_MAX_VALUE;
        }
        printf("Sending message from %d process. Message contains array: ", rank);
        for (int i = 0; i < ARRAY_SIZE; i++) {
            printf("%d ", a[i]);
        }
        printf("\n\n");
        for (int k = 0; k < size; k++) {
            if (k != sender) {
                MPI_Send(&a, ARRAY_SIZE, MPI_INT, k, tag, MPI_COMM_WORLD);
            }
        }
    }
    if (rank != sender) {
        MPI_Status status;
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        int count;
        MPI_Get_count(&status, MPI_INT, &count);
        int buf[count];
        MPI_Recv(&buf, count, MPI_INT, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);

        printf("! Received message in %d process from %d process. Contents: ", rank, status.MPI_SOURCE);
        for (int i = 0; i < count; i++) {
            printf("%d ", buf[i]);
        }
        printf("\n");
    }

    MPI_Finalize();
    return 0;
}
