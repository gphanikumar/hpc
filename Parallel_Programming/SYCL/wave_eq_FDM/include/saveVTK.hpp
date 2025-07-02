#ifndef SAVE_VTK_HPP
#define SAVE_VTK_HPP

#include <iostream>
#include <fstream>
#include <string>

// Add this function to save the output in .vtk format
void saveAsVTK(const float* data, size_t nrows, size_t ncols, const std::string& filename) {
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
    vtkFile << "DIMENSIONS " << ncols << " " << nrows << " 1\n";
    vtkFile << "ORIGIN 0 0 0\n";
    vtkFile << "SPACING 1 1 1\n";
    vtkFile << "POINT_DATA " << nrows * ncols << "\n";
    vtkFile << "SCALARS simulation_data float 1\n";
    vtkFile << "LOOKUP_TABLE default\n";

    // Write data
    for (size_t i = 0; i < nrows; ++i) {
        for (size_t j = 0; j < ncols; ++j) {
            vtkFile << data[i * ncols + j] << " ";
        }
        vtkFile << "\n";
    }

    vtkFile.close();
    std::cout << "VTK file saved as " << filename << "\n";
}

#endif