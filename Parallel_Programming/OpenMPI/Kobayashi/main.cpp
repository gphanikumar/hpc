#include <bits/stdc++.h>
#include <mpi.h>
#include "./SOL_INIT.hpp"
#include "./WRITER.hpp"
#include "./KOBAYASHI.hpp"

int main(int argc, char** argv) {
  
  /********
	   MPI PARAMETERS
  ********/
  
  int DESTINATION, SOURCE;
  int size;
  int rank;
  MPI_Status status;
  int TAG1, TAG2, TAG3, TAG4;
  
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);


  /*********
	    INITIALIZE SIMULATION VARIABLES
  *********/
    
  int const MESH_X = 1024;
  int const MESH_Y = MESH_X/size;
  int const TOTAL_ITERATIONS = 3072;
  
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
	   CREATING AND INITIALISING HE SIMULATION VARIABLE DATA BUFFERS.
	   ADDITIONAL TWO ARRAYS OF DATA FOR EXCHANGE OF BOUNDARY VALUES BETWEEN THE DIFFERENT PROCESSES ARE TACKED ONTO THE SIMULATION VARIABLE DATA BUFFERS.
	   (UPDATED ITERATION'S VALUE ARE CALCULATED IN PHASE_ARRAY1 AND TEMP_ARRAY1 AND THEN COPIED OVER TO PHASE_ARRAY0 AND TEMP_ARRAY0)
  ********/
  
  int const DATA_ALLOC_SIZE_X = MESH_X + 2;
  int const DATA_ALLOC_SIZE_Y = MESH_Y + 2;
  
  auto PHASE_ARRAY0 = new float [DATA_ALLOC_SIZE_Y][DATA_ALLOC_SIZE_X];
  auto PHASE_ARRAY1 = new float [DATA_ALLOC_SIZE_Y][DATA_ALLOC_SIZE_X];
  
  auto TEMP_ARRAY0 = new float [DATA_ALLOC_SIZE_Y][DATA_ALLOC_SIZE_X];
  auto TEMP_ARRAY1 = new float [DATA_ALLOC_SIZE_Y][DATA_ALLOC_SIZE_X];
  
  typedef decltype(PHASE_ARRAY0) DTYPE;
  
  float j_global;
  for (int j = 0; j < DATA_ALLOC_SIZE_Y; ++j)
    for (int i = 0; i < DATA_ALLOC_SIZE_X; ++i) {
      j_global = rank*DATA_ALLOC_SIZE_Y + j;
      PHASE_ARRAY1[j][i] = PHASE_ARRAY0[j][i] = phase_init.init(j_global,i);
      TEMP_ARRAY1[j][i] = TEMP_ARRAY0[j][i]  = temperature_init.init(j_global,i);
    }


  /********
	   CALCULATE THE PHASE FIELD AND TEMP FIELD ITERATIVELY FROM THEIR CORRESPONDING EVOLUTION EQUATIONS BASED ON KOBAYASHI MODEL.
  *********/

  int iter_range_x = DATA_ALLOC_SIZE_X - 2;
  int iter_range_y = DATA_ALLOC_SIZE_Y - 2;
  
  float phase, phase_laplacian, theta, eps, term1;
  float temperature, m, ph_evol_rhs;
  float t2, p2;
  
  bool add_noise = false;
  int TOP_PARTNER    = rank == 0      ? MPI_PROC_NULL : rank - 1;
  int BOTTOM_PARTNER = rank == size - 1 ? MPI_PROC_NULL : rank + 1;
  
  long time_start = time(0);
  
  for (int iter = 1; iter <= TOTAL_ITERATIONS; ++iter) {
    if(rank == 0) std::cout << "ITERATION: " << iter << "\n";
    
    /*
    add_noise = ((iter % NOISE_FREQUENCY) == 0);
    srand(time(0));
    */
    MPI_Barrier(MPI_COMM_WORLD);
    
    for (int y = 1 ; y <= iter_range_y ; ++y)
      for (int x = 1; x <= iter_range_x ; ++x){
	
	phase = PHASE_ARRAY0[y][x];
	temperature = TEMP_ARRAY0[y][x];
	m = (KOB::ALPHA/3.14152557)*atan( KOB::GAMMA * temperature);
	
	if (KOB::check_neighbors<DTYPE>(PHASE_ARRAY0, y, x)) {
	  ph_evol_rhs = phase*(1.0 - phase)*(phase - 0.5 + m);
	  p2 = phase + (KOB::D_T/KOB::TAU)*ph_evol_rhs;
	  t2 = temperature - KOB::LATENT_HEAT_SLD*(p2 - phase);
	}  
	else {
	  phase_laplacian = KOB::laplacian<DTYPE>(PHASE_ARRAY0, y, x);
	  theta = KOB::theta<DTYPE>(PHASE_ARRAY0,y,x);
	  eps = KOB::epsilon(theta);
	  term1 = KOB::phase_evol_rhs_term1<DTYPE>(PHASE_ARRAY0, y, x);
	  
	  ph_evol_rhs = term1 + eps*eps*phase_laplacian + phase*(1.0 - phase)*(phase - 0.5 + m);
	  p2 = phase + (KOB::D_T/KOB::TAU)*ph_evol_rhs;
	  
	  t2 = temperature + KOB::D_T*KOB::THERMAL_DIFFUSIVITY*KOB::laplacian(TEMP_ARRAY0, y, x);
	  t2 -= KOB::LATENT_HEAT_SLD*(p2 - phase);
	}
	/*
	if (add_noise) {
	  p2 += p2*(1.0 - p2)*NOISE_AMPLITUDE*(0.5 - (float) (rand()/RAND_MAX));
	}
	*/
	PHASE_ARRAY1[y][x] = p2 < 0.0 ? 0.0 : p2 > 1.0 ? 1.0 : p2;
	TEMP_ARRAY1[y][x]  = t2;
      }
    
    
    /********
	     SENDING AND RECEIVING BOUNDARY VALUES BETWEEN PROCESSES.
    ********/
    
    TAG1 = iter;
    TAG2 = TAG1 + 1;
    TAG3 = TAG1 + 2;
    TAG4 = TAG1 + 3;
    
    MPI_Send(PHASE_ARRAY0[1], DATA_ALLOC_SIZE_X, MPI_FLOAT, TOP_PARTNER, TAG1, MPI_COMM_WORLD);
    MPI_Recv(PHASE_ARRAY1[DATA_ALLOC_SIZE_Y - 1], DATA_ALLOC_SIZE_X, MPI_FLOAT, BOTTOM_PARTNER, TAG1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    MPI_Send(PHASE_ARRAY0[DATA_ALLOC_SIZE_Y - 2], DATA_ALLOC_SIZE_X, MPI_FLOAT, BOTTOM_PARTNER, TAG2, MPI_COMM_WORLD);
    MPI_Recv(PHASE_ARRAY1[0], DATA_ALLOC_SIZE_X, MPI_FLOAT, TOP_PARTNER, TAG2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    MPI_Send(TEMP_ARRAY0[1], DATA_ALLOC_SIZE_X, MPI_FLOAT, TOP_PARTNER, TAG3, MPI_COMM_WORLD);
    MPI_Recv(TEMP_ARRAY1[DATA_ALLOC_SIZE_Y - 1], DATA_ALLOC_SIZE_X, MPI_FLOAT, BOTTOM_PARTNER, TAG3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    
    MPI_Recv(TEMP_ARRAY1[0], DATA_ALLOC_SIZE_X, MPI_FLOAT, TOP_PARTNER, TAG4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Send(TEMP_ARRAY0[DATA_ALLOC_SIZE_Y - 2], DATA_ALLOC_SIZE_X, MPI_FLOAT, BOTTOM_PARTNER, TAG4, MPI_COMM_WORLD);


    std::copy(&PHASE_ARRAY1[0][0], &PHASE_ARRAY1[DATA_ALLOC_SIZE_Y - 1][DATA_ALLOC_SIZE_X - 1], &PHASE_ARRAY0[0][0]);
    std::copy(&TEMP_ARRAY1[0][0], &TEMP_ARRAY1[DATA_ALLOC_SIZE_Y - 1][DATA_ALLOC_SIZE_X - 1], &TEMP_ARRAY0[0][0]);
  }
  
  //MPI_Barrier(MPI_COMM_WORLD);

  long time_end = time(0);
  if (rank == 0) std::cout << "EXEC_TIME(in seconds) " << time_end - time_start << "\n";
  

  MPI_Finalize();

  /********
	   END OF PROGRAM
  ********/
  return 0;
}
