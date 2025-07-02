#include "Phase_field.h"
#include <cstdlib>


// set dimensions of the field
void set_field_dimensions(field *Phase_field, int nx, int ny){
    Phase_field->dx = DX;
    Phase_field->dy = DY;
    Phase_field->nx = nx;
    Phase_field->ny = ny;
}

void initialize(int argc, char* argv[], field *current, field *previous, int *nsteps){
    int rows = atoi(argv[1]);
    int cols = atoi(argv[2]);

    *nsteps = atoi(argv[3]);

    set_field_dimensions(current, rows, cols);
    set_field_dimensions(previous, rows, cols);
    generate_field(current);
    allocate_field(previous);
    copy_field(current, previous);
}


void generate_field(field *Phase_field){
    int ind;
    double radius;
    int dx, dy;

    // Allocate the Phase_field array including the gost cells for BCs
    int newsize = (Phase_field->nx+2) * (Phase_field->ny+2);
    Phase_field->data.resize(newsize, 0.0);
    Phase_field->Temperature.resize(newsize, 0.0);
    radius = R;

    srand((unsigned) time(NULL));

    for(int i=0; i<Phase_field->nx +2; ++i){
        for(int j=0; j<Phase_field->ny +2; ++j){
            ind = i * (Phase_field->ny +2) +j;

            dx = i - Phase_field->nx/2 +1;
            dy = j - Phase_field->ny/2 +1;

            if(dx*dx + dy*dy < radius*radius){
                Phase_field->data[ind] = 1.0;
            }else{
                Phase_field->data[ind] = 0.0;
            }

            Phase_field->Temperature[ind] = 0.9;
            //float random = rand() / (float)RAND_MAX;
            //float value = 0.0;
            //if(random >= 0.8f){
            //    value = 1.0f;
            //}else if(random < 0.2f){
            //    value = 0.0f;
            //}else{value = random;}
//
            //Phase_field->data[ind] = value;

        }
    }

    // Boundary conditions - Phase field
    // Top and bottom boundary conditions Neuman boundary conditions
    for(int i=0; i<Phase_field->nx + 2; ++i){
        Phase_field->data[i * (Phase_field->ny+2)] = Phase_field->data[i * (Phase_field->ny+2) +1];
        Phase_field->data[i * (Phase_field->ny+2) + Phase_field->ny+1] = Phase_field->data[i * (Phase_field->ny+2) + Phase_field->ny];
    }

    // Right boundary condition Neuman boundary conditions
    for(int j=0; j<Phase_field->ny+2; ++j){
        Phase_field->data[j] = Phase_field->data[(Phase_field->ny+2) +j];
        Phase_field->data[(Phase_field->nx + 1) * (Phase_field->ny + 2) +j] = Phase_field->data[(Phase_field->nx) * (Phase_field->ny + 2) +j];
    }

    //Boundary conditions - Temperature
    // Top and bottomo bounary conditions
    for(int i=0; i<Phase_field->nx + 2; ++i){
        Phase_field->Temperature[i * (Phase_field->ny+2)] = Phase_field->Temperature[i * (Phase_field->ny+2) +1];
        Phase_field->Temperature[i * (Phase_field->ny+2) + Phase_field->ny+1] = Phase_field->Temperature[i * (Phase_field->ny+2) + Phase_field->ny];
    }

    // Right boundary condition Neuman boundary conditions
    for(int j=0; j<Phase_field->ny+2; ++j){
        Phase_field->Temperature[j] = Phase_field->Temperature[(Phase_field->ny+2) +j];
        Phase_field->Temperature[(Phase_field->nx + 1) * (Phase_field->ny + 2) +j] = Phase_field->Temperature[(Phase_field->nx) * (Phase_field->ny + 2) +j];
    }
}

void Apply_boundary_conditions(field *Phase_field){
    // Boundary conditions - Phase field
    // Top and bottom boundary conditions Neuman boundary conditions
    for(int i=0; i<Phase_field->nx + 2; ++i){
        Phase_field->data[i * (Phase_field->ny+2)] = Phase_field->data[i * (Phase_field->ny+2) +1];
        Phase_field->data[i * (Phase_field->ny+2) + Phase_field->ny+1] = Phase_field->data[i * (Phase_field->ny+2) + Phase_field->ny];
    }

    // Right boundary condition Neuman boundary conditions
    for(int j=0; j<Phase_field->ny+2; ++j){
        Phase_field->data[j] = Phase_field->data[(Phase_field->ny+2) +j];
        Phase_field->data[(Phase_field->nx + 1) * (Phase_field->ny + 2) +j] = Phase_field->data[(Phase_field->nx) * (Phase_field->ny + 2) +j];
    }

    //Boundary conditions - Temperature
    // Top and bottomo bounary conditions
    for(int i=0; i<Phase_field->nx + 2; ++i){
        Phase_field->Temperature[i * (Phase_field->ny+2)] = Phase_field->Temperature[i * (Phase_field->ny+2) +1];
        Phase_field->Temperature[i * (Phase_field->ny+2) + Phase_field->ny+1] = Phase_field->Temperature[i * (Phase_field->ny+2) + Phase_field->ny];
    }

    // Right boundary condition Neuman boundary conditions
    for(int j=0; j<Phase_field->ny+2; ++j){
        Phase_field->Temperature[j] = Phase_field->Temperature[(Phase_field->ny+2) +j];
        Phase_field->Temperature[(Phase_field->nx + 1) * (Phase_field->ny + 2) +j] = Phase_field->Temperature[(Phase_field->nx) * (Phase_field->ny + 2) +j];
    }
}