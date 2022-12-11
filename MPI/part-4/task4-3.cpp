#include <iostream>
#include <mpi.h>
#include <chrono>

const int N = 1000;
const int RAND_MAX_VALUE = 100;
const int MAIN_PROCESS = 0;
const int CLIENT = 1;
const int TAG = 4;
const int TAG_CLIENT = 43;

void ping_pong_bi(int rank);
void ping_pong_uni(int rank);

int main(int argc, char** argv) {

    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  //  ping_pong_bi(rank);
    ping_pong_uni(rank);

    MPI_Finalize();
    return 0;
}

void ping_pong_bi(int rank) {
    static int* source = new int[N];


    if (rank == MAIN_PROCESS) {

        for (int i = 0; i < N; i++) {
            source[i] = rand() % RAND_MAX_VALUE;
        }

        double start = MPI_Wtime();

        MPI_Send(source, N, MPI_INT, CLIENT, TAG, MPI_COMM_WORLD);
        MPI_Recv(source, N, MPI_INT, CLIENT, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        double end = MPI_Wtime();

        double time = end - start;
        printf("Ping-pong bi-directional.\nTime passed: %f seconds\n", time);
    }
    else {
        int* received = new int[N];

        MPI_Recv(received, N, MPI_INT, MAIN_PROCESS, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(received, N, MPI_INT, MAIN_PROCESS, TAG, MPI_COMM_WORLD);
    }
}


void ping_pong_uni(int rank) {
    double start;

    MPI_Barrier(MPI_COMM_WORLD);
    
    if (rank == MAIN_PROCESS) {
        static int* source = new int[N];

        for (int i = 0; i < N; i++) {
            source[i] = rand() % RAND_MAX_VALUE;
        }

        start = MPI_Wtime();

        printf("\x1B[36m Sending data from process № %d \033[0m\n", rank);
        MPI_Send(source, N, MPI_INT, CLIENT, TAG_CLIENT, MPI_COMM_WORLD);
    }
    else {
        static int* source = new int[N];

        for (int i = 0; i < N; i++) {
            source[i] = rand() % RAND_MAX_VALUE;
        }

        start = MPI_Wtime();

        printf("\x1B[32m Sending data from process № %d \033[0m\n", rank);
        MPI_Send(source, N, MPI_INT, MAIN_PROCESS, TAG, MPI_COMM_WORLD);
    }

    printf("Processes wait for messages to receive. \n");
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == MAIN_PROCESS) {
        static int* received = new int[N];

        printf("\x1B[36m Receiving data on process № %d \033[0m\n", rank);
        MPI_Recv(received, N, MPI_INT, CLIENT, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    else {
        static int* received = new int[N];

        printf("\x1B[32m Receiving data on process № %d \033[0m\n", rank);
        MPI_Recv(received, N, MPI_INT, MAIN_PROCESS, TAG_CLIENT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    double end = MPI_Wtime();

    double time = end - start;
    printf("Ping-pong uni-directional.\nTime passed: %f seconds\n", time);
}
