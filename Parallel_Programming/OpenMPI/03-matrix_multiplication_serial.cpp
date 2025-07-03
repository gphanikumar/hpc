#include <iostream>
#include <vector>
#include <chrono>

int main(){
    int N{1024};

    // Initialize matrices A, B, and C
    int *A, *B, *C;
    A = new int[N * N];
    B = new int[N * N];
    C = new int[N * N];
    
    for(int i = 0; i < N; ++i) {
        for(int j = 0; j < N; ++j) {
            A[i * N + j] = 1;
            B[i * N + j] = 2;
            C[i * N + j] = 0;
        }
    }

    // Start the timer
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    // Perform matrix multiplication
    for(int i = 0; i < N; ++i) {
        for(int j = 0; j < N; ++j) {
            for(int k = 0; k < N; ++k) {
                C[i * N + j] += A[i * N + k] * B[k * N + j];
            }
        }
    }

    // Stop the timer
    std::chrono::high_resolution_clock::time_point stop = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(stop - start);

    // Print the result of the multiplication
    std::cout << "Result of matrix multiplication (first 5x5 block):" << std::endl;
    for(int i = 0; i < 5; ++i) {
        for(int j = 0; j < 5; ++j) {
            std::cout << C[i * N + j] << " ";
        }
        std::cout << std::endl;
    }

    std::cout << "Time taken for matrix multiplication: " << duration.count() << " seconds" << std::endl;
    
    delete[] A;
    delete[] B;
    delete[] C;
    
    return 0;
}