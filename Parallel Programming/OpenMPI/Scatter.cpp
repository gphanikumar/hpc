#include <iostream>
#include <mpi.h>

int main(int argc, char* argv[]){
    int my_id, num_proc;

    int *send_buf, *receive_buf;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&my_id);
    MPI_Comm_size(MPI_COMM_WORLD,&num_proc);

    if(my_id==0){
        send_buf = new int[10*num_proc];
        for(int i=0; i<10*num_proc; i++)send_buf[i]=100+5*i+i;
    }

    receive_buf = new int[10];
    MPI_Scatter(send_buf, 10, MPI_INT, receive_buf, 10, MPI_INT, 0, MPI_COMM_WORLD);

    if(my_id==2){
        for(int i=0; i<10; i++)std::cout<<receive_buf[i]<<"\t";
        std::cout<<std::endl;
    }

    if(my_id==0){delete[] send_buf;}
    delete[] receive_buf;
    MPI_Finalize();
    return 0;
}