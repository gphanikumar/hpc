#include <iostream>
#include <mpi.h>

int main(int argc, char* argv[]){

    int my_id, num_proc;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_proc);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_id);

    int* sendBuf = new int;

    if(my_id==0){
        *sendBuf = 1;
    }

    MPI_Bcast(sendBuf, 1, MPI_INT, 0, MPI_COMM_WORLD);

    std::cout<<"from proc "<<my_id<<" , "<<*sendBuf<<"\n";

    delete sendBuf;

    MPI_Finalize();
    return 0;
}