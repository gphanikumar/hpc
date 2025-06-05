#include <bits/stdc++.h>
#include "./SOL_INIT.hpp"
#include "./WRITER.hpp"
#include "./KOBAYASHI.hpp"
#include "./IO.hpp"


int main(int argc, char** argv) {

  /*********
	    INITIALIZE SIMULATION VARIABLES
  *********/
    
  int const MESH_X = 1024;
  int const MESH_Y = MESH_X;
  int const TOTAL_ITERATIONS = 3072;
  //int const TOTAL_ITERATIONS = 50;
  
  float SOLID =  0.0;
  float LIQUID = 1.0;
  float TEMP_SOL = -5.0;
  float TEMP_LIQ = -1.0;
  
  //int NOISE_FREQUENCY = 40;
  //float NOISE_AMPLITUDE = 6.1e-2;
  
  int CENTRE = MESH_X/2;
  int RADIUS = 10;
  
  SOL_INIT phase_init(CENTRE, RADIUS, SOLID, LIQUID) ;
  SOL_INIT temperature_init(CENTRE, RADIUS, TEMP_SOL, TEMP_LIQ) ;
  

  /********
	   CREATING  AND INITIALISIING THE SIMULATION VARIABLE DATA BUFFERS.
	   (UPDATED ITERATION'S VALUE ARE CALCULATED IN PHASE_ARRAY1 AND TEMP_ARRAY1 AND THEN COPIED OVER TO PHASE_ARRAY0 AND TEMP_ARRAY0)
  ********/
  
  auto PHASE_ARRAY0 = new float [MESH_Y][MESH_X];
  auto PHASE_ARRAY1 = new float [MESH_Y][MESH_X];
  auto TEMP_ARRAY0  = new float [MESH_Y][MESH_X];
  auto TEMP_ARRAY1  = new float [MESH_Y][MESH_X];
    
  typedef decltype(PHASE_ARRAY0) DTYPE;

  for (int j = 0; j < MESH_Y; ++j)
    for (int i = 0; i < MESH_X; ++i) {
      PHASE_ARRAY1[j][i] = PHASE_ARRAY0[j][i] = phase_init.init(j,i);
      TEMP_ARRAY1[j][i]  = TEMP_ARRAY0[j][i]  = temperature_init.init(j,i);
    }

  
  /********
	   CALCULATE THE PHASE FIELD AND TEMP FIELD ITERATIVELY FROM THEIR CORRESPONDING EVOLUTION EQUATIONS BASED ON KOBAYASHI MODEL.
  *********/
   
  float phase, phase_laplacian, theta, eps, term1;
  float temperature, m, ph_evol_rhs;
  float t2, p2;
    
  //bool add_noise = false;
  long time_start = time(0);
  
  for (int iter = 1 ; iter <= TOTAL_ITERATIONS; ++iter) {

    std::cout << "Iteration: " << iter << "\n";

    //add_noise = ((iter % NOISE_FREQUENCY) == 0);
    //srand(time(0));
    
    for (int y = 1; y < MESH_Y; ++y)
      for (int x = 1; x < MESH_X; ++x) {

	phase = PHASE_ARRAY0[y][x];
	temperature = TEMP_ARRAY0[y][x];
	m = (KOB::ALPHA/3.14152557)*atan(KOB::GAMMA*temperature);

	if (KOB::check_neighbors<DTYPE>(PHASE_ARRAY0, y, x)) {
	  
	  ph_evol_rhs = phase*(1.0 - phase)*(phase - 0.5 + m);
	  p2 = phase + (KOB::D_T/KOB::TAU)*ph_evol_rhs;
	  t2 = temperature - KOB::LATENT_HEAT_SLD*(p2 - phase); 
	}
	else {
	  phase_laplacian = KOB::laplacian<DTYPE>(PHASE_ARRAY0, y, x);
	  theta = KOB::theta<DTYPE>(PHASE_ARRAY0, y, x);
	  eps = KOB::epsilon(theta);
	  term1 = KOB::phase_evol_rhs_term1<DTYPE>(PHASE_ARRAY0, y, x);
	  
	  ph_evol_rhs = term1 + eps*eps*phase_laplacian + phase*(1.0 - phase)*(phase - 0.5 + m) ;
	  p2 = phase + (KOB::D_T/KOB::TAU)*ph_evol_rhs;
	  
	  t2 = temperature + KOB::D_T*KOB::THERMAL_DIFFUSIVITY*KOB::laplacian(TEMP_ARRAY0, y, x) ;
	  t2 -= KOB::LATENT_HEAT_SLD*(p2 - phase) ;                    
	}
	/*
	if (add_noise) {
	  p2 += p2*(1.0 - p2)*NOISE_AMPLITUDE*(0.5 - (float) (rand()/RAND_MAX));
	}
	*/
	PHASE_ARRAY1[y][x] = p2 < 0.0 ? 0.0 : p2 > 1.0 ? 1.0 : p2;
	TEMP_ARRAY1[y][x]  = t2;            
      }

    std::copy(&PHASE_ARRAY1[0][0], &PHASE_ARRAY1[MESH_Y - 1][MESH_X - 1], &PHASE_ARRAY0[0][0]);
    std::copy(&TEMP_ARRAY1[0][0] , &TEMP_ARRAY1[MESH_Y - 1][MESH_X - 1] , &TEMP_ARRAY0[0][0] );
    
  }

  long time_end = time(0);
  std::cout << "EXEC_TIME(in seconds) " << time_end-time_start << "\n";

  /********
	   WRITING THE OUTPUT TO A .CSV FILE IN DATA DIRECTORY FOR DATA VISUALISATION.
  *********/

  /*
  std::string phase_out_filename = "./data/phase_serial.csv";
  std::ofstream phase_output_fs (phase_out_filename, std::ios::out);
  WRITER phase_writer(0, MESH_Y, MESH_X);
  phase_writer.WRITE<DTYPE>(PHASE_ARRAY0, phase_output_fs);

  std::string temp_out_filename = "./data/temp_serial.csv";
  std::ofstream temp_output_fs (temp_out_filename, std::ios::out);
  WRITER temp_writer(0, MESH_Y, MESH_X);
  temp_writer.WRITE<DTYPE>(TEMP_ARRAY0, temp_output_fs);
  
  */
  std::string phase_out_filename = "phase_serial.vtk";
  saveAsVTK(&PHASE_ARRAY0[0][0], MESH_Y, MESH_X, phase_out_filename);
  std::cout << "Output files saved in current directory.\n";
  /*
  // write the PHASE_ARRAY0 and TEMP_ARRAY0 to files in to .vtk format for visualization using Paraview or similar tools
  std::string phase_out_filename = "phase_serial.vtk";
  std::ofstream vtkFile(phase_out_filename);
    if (!vtkFile.is_open()) {
        std::cerr << "Error: Could not open file " << phase_out_filename << " for writing.\n";
        return 0;
    }

    // Write VTK file header
    vtkFile << "# vtk DataFile Version 3.0\n";
    vtkFile << "2D simulation data\n";
    vtkFile << "ASCII\n";
    vtkFile << "DATASET STRUCTURED_POINTS\n";
    vtkFile << "DIMENSIONS " << MESH_X << " " << MESH_Y << " 1\n";
    vtkFile << "ORIGIN 0 0 0\n";
    vtkFile << "SPACING 1 1 1\n";
    vtkFile << "POINT_DATA " << (MESH_X) * (MESH_Y) << "\n";
    vtkFile << "SCALARS phase_data float 1\n";
    vtkFile << "LOOKUP_TABLE default\n";
    // Write data
    for (size_t i = 0; i < MESH_X; ++i) {
        for (size_t j = 0; j < MESH_Y; ++j) {
            vtkFile << PHASE_ARRAY0[j][i] << " ";
        }
        vtkFile << "\n";
    }
    vtkFile.close();
  std::cout << "VTK file saved as " << phase_out_filename << "\n";

  std::string temp_out_filename = "temp_serial.vtk";
  std::ofstream vtkFileTemp(temp_out_filename);
    if (!vtkFileTemp.is_open()) {
        std::cerr << "Error: Could not open file " << temp_out_filename << " for writing.\n";
        return 0;
    }

    // Write VTK file header
    vtkFileTemp << "# vtk DataFile Version 3.0\n";
    vtkFileTemp << "2D simulation data\n";
    vtkFileTemp << "ASCII\n";
    vtkFileTemp << "DATASET STRUCTURED_POINTS\n";
    vtkFileTemp << "DIMENSIONS " << MESH_X << " " << MESH_Y<< " 1\n";
    vtkFileTemp << "ORIGIN 0 0 0\n";
    vtkFileTemp << "SPACING 1 1 1\n";
    vtkFileTemp << "POINT_DATA " << (MESH_X) * (MESH_Y) << "\n";
    vtkFileTemp << "SCALARS temp_data float 1\n";
    vtkFileTemp << "LOOKUP_TABLE default\n";
    // Write data
    for (size_t i = 0; i < MESH_X; ++i) {
        for (size_t j = 0; j < MESH_Y; ++j) {
            vtkFileTemp << TEMP_ARRAY0[j][i] << " ";
        }
        vtkFileTemp << "\n";
    }
    vtkFileTemp.close();
  std::cout << "VTK file saved as " << temp_out_filename << "\n"; */



  /********
     CLEAN UP THE DYNAMICALLY ALLOCATED MEMORY.
  ********/
  delete[] PHASE_ARRAY0;
  delete[] PHASE_ARRAY1;
  delete[] TEMP_ARRAY0;
  delete[] TEMP_ARRAY1;

  /********
	   END OF PROGRAM
  ********/
  return 0;
}
