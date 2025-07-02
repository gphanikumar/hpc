#include <chrono>
#include <cstdio>
#include <vector>
#include <chrono>
#include <cstdlib>

#include <sycl/sycl.hpp>

constexpr double DX = 1;
constexpr double DY = 1;
constexpr double R = 10.0;

#include "Phase_field.h"
#include "utilities.h"
#include "setup.h"
#include "io.h"
#include "solve.h"


int main(int argc, char* argv[]){
    
    // outsaving intervel
    int time_intervel = 1000;

    int numTimesteps;

    field current, previous;
    initialize(argc, argv, &current, &previous, &numTimesteps);

    saveAsVTK(&current, 0, "Output_0.vtk");

    // Phase field parameters
    double M = 1.0; //Mobiliry
    double K = 1.0; //Gradient energy coefficienet
    double W = 1.0; //double well 
    
    // Temperature evalution parameters
    double Kt = 1.0;
    double Cv = 1.0;
    double Tm = 1.0;
    double Lf = 1.0;
    
    double dt = 0.01;

    using wall_clock_t = std::chrono::high_resolution_clock;

    // device selector queue
    sycl::queue q{sycl::cpu_selector_v};

    std::cout<<"Selected device : "<<q.get_device().get_info<sycl::info::device::name>()<<"\n";

    auto start = wall_clock_t::now();

    for(int i=0; i<numTimesteps; ++i){
        if(i%2 == 0){
            solve(q, &current, &previous, W, K, M, Tm, Lf, Kt, Cv, dt);
            Apply_boundary_conditions(&current);
        }else{
            solve(q, &previous, &current, W, K, M, Tm, Lf, Kt, Cv, dt);
            Apply_boundary_conditions(&previous);
        }

        if(i%time_intervel == 0){
            saveAsVTK(&current, 0, "Output_"+std::to_string(i)+".vtk");
        }
    }
    
    auto stop = wall_clock_t::now();

    std::chrono::duration<float> elapsed = stop - start;

    std::cout<<"Simulation completed in "<<elapsed.count()<<" seconds"<<"\n";

    saveAsVTK(&current, numTimesteps, "Final_Output.vtk");

    return 0;
}