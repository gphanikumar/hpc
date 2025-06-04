#include <bits/stdc++.h>
#include <mpi.h>

int main(int argc, char* argv[]) {
    int myid, size;
    int tag = 1001;
    
    std::string msg_send, msg_recv;
    MPI_Init(nullptr, nullptr);
    MPI_Status status;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);


    if (myid!=0) {
        msg_send = "Hello from process id: " + std::to_string(myid);
        MPI_Send(msg_send.c_str(), msg_send.size()+1, MPI_CHAR, 0, tag, MPI_COMM_WORLD);
    }
    else {
        for (int i =1 ; i < size ; i++) {
            std::vector<char> buffer(100);
            MPI_Recv(buffer.data(), buffer.size(), MPI_CHAR, i, tag, MPI_COMM_WORLD, &status);
            std::string msg_recv(buffer.data());
            std::cout << msg_recv << std::endl;
        }
    }

    MPI_Finalize();

    return 0;
}