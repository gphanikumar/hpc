#include <chrono>
#include <cstdio>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <unistd.h>

#include <sycl/sycl.hpp>

#include "structures.h"
#include "Kernel_functions.h"
#include "io.h"

int main(int argc, char* argv[]){

    // Domain parameters
    double Dx = 1.0;
    double Dy = 1.0;
    double Dz = 1.0;

    double dt = 0.01;
    int Numsteps;
    int SaveT = 1000;

    // Phase field parameters
    double M = 1.0; // Mobility
    double K = 1.0; // Gradient energy coefficient
    double W = 1.0; // double well height

    sycl::queue  q{sycl::cpu_selector_v};
    std::cout<<"selected device : "<<q.get_device().get_info<sycl::info::device::name>()<<"\n";

    fields current, previous;
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    Initialize(q, argc, argv, &current, &previous, &Numsteps, Dx, Dy, Dz);

    for(int i=0; i<Numsteps; ++i){
        if(i%2 == 0){
            solve_eq(q, &previous, &current, W, K, M, dt);
        }else{
            solve_eq(q, &current, &previous, W, K, M, dt);
        }

        if(i%SaveT == 0){
            saveAsVTK(&current, 0, "Output_"+std::to_string(i)+".vtk");
        }
    }

    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<float> Time_duration = end -start;

    std::cout<<"Time required to run the simulation : "<<Time_duration.count()<<"\n";

}
