#pragma once

struct Kobayashi_Parameters{
    float Tau;
    float epsilon_bar;
    float K;
    float Te;
    float dt;
    float dx;
    float dx2;

    float aniso_strength;
};

struct sim_info{
    int Nx;
    int Ny;
    int time_steps;
    int saveT;
};

enum field{
    PHI,
    TEMP
};

enum BCType {
    BC_PERIODIC = 0,
    BC_NEUMANN  = 1,
};

struct FaceBCs {
    BCType phi;
    BCType temp;
};

struct DomainBCs {
    FaceBCs left, right;
    FaceBCs top, bottom;
};