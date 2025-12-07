#pragma once

#include <string>
#include <H5Cpp.h>
#include <fstream>
#include <sstream>

class IO{
    private:
        std::string &filename;

        int Nx;
        int Ny;

        void Write_h5f5(const int iter, float* phi, float* Temp);
        
        void WriteXmf(const int iter);

    public:
        IO(std::string &fname, const int NX, const int NY);

        void write_output(const int iter, float* phi, float* Temp);
};