#include <iostream>
#include <vector>
#include <chrono>
#include <mpi.h>

int main(int argc, char* argv[]) {
    
    int my_id, num_procs;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    int N{1021};

    int lrows = N / num_procs;

    int *A, *C;
    int* B = new int[N * N];

    int* A_local = new int[lrows * N];
    int* C_local = new int[lrows * N];

    // Rank 0 initializes A and B
    if (my_id == 0) {
        A = new int[N * N];
        C = new int[N * N];
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j) {
                A[i * N + j] = 1;
                B[i * N + j] = 2;
            }
    }

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    MPI_Scatter(A, lrows * N, MPI_INT, A_local, lrows * N, MPI_INT, 0, MPI_COMM_WORLD);


    MPI_Bcast(B, N * N, MPI_INT, 0, MPI_COMM_WORLD);

    std::cout << "Process " << my_id << " has local rows: " << lrows << std::endl;
    
    // Matrix multiplication
    for (int i = 0; i < lrows; ++i)
        for (int j = 0; j < N; ++j)
            for (int k = 0; k < N; ++k)
                C_local[i * N + j] += A_local[i * N + k] * B[k * N + j];

    if (my_id == 0) {
        std::vector<int> C(N * N);
    }
    
    MPI_Gather(C_local, lrows * N, MPI_INT, C, lrows * N, MPI_INT, 0, MPI_COMM_WORLD);

    std::chrono::high_resolution_clock::time_point stop = std::chrono::high_resolution_clock::now();

    if (my_id == 0) {
        std::cout << "Result of matrix multiplication (first 5x5 block):" << std::endl;
        for (int i = 0; i < 5; ++i) {
            for (int j = 0; j < 5; ++j) {
                std::cout << C[i * N + j] << " ";
            }
            std::cout << std::endl;
        }
        std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(stop - start);
        std::cout << "Time taken for matrix multiplication: " << duration.count() << " seconds" << std::endl;   

    }

    delete[] B;
    delete[] A_local;
    delete[] C_local;
    if (my_id == 0) {
        delete[] A;
        delete[] C;
    }

    MPI_Finalize();
    return 0;
}
