#include "Phase_field.h"

void solve(sycl::queue &q, field *current, field *previous, double W, double K, double M, double dt){
    auto nx = current->nx;
    auto ny = current->ny;
    auto nz = current->nz;

    auto dx2 = previous->dx * previous->dx;
    auto dy2 = previous->dy * previous->dy;
    auto dz2 = previous->dz * previous->dz;

    // buffer range
    sycl::buffer<double, 3> current_buff{current->data.data(), sycl::range<3>(nx +2, ny+2, nz+2)};
    sycl::buffer<double, 3> previous_buff{previous->data.data(), sycl::range<3>(nx +2, ny+2, nz+2)};

    q.submit([&](sycl::handler &h){
        sycl::accessor current_acc{current_buff, h, sycl::read_write};
        sycl::accessor previous_acc{previous_buff, h, sycl::read_only};

        h.parallel_for(sycl::range(nx, ny, nz), [=](sycl::id<3> idx){
            auto j = idx[0]+1;
            auto i = idx[1] +1;
            auto k = idx[2] +1;

            double Laplacian = (previous_acc[j][i+1][k] - 2 * previous_acc[j][i][k] + previous_acc[j][i-1][k]) / dx2
                                + (previous_acc[j+1][i][k] - 2 * previous_acc[j][i][k] + previous_acc[j-1][i][k]) / dy2
                                + (previous_acc[j][i][k+1] - 2 * previous_acc[j][i][k] + previous_acc[j][i][k-1]) / dz2;

            double d_freeEnergy = 2 * W * previous_acc[j][i][k] * (1 - previous_acc[j][i][k]) * (1 - 2*previous_acc[j][i][k]);

            current_acc[j][i][k] = previous_acc[j][i][k] + dt * M* ( K * Laplacian - d_freeEnergy);
        });
    });

}