#include <iostream>
#include <math.h>
#include <chrono>
#include <mpi.h>

constexpr double PI = 3.14159;

double function(double x){
    return 1.0+sin(x);
}

double trapezoidal_rule(double la, double lb, double h, double ln){
    double total{(function(la)+function(lb))*0.5};
    for(int i=1; i<ln; i++)total+=function(la+i*h);
    return total*h;
}

int main(int argc, char* argv[]){

    int my_id, num_procs;

    MPI_Init(NULL,NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    int n{10240};
    double a{0}, b{PI};
    double h{(b-a)/n};

    int ln{n/num_procs}; //local n
    double la{a+my_id*ln*h};
    double lb{la+h*ln};

    MPI_Barrier(MPI_COMM_WORLD);
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    double lsum = trapezoidal_rule(la,lb,h,ln);
    double final_result{0};

    // if(my_id !=0){
    //     MPI_Send(&lsum, 1, MPI_DOUBLE, 0, 1001, MPI_COMM_WORLD);
    // }else{
    //     for(int i=1; i<num_procs; i++){
    //         MPI_Recv(&lsum, 1, MPI_DOUBLE, i, 1001, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //         final_result +=lsum;
    //     }
    // }

    MPI_Reduce(&lsum, &final_result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    std::chrono::high_resolution_clock::time_point stop = std::chrono::high_resolution_clock::now();
    
    if(my_id == 0){
        std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(stop - start);   
        std::cout<<"Final result: "<<final_result<<", Analytical result :"<<(b-cos(b))-(a-cos(a))<<"\n";
        std::cout<<"Time taken for numerical integration : "<<duration.count()<<" seconds\n";
    }
    MPI_Finalize();
    return 0;
}