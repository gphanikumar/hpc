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
    double Dx = 0.4;
    double Dy = 0.4;
    double R = 10.0; // Radius of the solid seed 
    double Dt = 0.001; 

    int Numsteps;
    int SaveT = 2000;
    int NoiseT = 50;

    // Phase field parameters
    double epsilon = 0.28;
    double Lamda = 3.19;
    double U = -0.55; //Scaled temperature or undercooling
    double D = 0.6267 * Lamda;
    //double D = 5.0;

    sycl::queue  q{sycl::cpu_selector_v};
    std::cout<<"selected device : "<<q.get_device().get_info<sycl::info::device::name>()<<"\n";

    fields current, previous;
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    
    Initialize(q, argc, argv, &current, &previous, &Numsteps, Dx, Dy, R, U);

    fields *A = &current, *B = &previous;

    for (int i = 0; i < Numsteps; ++i) {
        solve_eq(q, B, A, epsilon, Lamda, U, Dt, D);
        std::swap(A, B);

        if (i % SaveT == 0) {
            saveAsVTK(B, 0, "Output_" + std::to_string(i) + ".vtk");
        }

        if(i%NoiseT == 0){
            Add_noise(q, B, &Dx, &Dy);
        }
    }

    saveAsVTK(&current, 0, "Output_" + std::to_string(Numsteps) + ".vtk");

    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<float> Time_duration = end -start;

    std::cout<<"Time required to run the simulation : "<<Time_duration.count()<<"\n";

}