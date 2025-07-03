#include <cassert>

void allocate_field(field *Phase_field){
    int new_size = (Phase_field->nx + 2) * (Phase_field->ny +2) * (Phase_field->nz+2);
    Phase_field->data.resize(new_size, 0.0);
}

void copy_field(field *Phase_field1, field *Phase_field2){
    assert(Phase_field1->nx == Phase_field2->nx);
    assert(Phase_field1->ny == Phase_field2->ny);
    assert(Phase_field1->nz == Phase_field2->nz);
    assert(Phase_field1->data.size() == Phase_field2->data.size());

    std::copy(Phase_field1->data.begin(), Phase_field1->data.end(), Phase_field2->data.begin());
}