#include <iostream>
#include <mpi.h>
#include <vector>
#include <cmath>
#include <unordered_map>

const int RAND_MAX_VALUE = 10;
const int ARRAY_SIZE = 10;
const int A = 2;
const int B = 3;
typedef std::vector<int> Data;

void print_map(const std::unordered_map<int, Data>& m)
{
    for (const auto& [key, value] : m)
        std::cout << '[' << key << "] = {" << value[0] << ", " << value[1] << "}; ";
    std::cout << '\n';
}

int main(int argc, char *argv[]) {
    int rank, size;
    int tag_from_main = 40;
    // int tag_from_main_y = 41;
    int tag_answer = 42;

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

        int BLOCK_SIZE = ARRAY_SIZE / (size - 1) + (ARRAY_SIZE % (size - 1) != 0);
        std::unordered_map<int, Data> mp; // карта, которая хранит инф-цию о том, какие индексы обрабатывает i-тый процесс. 
        // key - номер процесса, value  - двумерный массив, у коготого 
        // arr[0] = начальный индекс, обрабатываемый данным процессом, arr[1] = кол-во обрабатываемых данным процессом элементов.
       
        int receiver = 1;
        for (int k = 0; k < ARRAY_SIZE; k += BLOCK_SIZE) {
            int block_size = std::min(BLOCK_SIZE, ARRAY_SIZE - k);
            printf("Sending %d elements (from %d to %d) to process №%d\n", block_size, k, k + block_size - 1, receiver);
            mp[receiver] = {k , block_size};
            MPI_Send(&x[k], block_size, MPI_INT, receiver, tag_from_main, MPI_COMM_WORLD);
            MPI_Send(&y[k], block_size, MPI_INT, receiver, tag_from_main, MPI_COMM_WORLD);
            receiver++;
        }
        printf("Process %d sent all parts of vectors x and y. Waiting for results from another processes!.\n", sender);
        // print_map(mp);

        int z[ARRAY_SIZE];
        int waitingCount = size - 1;
        while(waitingCount > 0) {
            // получить сообщение от других процессов
            MPI_Status status;
            MPI_Probe(MPI_ANY_SOURCE, tag_answer, MPI_COMM_WORLD, &status);
            int count;
            MPI_Get_count(&status, MPI_INT, &count);
            printf("Received answer from %d process in main process.\n", status.MPI_SOURCE);
            int received[count];
            MPI_Recv(&received, count, MPI_INT, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);

            // узнать кто прислал, что прислал и заполнить массив z
            Data data = mp.at(status.MPI_SOURCE);
            int i = 0;
            for (int j = data[0]; j < data[0] + data[1]; j++, i++) {
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
        MPI_Probe(sender, tag_from_main, MPI_COMM_WORLD, &status);
        int count;
        MPI_Get_count(&status, MPI_INT, &count);
        int x[count];
        int y[count];
        MPI_Recv(&x, count, MPI_INT, sender, status.MPI_TAG, MPI_COMM_WORLD, &status);
        MPI_Recv(&y, count, MPI_INT, sender, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

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
            z[i] = A * x[i] + B * y[i];
        }
        printf("\x1B[33m    ! Calculated part of z in process №%d. Elements: ", rank);
        for (int i = 0; i < count; i++) {
            printf("%d ", z[i]);
        }
        printf("\033[0m\n");

        MPI_Send(&z[0], count, MPI_INT, sender, tag_answer, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
