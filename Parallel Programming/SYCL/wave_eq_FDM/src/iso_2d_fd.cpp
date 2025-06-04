#include <iostream>
#include <sycl/sycl.hpp>
#include "../include/saveVTK.hpp"
#include "dpc_common.hpp"

constexpr float DT{0.002f}, DXY{20.0f};
constexpr unsigned int half_length{1};

void Initialize(float* ptr_previous, float* ptr_next, float* ptr_velocity, size_t nrows, size_t ncols){
    std::cout<<"Initializing the array ...\n";
    float Wavelet[12]{0.016387336, -0.041464937, -0.067372555, 0.386110067, 0.812723635, 0.416998396,  
                      0.076488599,  -0.059434419, 0.023680172, 0.005611435,  0.001823209,  -0.000720549};

    for(int i=0; i<nrows; i++){
        for(int j=0; j<ncols; j++){
            ptr_previous[j+i*ncols]=0.0f;
            ptr_next[j+i*ncols]=0.0f;
            ptr_velocity[j+i*ncols]=1500.0f*1500.0f;
        }
    }

    for (int s=11; s>=0; s--){
      for (int i=(nrows/2)-s; i<(nrows/2)+s; i++){
        for (int k=(ncols/2)-s; k<(ncols/2)+s; k++){
          ptr_previous[i*ncols+k] = Wavelet[s];
        }
      }
    }
}

void printDevice(sycl::queue& q){
    sycl::device Device{q.get_device()};
    size_t Max_Blocksize{Device.get_info<sycl::info::device::max_work_group_size>()};
    size_t Max_EUcount{Device.get_info<sycl::info::device::max_compute_units>()};

    std::cout<<"Selected device: "<<Device.get_info<sycl::info::device::name>()<<"\n";
    std::cout<<"Device Max work group size: "<<Max_Blocksize<<"\n";
    std::cout<<"Device Max compute unit: "<<Max_EUcount<<"\n";
}

void Iso2Dfditeration(sycl::id<2> it, float* next, float* prev, 
                    sycl::multi_ptr<const float, sycl::access::address_space::global_space, (sycl::access::decorated)2> vel,
                    const float Dt2byDx2, int nRows, int nCols){
                        float value{0.0f};

                        int gidRow = it.get(0);
                        int gidCol = it.get(1);

                        int gid = gidRow*nCols+gidCol;

                        if((gidCol>=half_length && gidCol<nCols-half_length) &&
                            (gidRow>=half_length && gidRow<nRows-half_length)){
                                value += (prev[gid+1]+prev[gid-1]+prev[gid+nCols]+prev[gid-nCols] - 4.0f*prev[gid]);
                                value *= Dt2byDx2*vel[gid];
                                next[gid] = 2*prev[gid]-next[gid]+value;
                            }
                    }

void Message(const char* message){std::cout<<message<<std::endl;}


int main(int argc, char* argv[]){

    size_t n_rows;
    size_t n_cols;
    unsigned int iterations;
    n_rows =std::stoi(argv[1]);
    n_cols =std::stoi(argv[2]);
    iterations =std::stoi(argv[3]);

    size_t n_size = n_rows*n_cols;
    float* previous_base = new float[n_size]{};
    float* next_base = new float[n_size]{};
    float* next_cpu = new float[n_size]{};
    float* velocity_base = new float[n_size]{};

    float Dt2bDXY = (DT*DT)/(DXY*DXY);

    Initialize(previous_base, next_base, velocity_base, n_rows, n_cols);

    sycl::queue q{sycl::gpu_selector_v};

    printDevice(q);

    Message("Kernal begin");

    dpc_common::TimeInterval StartOffload;

    //Scope of the buffer
    {
        sycl::buffer preBuff(previous_base, sycl::range(n_size));
        sycl::buffer nxtBuff(next_base, sycl::range(n_size));
        sycl::buffer velBuff(velocity_base, sycl::range(n_size));
        //q.wait();
        Message("Buffer Alocated ...");
        for(unsigned int i=0; i<iterations; i++){
            q.submit([&](sycl::handler &h){
                sycl::accessor prevA(preBuff, h);
                sycl::accessor nextA(nxtBuff, h);
                sycl::accessor velA(velBuff, h);

                sycl::range globalRange = sycl::range<2>(n_rows, n_cols);

                if(i%2 == 0){
                    h.parallel_for(globalRange, [=](sycl::id<2> it){
                        Iso2Dfditeration(it, nextA.get_pointer(), prevA.get_pointer(), velA.get_pointer(), Dt2bDXY, n_rows, n_cols);
                    });
                }else{
                    h.parallel_for(globalRange, [=](sycl::id<2> it){
                        Iso2Dfditeration(it, prevA.get_pointer(), nextA.get_pointer(), velA.get_pointer(), Dt2bDXY, n_rows, n_cols);
                    });
                }
            });
        }
    }//End of buffer scope
    q.wait_and_throw();

    float time = StartOffload.Elapsed();

    Message("Simulation completed");

    saveAsVTK(next_base, n_rows, n_cols, "simulation_output.vtk");

    q.wait();
    delete[] previous_base;
    delete[] next_base;
    delete[] velocity_base;

    std::cout<<"Elapsed time: "<<time<<"\n";
    return 0;

}