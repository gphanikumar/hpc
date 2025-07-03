#include "Phase_field.h"

#include <iostream>
#include <fstream>
#include <string>

// Add this function to save the output in .vtk format
void saveAsVTK(field *Phase_field, int iter, const std::string &filename) {
    std::ofstream vtkFile(filename);
    if (!vtkFile.is_open()) {
        std::cerr << "Error: Could not open file " << filename << " for writing.\n";
        return;
    }

    // Write VTK file header
    vtkFile << "# vtk DataFile Version 3.0\n";
    vtkFile << "2D simulation data\n";
    vtkFile << "ASCII\n";
    vtkFile << "DATASET STRUCTURED_POINTS\n";
    vtkFile << "DIMENSIONS " << Phase_field->nx +2 << " " << Phase_field->ny +2<< " " << Phase_field->nz+2 <<"\n";
    vtkFile << "ORIGIN 0 0 0\n";
    vtkFile << "SPACING 1 1 1\n";
    vtkFile << "POINT_DATA " << (Phase_field->nx +2)* (Phase_field->ny +2) * (Phase_field->nz +2)<< "\n";
    vtkFile << "SCALARS simulation_data float 1\n";
    vtkFile << "LOOKUP_TABLE default\n";

    // Write data
    for (size_t i = 0; i < Phase_field->nx +2; ++i) {
        for (size_t j = 0; j < Phase_field->ny +2; ++j) {
            for (size_t k = 0; k < Phase_field->nz +2; ++k) {
                vtkFile << Phase_field->data[i * (Phase_field->ny +2) * (Phase_field->nz+2) + j * (Phase_field->nz+2) +k] << " ";
            }
            vtkFile << "\n";
        }
        vtkFile << "\n";
    }

    vtkFile.close();
    std::cout << "VTK file saved as " << filename << "\n";
}


