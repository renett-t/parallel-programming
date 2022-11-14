#include <iostream>
#include <mpi.h>
#include <vector>
#include <cmath>

const int RAND_MAX_VALUE = 10;
const int ARRAY_SIZE = 10;

using namespace std;

int main(int argc, char *argv[]) {
    int rank, size;
    int tag_X = 40;
    int tag_Y = 80;

    int tag_answer = 120;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    int sender = 0;
    if (rank == sender) {
        srand(time(nullptr));
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

        int BLOCK_SIZE = ARRAY_SIZE / (size - 1) + (ARRAY_SIZE % (size - 1) != 0);

        for (int receiver = 1; receiver < size; receiver++) {
            int block_size = std::min(BLOCK_SIZE, ARRAY_SIZE - ((receiver - 1) * BLOCK_SIZE));
            int k = (receiver - 1) * BLOCK_SIZE;
            printf("Sending %d elements (from %d to %d) to process №%d\n", block_size, k, k + block_size - 1, receiver);

            MPI_Send(&x[k], block_size, MPI_INT, receiver, tag_X + receiver, MPI_COMM_WORLD);
            MPI_Send(&y[k], block_size, MPI_INT, receiver, tag_Y + receiver, MPI_COMM_WORLD);
        }
        printf("Process %d sent all parts of vectors x and y. Waiting for results from another processes!.\n", sender);

        int z[ARRAY_SIZE];
        int waitingCount = size - 1;
        while(waitingCount > 0) {
            MPI_Status status;
            MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            int count;
            MPI_Get_count(&status, MPI_INT, &count);
            int fromProcess = status.MPI_TAG - tag_answer;
            printf("Received answer from %d process in main process.\n", status.MPI_SOURCE);
            int received[count];
            MPI_Recv(&received, count, MPI_INT, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);

            int i = 0;
            int zIndex = (fromProcess - 1) * BLOCK_SIZE;
            for (int j = zIndex; j < zIndex + count; j++, i++) {
                z[j] = received[i];
            }
            waitingCount--;
        }

        printf("\nGot result! Z: ");
        for (int i = 0; i < ARRAY_SIZE; i++) {
            printf("%d ", z[i]);
        }
        printf("\n\n");
    }
    if (rank != sender) {

        MPI_Status status;
        MPI_Probe(sender, tag_X + rank, MPI_COMM_WORLD, &status);
        int count;
        MPI_Get_count(&status, MPI_INT, &count);

        int x[count];
        int y[count];
        MPI_Recv(&x, count, MPI_INT, sender, status.MPI_TAG, MPI_COMM_WORLD, &status);
        MPI_Recv(&y, count, MPI_INT, sender, tag_Y + rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("\x1B[33m    Received elements in process №%d. Elements: X: ", rank);
        for (int i = 0; i < count; i++) {
            printf("%d ", x[i]);
        }
        printf("; Y: ");
        for (int i = 0; i < count; i++) {
            printf("%d ", y[i]);
        }
        printf("\033[0m\n");

        int z[count];
        for (int i = 0; i < count; i++) {
            z[i] = x[i] * y[i];
        }
        printf("\x1B[33m    ! Calculated part of z in process №%d. Elements: ", rank);
        for (int i = 0; i < count; i++) {
            printf("%d ", z[i]);
        }
        printf("\033[0m\n");

        MPI_Send(&z[0], count, MPI_INT, sender, tag_answer + rank, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
