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
    MPI_Reduce(&la, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    
    std::cout<<"random number from process id : "<<my_id<<", is "<<la<<std::endl;

    // Ensure all processes have completed before printing the result
    MPI_Barrier(MPI_COMM_WORLD);

    // Only process 0 will print the total sum
    if(my_id == 0) std::cout<<"The total sum is : "<<sum<<std::endl;

    MPI_Finalize();
    return 0;
}