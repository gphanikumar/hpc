#include <bits/stdc++.h>

namespace KOB{
  float D_X = 0.03;
  float D_Y = D_X;
  float D_T = 1e-4;
  float EPS_BAR = 0.01;
  float ALPHA = 0.92;
  float GAMMA = 10;
  float DELTA = 0.6;
  float J = 4;
  float TAU = 3.0e-4;
  float THETA_0 = 0.01;
  float THERMAL_DIFFUSIVITY = 1.0;
  float LATENT_HEAT_SLD = 1.52;
  

  /*********
	    AN OPTIMIZATION FUNCTION TO REDUCE CALCULATIONS IF THE LAPLACIAN IS ZERO.
   *********/
  
  template<typename TYPE>
  static inline bool check_neighbors(TYPE FIELD, int j, int i) {
    bool nbh = true;
    float c = FIELD[j][i];
    nbh = nbh && (FIELD[j+1][i] == c) && (FIELD[j-1][i] == c) && (FIELD[j][i+1] == c) && (FIELD[j][i-1] == c);
    return nbh;
  }
  
  
  /*********
	    LAPLACIAN MATRIX AROUND THE CENTRE POINT:
	    00  01  00
	    01  -4  01
	    00  01  00
   *********/
  
  template<typename T>
  static inline float laplacian(T FIELD, int j, int i) {
    float laplacian = 0.0;
    
    laplacian += FIELD[j][i+1];
    laplacian += FIELD[j+1][i];
    laplacian += FIELD[j][i-1];
    laplacian += FIELD[j-1][i];
    
    laplacian -= 4.0*FIELD[j][i];
    
    return laplacian/(D_X*D_Y);
  }
  
  template<typename T>
    static inline float grad_x(T FIELD, int j, int i) {
      return (FIELD[j][i+1] - FIELD[j][i-1])/(2.0*D_X);
  }

  template<typename T>
    static inline float grad_y(T FIELD, int j, int i) {
      return (FIELD[j+1][i] - FIELD[j-1][i])/(2.0*D_Y);
  }

  template<typename T>
  static inline float theta(T FIELD, int j, int i) {
    float dFdx = (FIELD[j][i+1] - FIELD[j][i-1]);
    float dFdy = (FIELD[j+1][i] - FIELD[j-1][i]);
    
    if (dFdx == 0.0) return 1.57079;
    else return atan( dFdy/dFdx );
  }

  static inline float epsilon(float theta) {
    return (EPS_BAR*(1.0 + DELTA*cos(J*(theta - THETA_0))));
  }
  
  static inline float DepsilonDtheta(float theta) {
    return -EPS_BAR*DELTA*J*sin(J*(theta - THETA_0));
  }

  template<typename T>
  static inline float epsDepsDtheta(T FIELD, int j,int i) {
    float th = theta(FIELD,j,i);
    return epsilon(th)*DepsilonDtheta(th);
  }

  template<typename T>
  static inline float phase_evol_rhs_term1(T FIELD, int j,int i) { 
    float tmp1, tmp2, term11, term12 ;
    
    tmp1 = grad_x(FIELD, j+1, i)*epsDepsDtheta(FIELD, j+1, i);
    tmp2 = grad_x(FIELD, j-1, i)*epsDepsDtheta(FIELD, j-1, i);
    term11 = (tmp1 - tmp2)/(2.0*D_Y);
    
    tmp1 = grad_y(FIELD, j, i+1)*epsDepsDtheta(FIELD, j, i+1);
    tmp2 = grad_y(FIELD, j, i-1)*epsDepsDtheta(FIELD, j, i-1);
    term12 = (tmp1 - tmp2)/(2.0*D_X);
    
    return (term11 - term12);
  }
  
}
