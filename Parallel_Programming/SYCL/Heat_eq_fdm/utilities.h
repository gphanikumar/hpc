#include <cassert>

void allocate_field(field *temperature){
    int new_size = (temperature->nx + 2) * (temperature->ny +2);
    temperature->data.resize(new_size, 0.0);
}

void copy_field(field *temperature1, field *temperature2){
    assert(temperature1->nx == temperature2->nx);
    assert(temperature1->ny == temperature2->ny);
    assert(temperature1->data.size() == temperature2->data.size());

    std::copy(temperature1->data.begin(), temperature1->data.end(), temperature2->data.begin());
}