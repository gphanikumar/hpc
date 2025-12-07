#include "utilities/IO.hpp"


IO::IO(std::string &fname, const int NX, const int NY):
    filename(fname), Nx(NX), Ny(NY){}

// Wrie the .h5 file
void IO::Write_h5f5(const int iter, float* phi, float* Temp){
    std::string H5filename = "DATA/"+filename+"_"+std::to_string(iter)+".h5";

    H5::H5File file(H5filename, H5F_ACC_TRUNC);

    // Defining size of the dataspace in file
    hsize_t dims_file[2] = {static_cast<hsize_t>(Nx), static_cast<hsize_t>(Ny)};
    H5::DataSpace dataspace_file(2, dims_file);

    // Define datatype
    H5::FloatType datatype(H5::PredType::NATIVE_FLOAT);

    // Define memory dataspace (Nx+2 x Ny+2) with ghost cells
    hsize_t dims_mem[2] = {static_cast<hsize_t>(Nx + 2), static_cast<hsize_t>(Ny + 2)};
    H5::DataSpace dataspace_mem(2, dims_mem);

    // Select hyperslab in memory to skip ghost cells
    hsize_t start[2] = {1, 1};
    hsize_t count[2] = {static_cast<hsize_t>(Nx), static_cast<hsize_t>(Ny)};
    dataspace_mem.selectHyperslab(H5S_SELECT_SET, count, start);

    // write phi field
    H5::DataSet dataset_phi = file.createDataSet("phi", datatype, dataspace_file);
    dataset_phi.write(phi, H5::PredType::NATIVE_FLOAT, dataspace_mem, dataspace_file);

    //write Temperature field
    H5::DataSet dataset_temp = file.createDataSet("Temp", datatype, dataspace_file);
    dataset_temp.write(Temp, H5::PredType::NATIVE_FLOAT, dataspace_mem, dataspace_file);
}

//Write the .xmf file
void IO::WriteXmf(const int iter){
    std::string H5filename = filename+"_"+std::to_string(iter)+".h5";
    std::string xmffilename = "DATA/"+filename+"_"+std::to_string(iter)+".xmf";

    std::ofstream xmf(xmffilename);
    xmf << "<?xml version=\"1.0\" ?>\n";
    xmf << "<Xdmf Version=\"2.0\">\n";
    xmf << " <Domain>\n";
    xmf << "   <Grid Name=\"PhaseField\" GridType=\"Uniform\">\n";
    xmf << "     <Topology TopologyType=\"2DCoRectMesh\" Dimensions=\"" << Nx << " " << Ny << "\"/>\n";
    xmf << "     <Geometry GeometryType=\"Origin_DxDy\">\n";
    xmf << "       <DataItem Name=\"Origin\" Dimensions=\"2\" NumberType=\"Float\" Format=\"XML\">\n";
    xmf << "         0 0\n";
    xmf << "       </DataItem>\n";
    xmf << "       <DataItem Name=\"Spacing\" Dimensions=\"2\" NumberType=\"Float\" Format=\"XML\">\n";
    xmf << "         1 1\n"; 
    xmf << "       </DataItem>\n";
    xmf << "     </Geometry>\n";
    
    // Attribute: Phi
    xmf << "     <Attribute Name=\"Phi\" AttributeType=\"Scalar\" Center=\"Node\">\n";
    xmf << "       <DataItem Dimensions=\"" << Nx << " " << Ny << "\" NumberType=\"Float\" Precision=\"4\" Format=\"HDF\">\n";
    xmf << "        " << H5filename << ":/phi\n";
    xmf << "       </DataItem>\n";
    xmf << "     </Attribute>\n";

    // Attribute: Temperature
    xmf << "     <Attribute Name=\"Temperature\" AttributeType=\"Scalar\" Center=\"Node\">\n";
    xmf << "       <DataItem Dimensions=\"" << Nx << " " << Ny << "\" NumberType=\"Float\" Precision=\"4\" Format=\"HDF\">\n";
    xmf << "        " << H5filename << ":/Temp\n";
    xmf << "       </DataItem>\n";
    xmf << "     </Attribute>\n";

    xmf << "   </Grid>\n";
    xmf << " </Domain>\n";
    xmf << "</Xdmf>\n";
    xmf.close();
}

void IO::write_output(const int iter, float* phi, float* Temp){
    Write_h5f5(iter, phi, Temp);
    WriteXmf(iter);
}
