#include <mpi.h>
#include <iostream>
#include <random>

const int N = 15;
const int RAND_MAX_VALUE = 100;
const int MAIN_PROCESS = 0;
const int TAG = 42;
const int GROUP_PR_COUNT = 5;

int main(int argc, char** argv)
{
	MPI_Comm comm;

	int rank, size;
	int a[N];

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int* ranks = new int[]{1, 3, 6, 8, 9};

	MPI_Group wgroup, group1, group2;
	MPI_Comm_group(MPI_COMM_WORLD, &wgroup);
	MPI_Group_incl(wgroup, GROUP_PR_COUNT, ranks, &group1);
	MPI_Group_excl(wgroup, GROUP_PR_COUNT, ranks, &group2);

	MPI_Comm comm1, comm2;
	MPI_Comm_create(MPI_COMM_WORLD, group1, &comm1);
	MPI_Comm_create(MPI_COMM_WORLD, group2, &comm2);

    int rank1;
    int rank2;
	if (comm1 != MPI_COMM_NULL) {
		MPI_Comm_rank(comm1, &rank1);
	}
	if (comm2 != MPI_COMM_NULL) {
		MPI_Comm_rank(comm2, &rank2);
	}
    printf("rank = %d, rank1 = %d, rank2 = %d \n", rank, rank1, rank2);

    MPI_Barrier(MPI_COMM_WORLD);
	if (comm1 != MPI_COMM_NULL && rank1 == MAIN_PROCESS) {

		printf("\x1B[36m Initialization of matrix A on process № %d (old rank = %d). Total processes: %d\033[0m\n", rank1, rank, size);
		for (int i = 0; i < N; i++) {
			a[i] = rand() % RAND_MAX_VALUE;
			printf("\x1B[36m%d \033[0m", a[i]);
		}
		printf("\n");
	}

	if (comm1 != MPI_COMM_NULL) {
		MPI_Bcast(&a[0], N, MPI_INT, MAIN_PROCESS, comm1);

        printf("\x1B[32m On process № %d (old rank = %d) \n\033[0m", rank1, rank);
		for (int i = 0; i < N; i++) {
			printf("\x1B[32m%d \033[0m", a[i]);
		}
		printf("\n");
	}

	if (comm1 != MPI_COMM_NULL && rank1 == GROUP_PR_COUNT - 1) {
		printf("Sending data from %d (old %d) process to 0 initial process\n", rank1, rank);
		MPI_Send(&a[0], N, MPI_INT, MAIN_PROCESS, TAG, MPI_COMM_WORLD);
	}

	if (rank == MAIN_PROCESS) {
		int b[N];
		MPI_Status status;
		MPI_Recv(&b[0], N, MPI_INT, MPI_ANY_SOURCE, TAG, MPI_COMM_WORLD, &status);

		printf("\x1B[33m Received array from %d process on process № %d\033[0m\n", status.MPI_SOURCE, rank);
		for (int i = 0; i < N; i++) {
			printf("\x1B[33m%d \033[0m", b[i]);
		}
        printf("\n");
	}

	if (comm1 != MPI_COMM_NULL) {
		MPI_Comm_free(&comm1);
	}
	if (comm2 != MPI_COMM_NULL) {
		MPI_Comm_free(&comm2);
	}

	MPI_Group_free(&group1);
	MPI_Group_free(&group2);

	MPI_Finalize();
    return 0;
}