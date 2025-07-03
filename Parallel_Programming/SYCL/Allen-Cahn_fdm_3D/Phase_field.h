#pragma once



struct field{
    int nx;
    int ny;
    int nz;

    double dx;
    double dy;
    double dz;

    std::vector<double> data;
};

// Function declaration
void set_field_dimensions(field *Phase_field, int nx, int ny, int nz);

void initialize(int argc, char* argv[], field *Phase_field1, field *Phase_field2, int *nsteps);

void generate_field(field *Phase_field);

void allocate_field(field *Phase_field);

void copy_field(field *Phase_field1, field *Phase_field2);