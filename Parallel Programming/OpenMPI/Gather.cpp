#include <iostream>
#include <mpi.h>

int main(int argc, char* argv[]){
    int my_id, num_procs;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_id);

    int *sendBuf, *receiveBuf;
    
    sendBuf = new int[2];
    for(int i=0;i<2; i++)sendBuf[i] = 1+my_id*(i+1);

    if(my_id==0){receiveBuf = new int[num_procs*2];}
    MPI_Gather(sendBuf, 2, MPI_INT, receiveBuf, 2, MPI_INT, 0, MPI_COMM_WORLD);

    if(my_id==0){
        for(int i=0; i<num_procs*2; i++)std::cout<<receiveBuf[i]<<"\t";
        std::cout<<std::endl;
        delete[] receiveBuf;
    }

    delete[] sendBuf;
    MPI_Finalize();
    return 0;
}