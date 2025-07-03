#include <iostream>
#include <math.h>
#include <mpi.h>

int main(int argc, char* argv[]){
    int my_id, num_procs;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    // Generate unique random number using seed based on process id
    // and current time to ensure different random numbers across processes
    // Each process generates a random number between 0 and 9
    srand(time(NULL)+my_id);
    int la = rand()%10;
    int sum;

    // Reduce operation to sum all random numbers
    // from all processes and store the result in process 0
    MPI_Allreduce(&la, &sum, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    // Print the random number generated and the total sum by each process
    // Note: All processes will print their own random number and the total sum`
    std::cout<<"Process id : "<<my_id<<", random number "<<la<<", total sum "<<sum<<std::endl;

    MPI_Finalize();
    return 0;
}