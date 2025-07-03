#include <vector>
#include <oneapi/dpl/random>

struct fields{
    int nx;
    int ny;
    int nz;

    double dx;
    double dy;
    double dz;

    std::vector<double> phi;
    std::vector<double> Laplacian;
};

void set_dimentions(fields *data, int nx, int ny, int nz, double Dx, double Dy, double Dz){
    data->dx = Dx;
    data->dy = Dy;
    data->dz = Dz;

    data->nx = nx;
    data->ny = ny;
    data->nz = nz;
}


void Allocate_field(fields *Phase_field){
    int new_size = (Phase_field->nx + 2) * (Phase_field->ny +2) * (Phase_field->nz+2);
    Phase_field->phi.resize(new_size, 0.0);
    Phase_field->Laplacian.resize(new_size, 0.0);
}

void Geneate_field(sycl::queue q, fields *Current){
    auto nx = Current->nx;
    auto ny = Current->ny;
    auto nz = Current->nz;

    std::uint32_t seed = 777;

    sycl::buffer<double, 3> current_buff(Current->phi.data(), sycl::range<3>(nx+2, ny+2, nz+2));

    q.submit([&](sycl::handler &h){
        sycl::accessor current_acc{current_buff, h, sycl::write_only};
        h.parallel_for(sycl::range<3>(nx+2, ny+2, nz+2),[=](sycl::id<3> idx){
            int j = idx[0];
            int i = idx[1];
            int k = idx[2];

            std::uint64_t index = i * (nx+2) * (nz+2) + j * (nz+2) + k;

            oneapi::dpl::minstd_rand engine(seed, index);
            oneapi::dpl::uniform_real_distribution<float> distr(0.0f, 1.0f);

            current_acc[i][j][k] = distr(engine);
        });
    });
    
}


void Initialize(sycl::queue q, int argc, char* argv[], fields *current, fields *previous, int *numsteps, double Dx, double Dy, double Dz){
    int rows = atoi(argv[1]);
    int cols = atoi(argv[2]);
    int height = atoi(argv[3]);

    *numsteps = atoi(argv[4]);

    set_dimentions(current, rows, cols, height, Dx, Dy, Dz);
    set_dimentions(previous, rows, cols, height, Dx, Dy, Dz);

    Allocate_field(current);
    Allocate_field(previous);

    Geneate_field(q, current);
}