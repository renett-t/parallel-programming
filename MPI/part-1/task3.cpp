#include <iostream>
#include <mpi.h>

const int RAND_MAX_VALUE = 1000;
const int ARRAY_SIZE = 10;

int main(int argc, char *argv[]) {
    int rank, size;
    int tag = 1;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
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
        MPI_Send(&a, ARRAY_SIZE, MPI_INT, 1, tag, MPI_COMM_WORLD);
    }
    if (rank == 1) {
        int buf[ARRAY_SIZE];
        MPI_Status status;
        MPI_Recv(&buf, ARRAY_SIZE, MPI_INT, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);

        printf("! Received message in %d process from %d process. Contents: \n", rank, status.MPI_SOURCE);
        for (int i = 0; i < ARRAY_SIZE; i++) {
            printf("%d ", buf[i]);
        }
        printf("\n");
    }

    MPI_Finalize();
    return 0;
}