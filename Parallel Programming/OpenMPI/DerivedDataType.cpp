#include <iostream>
#include <math.h>
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

//creating a custom MPI_datatype to send all information in single data transfer using the PCI
void CreatenewDataType(double* a_p, double* b_p, int* n_p, MPI_Datatype* newMPI_Datatype){
    int Blocklength[3]{1,1,1};
    MPI_Aint Displacement[3]{0,0,0};
    
    MPI_Get_address(a_p, &Displacement[0]);
    MPI_Get_address(b_p, &Displacement[1]);
    MPI_Get_address(n_p, &Displacement[2]);

    MPI_Aint baseDisplacement = Displacement[0];

    for(int i=0; i<3; i++)Displacement[i]-=baseDisplacement;

    MPI_Datatype Type[3]{MPI_DOUBLE, MPI_DOUBLE, MPI_INT};

    MPI_Type_create_struct(3, Blocklength, Displacement, Type, newMPI_Datatype);
    MPI_Type_commit(newMPI_Datatype);
};

int main(int argc, char* argv[]){

    int my_id, num_procs;

    MPI_Init(NULL,NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    double a, b;
    int n;

    MPI_Datatype MPI_Trapezoidal;
    CreatenewDataType(&a, &b, &n, &MPI_Trapezoidal);

    if(my_id == 0){n = 1024, a=0.0, b=PI;}
    
    MPI_Bcast(&a, 1, MPI_Trapezoidal, 0, MPI_COMM_WORLD);
    MPI_Type_free(&MPI_Trapezoidal);

    double h{(b-a)/n};

    int ln{n/num_procs}; //local n
    double la{a+my_id*ln*h};
    double lb{la+h*ln};

    double lsum = trapezoidal_rule(la,lb,h,ln);
    double final_result{0.0};

    MPI_Reduce(&lsum, &final_result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if(my_id == 0){
        std::cout<<"Final result: "<<final_result<<", Analytical result :"<<(b-cos(b))-(a-cos(a))<<"\n";
    }
    MPI_Finalize();
    return 0;
}