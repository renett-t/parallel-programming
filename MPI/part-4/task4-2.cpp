#include <iostream>
#include <mpi.h>

const int N = 12;
const int RAND_MAX_VALUE = 100;

void print_array(int*, int data, int length);

int main(int argc, char** argv) {

    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int TAG = 42;
    int MAIN_PROCESS = 0;

    int block = N / size + 1;
    int main_process_count = N % block;

    int* a = new int[N];
    int* local_a = new int[block];

    if (rank == MAIN_PROCESS) {
        printf("\x1B[36mInitialization. Processes count: %d\033[0m\n", size);

        printf("\x1B[32m array `a` on main process: \n");
        for (int i = 0; i < N; i++) {
            a[i] = rand() % RAND_MAX_VALUE;
            printf("%d ", a[i]);
        }
        printf("\n\033[0m");

        int start = main_process_count;
        for (int i = 1; i < size; i++) {
            MPI_Send(a + start, block, MPI_INT, i, TAG, MPI_COMM_WORLD);
            start += block;
        }

        printf("On process №%d: ", rank);
        print_array(a, 0, main_process_count);
    } 
    else {
        local_a = new int[block];
        MPI_Recv(local_a, block, MPI_INT, MAIN_PROCESS, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("On process №%d: ", rank);
        if (rank < N / block + 1) {
            print_array(local_a, 0, block);
        }
    }

    MPI_Finalize();
    return 0;
}

void print_array(int* a, int start, int length) {
    for (int i = start; i < length; i++) {
        printf("%d ", a[i]);
    }
    printf("\n");
}
