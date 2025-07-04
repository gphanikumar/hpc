#include <vector>
#include <oneapi/dpl/random>

struct fields{
    int nx;
    int ny;

    double dx;
    double dy;

    std::vector<double> phi;
    std::vector<double> Temperature;
};

enum seed_Type{Center_Circle, Bottom_Center_Circle ,Bottom_Rectangle};

void set_dimentions(fields *data, int nx, int ny, double Dx, double Dy){
    data->dx = Dx;
    data->dy = Dy;

    data->nx = nx;
    data->ny = ny;
}


void Allocate_field(fields *Phase_field){
    int new_size = (Phase_field->nx + 2) * (Phase_field->ny +2);
    Phase_field->phi.resize(new_size, 0.0);
    Phase_field->Temperature.resize(new_size, 0.0);
}

void Geneate_field(sycl::queue q, fields *Current, seed_Type intial, double Seed_param, double U){
    auto nx = Current->nx;
    auto ny = Current->ny;

    std::uint32_t seed = 777;

    sycl::buffer<double, 2> current_buff(Current->phi.data(), sycl::range<2>(nx+2, ny+2));
    sycl::buffer<double, 2> Temp_buff(Current->Temperature.data(), sycl::range<2>(nx+2, ny+2));

    q.submit([&](sycl::handler &h){
        sycl::accessor current_acc{current_buff, h, sycl::write_only, sycl::no_init};
        sycl::accessor Temp_acc{Temp_buff, h, sycl::write_only, sycl::no_init};
        h.parallel_for(sycl::range<2>(nx+2, ny+2),[=](sycl::id<2> idx){
            int i = idx[0];
            int j = idx[1];

            long index = i * (ny+2) + j;

            Temp_acc[i][j] = U;
            
            switch (intial) {
                case Center_Circle: {
                    double dx = i - (nx / 2.0 + 1);
                    double dy = j - (ny / 2.0 + 1);

                    if (dx * dx + dy * dy <= Seed_param * Seed_param) {
                        current_acc[i][j] = 1.0;
                    } else {
                        current_acc[i][j] = -1.0;
                    }
                    break;
                }

                case Bottom_Rectangle: {
                    if (i < static_cast<int>(Seed_param)) {
                        current_acc[i][j] = 1.0;
                    } else {
                        current_acc[i][j] = 0.0;
                    }
                    break;
                }

                case Bottom_Center_Circle: {
                    double dx = i - 1;
                    double dy = j - (ny / 2.0 + 1);

                    if (dx * dx + dy * dy <= Seed_param * Seed_param) {
                        current_acc[i][j] = 1.0;
                    } else {
                        current_acc[i][j] = -1.0;
                    }
                    break;
                }

                default: {
                    current_acc[i][j] = 0.0;
                    break;
                }
            }
                
        });
    }).wait();
    
}


void Initialize(sycl::queue q, int argc, char* argv[], fields *current, fields *previous, int *numsteps, double Dx, double Dy, double R, double U){
    int rows = atoi(argv[1]);
    int cols = atoi(argv[2]);

    *numsteps = atoi(argv[3]);

    set_dimentions(current, rows, cols, Dx, Dy);
    set_dimentions(previous, rows, cols, Dx, Dy);

    Allocate_field(current);
    Allocate_field(previous);

    Geneate_field(q, current, Bottom_Center_Circle, R, U);
}