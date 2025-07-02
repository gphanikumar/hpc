#include "Phase_field.h"

void solve(sycl::queue &q, field *current, field *previous, double W, double K, double M, double dt){
    auto nx = current->nx;
    auto ny = current->ny;

    auto dx2 = previous->dx * previous->dx;
    auto dy2 = previous->dy * previous->dy;

    // buffer range
    sycl::buffer<double, 2> current_buff{current->data.data(), sycl::range<2>(nx +2, ny+2)};
    sycl::buffer<double, 2> previous_buff{previous->data.data(), sycl::range<2>(nx +2, ny+2)};

    q.submit([&](sycl::handler &h){
        sycl::accessor current_acc{current_buff, h, sycl::read_write};
        sycl::accessor previous_acc{previous_buff, h, sycl::read_only};

        h.parallel_for(sycl::range(nx, ny), [=](sycl::id<2> idx){
            auto j = idx[0]+1;
            auto i = idx[1] +1;

            double Laplacian = (previous_acc[j][i+1] - 2 * previous_acc[j][i] + previous_acc[j][i-1]) / dx2
                                + (previous_acc[j+1][i] - 2 * previous_acc[j][i] + previous_acc[j-1][i]) / dy2;

            double d_freeEnergy = 2 * W * previous_acc[j][i] * (1 - previous_acc[j][i]) * (1 - 2*previous_acc[j][i]);

            current_acc[j][i] = previous_acc[j][i] + dt * M* ( K * Laplacian - d_freeEnergy);
        });
    });

}