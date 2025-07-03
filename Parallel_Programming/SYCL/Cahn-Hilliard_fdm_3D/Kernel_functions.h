//#include "./structures.h"



void solve_eq(sycl::queue &q, fields *current, fields *previous, double W, double K, double M, double dt){
    auto nx = current->nx;
    auto ny = current->ny;
    auto nz = current->nz;

    auto dx2 = previous->dx * previous->dx;
    auto dy2 = previous->dy * previous->dy;
    auto dz2 = previous->dz * previous->dz;

    // buffer range
    sycl::buffer<double, 3> current_buff{current->phi.data(), sycl::range<3>(nx +2, ny+2, nz+2)};
    sycl::buffer<double, 3> previous_buff{previous->phi.data(), sycl::range<3>(nx +2, ny+2, nz+2)};
    sycl::buffer<double, 3> Laplacian_buff{current->Laplacian.data(), sycl::range<3>(nx +2, ny+2, nz+2)};

    q.submit([&](sycl::handler &h){
        sycl::accessor Laplacian_acc{Laplacian_buff, h, sycl::write_only};
        sycl::accessor previous_acc{previous_buff, h, sycl::read_only};

        h.parallel_for(sycl::range(nx, ny, nz), [=](sycl::id<3> idx){
            auto j = idx[0]+1;
            auto i = idx[1] +1;
            auto k = idx[2] +1;

            double Laplacian = (previous_acc[j][i+1][k] - 2 * previous_acc[j][i][k] + previous_acc[j][i-1][k]) / dx2
                                + (previous_acc[j+1][i][k] - 2 * previous_acc[j][i][k] + previous_acc[j-1][i][k]) / dy2
                                + (previous_acc[j][i][k+1] - 2 * previous_acc[j][i][k] + previous_acc[j][i][k-1]) / dz2;

            double d_freeEnergy = 2 * W * previous_acc[j][i][k] * (1 - previous_acc[j][i][k]) * (1 - 2*previous_acc[j][i][k]);

            Laplacian_acc[j][i][k] = (d_freeEnergy - K * Laplacian);
        });
    }).wait();

    // Find the fourth order term
    q.submit([&](sycl::handler &h){
        sycl::accessor Laplacian_acc{Laplacian_buff, h, sycl::read_only};
        sycl::accessor previous_acc{previous_buff, h, sycl::read_only};
        sycl::accessor current_acc{current_buff, h, sycl::write_only};

        h.parallel_for(sycl::range(nx, ny, nz), [=](sycl::id<3> idx){
            auto j = idx[0]+1;
            auto i = idx[1] +1;
            auto k = idx[2] +1;

            double delta_phi = previous_acc[j][i][k] + dt * M* ((Laplacian_acc[j][i+1][k] - 2 * Laplacian_acc[j][i][k] + Laplacian_acc[j][i-1][k]) / dx2
                                + (Laplacian_acc[j+1][i][k] - 2 * Laplacian_acc[j][i][k] + Laplacian_acc[j-1][i][k]) / dy2
                                + (Laplacian_acc[j][i][k+1] - 2 * Laplacian_acc[j][i][k] + Laplacian_acc[j][i][k-1]) / dz2);

            current_acc[j][i][k] = sycl::fmin(sycl::fmax(delta_phi, 0.02), 0.98);
        });
    }).wait();


    // Apply periodic bounary conditions
    q.submit([&](sycl::handler &h){
        sycl::accessor current_acc{current_buff, h, sycl::read_write};

        h.parallel_for(sycl::range(nx, ny, nz), [=](sycl::id<3> idx){
            auto j = idx[0]+1;
            auto i = idx[1] +1;
            auto k = idx[2] +1;

            // YZ Plane
            current_acc[0][i][k] = current_acc[nx][i][k];
            current_acc[nx+1][i][k] = current_acc[1][i][k];

            // XZ Plane
            current_acc[j][0][k] = current_acc[j][ny][k];
            current_acc[j][ny+1][k] = current_acc[j][1][k];

            // XY Plane
            current_acc[j][i][0] = current_acc[j][i][nz];
            current_acc[j][i][nz+1] = current_acc[j][i][1];

        });
    }).wait();

}