#include <chrono>
#include <cstdio>
#include <vector>
#include <chrono>
#include <cstdlib>

#include <sycl/sycl.hpp>

#include "heat.h"
#include "utilities.h"
#include "setup.h"
#include "io.h"
#include "solve.h"

int main(int argc, char* argv[]){
    
    // outsaving intervel
    int time_intervel = 500;

    int numTimesteps;

    field current, previous;
    initialize(argc, argv, &current, &previous, &numTimesteps);

    saveAsVTK(&current, 0, "Output_0.vtk");

    // Diffusiom constant
    double a = 0.5;

    double dx2 = current.dx * current.dx;
    double dy2 = current.dy * current.dy;

    // calculate the highest stable timestep
    double dt = dx2 * dy2 / (2.0 * a * (dx2 + dy2));

    using wall_clock_t = std::chrono::high_resolution_clock;

    // device selector queue
    sycl::queue q{sycl::cpu_selector_v};

    std::cout<<"Selected device : "<<q.get_device().get_info<sycl::info::device::name>()<<"\n";

    auto start = wall_clock_t::now();

    for(int i=0; i<numTimesteps; ++i){
        if(i%2 == 0){
            solve(q, &current, &previous, a, dt);
        }else{
            solve(q, &previous, &current, a, dt);
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