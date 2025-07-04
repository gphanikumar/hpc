//#include "./structures.h"
#include <math.h>
#include <vector>
#include <oneapi/dpl/random>
#include "Utility.h"


void solve_eq(sycl::queue &q, fields *current, fields *previous, double epsilon, double lamda, double under_cooling, double Dt, double Diffu){
    auto nx = current->nx;
    auto ny = current->ny;

    auto dx = previous->dx;
    auto dy = previous->dy;

    auto dx2 = previous->dx * previous->dx;
    auto dy2 = previous->dy * previous->dy;

    // buffer range
    sycl::buffer<double, 2> current_buff{current->phi.data(), sycl::range<2>(nx +2, ny+2)};
    sycl::buffer<double, 2> previous_buff{previous->phi.data(), sycl::range<2>(nx +2, ny+2)};
    sycl::buffer<double, 2> Temp_curr_buff{current->Temperature.data(), sycl::range<2>(nx +2, ny+2)};
    sycl::buffer<double, 2> Temp_prev_buff{previous->Temperature.data(), sycl::range<2>(nx +2, ny+2)};


    q.submit([&](sycl::handler &h){
        sycl::accessor current_acc{current_buff, h, sycl::write_only};
        sycl::accessor previous_acc{previous_buff, h, sycl::read_only};
        sycl::accessor Temp_curr_acc{Temp_curr_buff, h, sycl::write_only};
        sycl::accessor Temp_prev_acc{Temp_prev_buff, h, sycl::read_only};

        h.parallel_for(sycl::range(nx, ny), [=](sycl::id<2> idx){
            auto i = idx[0]+1;
            auto j = idx[1] +1;


            double Dphi_dx_c = (previous_acc[i+1][j] - previous_acc[i-1][j])/(2*dx);
            double Dphi_dy_c = (previous_acc[i][j+1] - previous_acc[i][j-1])/(2*dy);

            double Dphi_dx_Pj = (previous_acc[i+1][j] - previous_acc[i][j])/dx;
            double Dphi_dx_Nj = (previous_acc[i][j] - previous_acc[i-1][j])/dx;

            double Dphi_dx_iP = (previous_acc[i+1][j] + previous_acc[i+1][j+1] - previous_acc[i-1][j+1] - previous_acc[i-1][j])/(4*dx);
            double Dphi_dx_iN = (previous_acc[i+1][j] + previous_acc[i+1][j-1] - previous_acc[i-1][j-1] - previous_acc[i-1][j])/(4*dx);

            double Dphi_dy_iP = (previous_acc[i][j+1] - previous_acc[i][j])/dy;
            double Dphi_dy_iN = (previous_acc[i][j] - previous_acc[i][j-1])/dy;

            double DPhi_dy_Pj = (previous_acc[i+1][j+1] + previous_acc[i][j+1] - previous_acc[i][j-1] - previous_acc[i+1][j-1])/(4*dy);
            double DPhi_dy_Nj = (previous_acc[i][j-1] + previous_acc[i-1][j+1] - previous_acc[i-1][j-1] - previous_acc[i][j-1])/(4*dy);

            // A_ip_j  A_in_j  A_i_jp  A_i_jn
            double Aniso_ener[5] = {0.0};
            double Aniso_prime[5] = {0.0};
            
            Anisotropy_calc(&Aniso_ener[0], &Aniso_prime[0], &Dphi_dx_Pj, &DPhi_dy_Pj, &epsilon);
            Anisotropy_calc(&Aniso_ener[1], &Aniso_prime[1], &Dphi_dx_Nj, &DPhi_dy_Nj, &epsilon);
            Anisotropy_calc(&Aniso_ener[2], &Aniso_prime[2], &Dphi_dx_iP, &Dphi_dy_iP, &epsilon);
            Anisotropy_calc(&Aniso_ener[3], &Aniso_prime[3], &Dphi_dx_iN, &Dphi_dy_iN, &epsilon);

            Anisotropy_calc(&Aniso_ener[4], &Aniso_prime[4], &Dphi_dx_c, &Dphi_dy_c, &epsilon);

            double Laplacian_pf = (Aniso_ener[0] * Aniso_ener[0] * Dphi_dx_Pj - Aniso_ener[1] * Aniso_ener[1] * Dphi_dx_Nj)/ dx;
            Laplacian_pf += (Aniso_ener[2] * Aniso_ener[2] * Dphi_dy_iP - Aniso_ener[3] * Aniso_ener[3] * Dphi_dy_iN) / dy;
            Laplacian_pf += (-1 * Aniso_ener[0] * Aniso_prime[0] * DPhi_dy_Pj + Aniso_ener[1] * Aniso_prime[1] * DPhi_dy_Nj) / dy;
            Laplacian_pf += (Aniso_ener[2] * Aniso_prime[2] * Dphi_dx_iP - Aniso_ener[3] * Aniso_prime[3] * Dphi_dx_iN) / dx;

            double Laplacian_Temp = (Temp_prev_acc[i][j+1] - 2 * Temp_prev_acc[i][j] + Temp_prev_acc[i][j-1]) / dy2;
            Laplacian_Temp += (Temp_prev_acc[i+1][j] - 2 * Temp_prev_acc[i][j] + Temp_prev_acc[i-1][j]) / dx2;
            

            double double_well = previous_acc[i][j] * ( previous_acc[i][j] * previous_acc[i][j] - 1);

            double latent_heat = lamda * under_cooling * ( 1.0 - previous_acc[i][j] * previous_acc[i][j]) * ( 1.0 - previous_acc[i][j] * previous_acc[i][j]);

            double Dphi_dt = 1.0/(Aniso_ener[4]*Aniso_ener[4]) * ( Laplacian_pf - double_well - latent_heat );

            current_acc[i][j] = previous_acc[i][j] +  Dt * sycl::fmin(sycl::fmax(Dphi_dt, -0.98), 0.98);
            Temp_curr_acc[i][j] = Temp_prev_acc[i][j] + ((Diffu * Dt) / (dx * dy)) * Laplacian_Temp + 0.5 * Dt * Dphi_dt;
        });
    }).wait();

    // Apply Nueman or Zero flux bounary conditions
    q.submit([&](sycl::handler &h){
        sycl::accessor current_acc{current_buff, h, sycl::read_write};
        sycl::accessor Temp_curr_acc{Temp_curr_buff, h, sycl::read_write};

        h.parallel_for(sycl::range(nx, ny), [=](sycl::id<2> idx){
            auto i = idx[0]+1;
            auto j = idx[1] +1;

            // Top and bottom
            // Phase field values
            current_acc[0][j] = current_acc[1][j];
            current_acc[nx+1][j] = current_acc[nx][j];

            // Scaled temperature or undercooling
            Temp_curr_acc[0][j] = Temp_curr_acc[1][j];
            Temp_curr_acc[nx+1][j] = Temp_curr_acc[nx][j];

            // Left and right
            current_acc[i][0] = current_acc[i][1];
            current_acc[i][ny+1] = current_acc[i][ny];

            Temp_curr_acc[i][0] = Temp_curr_acc[i][1];
            Temp_curr_acc[i][nx+1] = Temp_curr_acc[i][nx];

        });
    }).wait();

}


void Add_noise(sycl::queue &q, fields *phase_field, double *Dx, double *Dy){
    auto nx = phase_field->nx;
    auto ny = phase_field->ny;


    std::uint32_t seed = 777;

    sycl::buffer<double, 2> field_buff{phase_field->phi.data(), sycl::range<2>(nx +2, ny+2)};

    q.submit([&](sycl::handler &h){
        sycl::accessor field_acc{field_buff, h, sycl::read_write};

        h.parallel_for(sycl::range(nx, ny), [=](sycl::id<2> idx){
            auto i = idx[0]+1;
            auto j = idx[1] +1;

            long index = i * (ny +2) + j;
            
            double Laplacian = (field_acc[i+1][j] - field_acc[i][j] + field_acc[i-1][j])/(*Dx);
            Laplacian += (field_acc[i][j+1] - field_acc[i][j] + field_acc[i][j-1])/(*Dy);

            if(Laplacian >= 0.1){

                oneapi::dpl::minstd_rand engine(seed, index);
                oneapi::dpl::minstd_rand engine1(seed, index+1);
                oneapi::dpl::minstd_rand engine2(seed, index+2);
                oneapi::dpl::uniform_real_distribution<float> distr(-1.0f, 1.0f);
                field_acc[i][j] = distr(engine);
                field_acc[i+1][j] = distr(engine1);
                field_acc[i][j+1] = distr(engine2);
                field_acc[i-1][j] = distr(engine1);
                field_acc[i][j-1] = distr(engine2);
            }
        
        });
    }).wait();

}
