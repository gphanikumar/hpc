#pragma once

constexpr auto DX = 0.01;
constexpr auto DY = 0.01;

struct field{
    int nx;
    int ny;

    double dx;
    double dy;

    std::vector<double> data;
};

// Function declaration
void set_field_dimensions(field *temperature, int nx, int ny);

void initialize(int argc, char* argv[], field *temperature1, field *temperature2, int *nsteps);

void generate_field(field *temperature);

void allocate_field(field *temperature);

void copy_field(field *temperature1, field *temperature2);