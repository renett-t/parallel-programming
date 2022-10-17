#include <iostream>
#include <mpi.h>
#include <map>
#include <cmath>

const int RAND_MAX_VALUE = 10;
const int ARRAY_SIZE = 9;
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
        int BLOCK_SIZE = std::ceil((rank - 1.0) / (0.0 + ARRAY_SIZE));
        
        std::map<int, int[2]> mp; 
        // карта, которая хранит инф-цию о том, какие индексы обрабатывает i-тый процесс. key - номер процесса, value  - двумерный массив, у коготого arr[0] = начальный индекс, обрабатываемый данным процессом, arr[1] = кол-во обрабатываемых данным процессом элементов.

        for (int k = 0; k < ARRAY_SIZE; k += BLOCK_SIZE) {
            int block_size = std::min(BLOCK_SIZE, ARRAY_SIZE - k);
            printf("    sending %d elements (from %d to %d) to process №%d\n", block_size, k, k + block_size - 1, receiver);
            int data[2] = {k , block_size};
            mp[receiver] = data;
            MPI_Send(&x[k], block_size, MPI_INT, receiver, tag, MPI_COMM_WORLD);
            MPI_Send(&x[k], block_size, MPI_INT, receiver, tag, MPI_COMM_WORLD);
            receiver++;
        }
        printf("Process %d sent all parts of vectors x and y. Waiting for results from another processes!.\n", sender);

        int z[ARRAY_SIZE];
        for (int k = 1; k < size; k++) {
            // получить сообщение от других процессов
            MPI_Status status;
            MPI_Probe(receiver, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            int count;
            MPI_Get_count(&status, MPI_INT, &count);
            int received[count];
            MPI_Recv(&received, count, MPI_INT, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);

            // узнать кто прислал, что прислал и заполнить массив z
            int data[2] = mp[status.MPI_SOURCE];
            for (int j = data[0], i = 0; j < data[1]; j++, i++) {
                z[j] = received[i];
            }
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
