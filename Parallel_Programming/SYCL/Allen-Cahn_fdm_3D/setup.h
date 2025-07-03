#include "Phase_field.h"
#include <cstdlib>


// set dimensions of the field
void set_field_dimensions(field *Phase_field, int nx, int ny, int nz){
    Phase_field->dx = DX;
    Phase_field->dy = DY;
    Phase_field->dz = DZ;
    Phase_field->nx = nx;
    Phase_field->ny = ny;
    Phase_field->nz = nz;
}

void initialize(int argc, char* argv[], field *current, field *previous, int *nsteps){
    int rows = atoi(argv[1]);
    int cols = atoi(argv[2]);
    int height = atoi(argv[3]);

    *nsteps = atoi(argv[4]);

    set_field_dimensions(current, rows, cols, height);
    set_field_dimensions(previous, rows, cols, height);
    generate_field(current);
    allocate_field(previous);
    copy_field(current, previous);
}


void generate_field(field *Phase_field){
    int ind;
    double radius;
    int dx, dy, dz;

    // Allocate the Phase_field array including the gost cells for BCs
    int newsize = (Phase_field->nx+2) * (Phase_field->ny+2) * (Phase_field->nz +2);
    Phase_field->data.resize(newsize, 0.0);

    radius = R;

    srand((unsigned) time(NULL));

    for(int i=0; i<Phase_field->nx +2; ++i){
        for(int j=0; j<Phase_field->ny +2; ++j){
            for(int k=0; k<Phase_field->nz+2; ++k){
                ind = i * (Phase_field->ny +2) * (Phase_field->nz+2) + j * (Phase_field->nz+2) + k;
                
                //dx = i - Phase_field->nx/2 +1;
                //dy = j - Phase_field->ny/2 +1;
                //dz = k - Phase_field->nz/2 +1;

                //if(dx*dx + dy*dy < radius*radius){
                //    Phase_field->data[ind] = 1.0;
                //}else{
                //    Phase_field->data[ind] = 0.0;
                //}
                float random = rand() / (float)RAND_MAX;
                float value = 0.0;
                if(random >= 0.8f){
                    value = 1.0f;
                }else if(random < 0.2f){
                    value = 0.0f;
                }else{value = random;}

                Phase_field->data[ind] = value;
            }

        }
    }

    // 3D boundary conditions
    // YZ plane bouundary conditions
    for(int j=0; j<Phase_field->ny+2; ++j){
        for(int k=0; k<Phase_field->nz+2; ++k){
            // Bottom plane
            Phase_field->data[j*(Phase_field->nz+2)+k] = Phase_field->data[Phase_field->nx*(Phase_field->ny+2)*(Phase_field->nz+2) + j*(Phase_field->nz+2)+k];
            // Top plane
            Phase_field->data[(Phase_field->nx+1)*(Phase_field->ny+2)*(Phase_field->nz+2) + j*(Phase_field->nz+2)+k] = Phase_field->data[(Phase_field->ny+2)*(Phase_field->nz+2) +j*(Phase_field->nz+2)+k];
        }
    }

    // XZ Plane
    for(int i=0; i<Phase_field->nx + 2; ++i){
        for(int k=0; k<Phase_field->nz +2; ++k){
            // left plane
            Phase_field->data[i*(Phase_field->ny+2)*(Phase_field->nz+2)+k] = Phase_field->data[i*(Phase_field->ny+2)*(Phase_field->nz+2) + (Phase_field->ny)* (Phase_field->nz+2) +k];
            // right plane
            Phase_field->data[i*(Phase_field->ny+2)*(Phase_field->nz+2) + (Phase_field->ny+1)* (Phase_field->nz+2) +k] = Phase_field->data[i*(Phase_field->ny+2)*(Phase_field->nz+2)+(Phase_field->nz+2)+k];
        }
    }

    // XY Plane
    for(int i=0; i<Phase_field->nx+2; ++i){
        for(int j=0; j<Phase_field->ny+2; ++j){
            // Frond plane
            Phase_field->data[i*(Phase_field->ny+2)*(Phase_field->nz+2)+j*(Phase_field->nz+2)] = Phase_field->data[i*(Phase_field->ny+2)*(Phase_field->nz+2)+j*(Phase_field->nz+2) + (Phase_field->nz)];
            // Rear plane
            Phase_field->data[i*(Phase_field->ny+2)*(Phase_field->nz+2)+j*(Phase_field->nz+2) + (Phase_field->nz+1)] = Phase_field->data[i*(Phase_field->ny+2)*(Phase_field->nz+2)+j*(Phase_field->nz+2)+1];
        }
    }

}

void Apply_boundary_conditions(field *Phase_field){
    // 3D boundary conditions
    // YZ plane bouundary conditions
    for(int j=0; j<Phase_field->ny+2; ++j){
        for(int k=0; k<Phase_field->nz+2; ++k){
            // Bottom plane
            Phase_field->data[j*(Phase_field->nz+2)+k] = Phase_field->data[Phase_field->nx*(Phase_field->ny+2)*(Phase_field->nz+2) + j*(Phase_field->nz+2)+k];
            // Top plane
            Phase_field->data[(Phase_field->nx+1)*(Phase_field->ny+2)*(Phase_field->nz+2) + j*(Phase_field->nz+2)+k] = Phase_field->data[(Phase_field->ny+2)*(Phase_field->nz+2) +j*(Phase_field->nz+2)+k];
        }
    }

    // XZ Plane
    for(int i=0; i<Phase_field->nx + 2; ++i){
        for(int k=0; k<Phase_field->nz +2; ++k){
            // left plane
            Phase_field->data[i*(Phase_field->ny+2)*(Phase_field->nz+2)+k] = Phase_field->data[i*(Phase_field->ny+2)*(Phase_field->nz+2) + (Phase_field->ny)* (Phase_field->nz+2) +k];
            // right plane
            Phase_field->data[i*(Phase_field->ny+2)*(Phase_field->nz+2) + (Phase_field->ny+1)* (Phase_field->nz+2) +k] = Phase_field->data[i*(Phase_field->ny+2)*(Phase_field->nz+2)+(Phase_field->nz+2)+k];
        }
    }

    // XY Plane
    for(int i=0; i<Phase_field->nx+2; ++i){
        for(int j=0; j<Phase_field->ny+2; ++j){
            // Frond plane
            Phase_field->data[i*(Phase_field->ny+2)*(Phase_field->nz+2)+j*(Phase_field->nz+2)] = Phase_field->data[i*(Phase_field->ny+2)*(Phase_field->nz+2)+j*(Phase_field->nz+2) + (Phase_field->nz)];
            // Rear plane
            Phase_field->data[i*(Phase_field->ny+2)*(Phase_field->nz+2)+j*(Phase_field->nz+2) + (Phase_field->nz+1)] = Phase_field->data[i*(Phase_field->ny+2)*(Phase_field->nz+2)+j*(Phase_field->nz+2)+1];
        }
    }
}