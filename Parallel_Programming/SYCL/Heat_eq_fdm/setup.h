#include "heat.h"


// set dimensions of the field
void set_field_dimensions(field *temperature, int nx, int ny){
    temperature->dx = DX;
    temperature->dy = DY;
    temperature->nx = nx;
    temperature->ny = ny;
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


void generate_field(field *temperature){
    int ind; //global indices
    double radius;
    int dx, dy;

    // Allocate the temperature array including the gost cells for BCs
    int newsize = (temperature->nx+2) * (temperature->ny+2);
    temperature->data.resize(newsize, 0.0);

    radius = temperature->nx / 6.0;

    for(int i=0; i<temperature->nx +2; ++i){
        for(int j=0; j<temperature->ny +2; ++j){
            ind = i * (temperature->ny +2) +j;
            
            dx = i - temperature->nx/2 +1;
            dy = j - temperature->ny/2 +1;

            if(dx*dx + dy*dy < radius*radius){
                temperature->data[ind] = 5.0;
            }else{
                temperature->data[ind] = 65.0;
            }
        }
    }

    // Boundary conditions
    // Top and bottom boundary conditions
    for(int i=0; i<temperature->nx + 2; ++i){
        temperature->data[i * (temperature->ny+2)] = 20.0;
        temperature->data[i * (temperature->ny+2) + temperature->ny+1] = 70.0;
    }

    // Right boundary condition
    for(int j=0; j<temperature->ny+2; ++j){
        temperature->data[j] = 85.0; 
    }
    
    // Left boundary condition
    for(int j=0; j<temperature->ny+2; ++j){
        temperature->data[(temperature->nx + 1) * (temperature->ny + 2) +j] = 5.0;
    }
}