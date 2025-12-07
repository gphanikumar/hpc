

template <typename A>
ryokobayashi1993<A>::ryokobayashi1993(const sim_info* s, const Kobayashi_Parameters* k, sycl::queue &q)
    :s_(s), kb_(k),anisotropy_(k->aniso_strength), q_(q){
        phi = nullptr;
        phi_new = nullptr;
        temp = nullptr;
        temp_new = nullptr;
    }


template <typename A>
void ryokobayashi1993<A>::Allocate_mem(){
    phi      = sycl::malloc_shared<float>((s_->Nx + 2) * (s_->Ny + 2), q_);
    phi_new  = sycl::malloc_shared<float>((s_->Nx + 2) * (s_->Ny + 2), q_);
    temp     = sycl::malloc_shared<float>((s_->Nx + 2) * (s_->Ny + 2), q_);
    temp_new = sycl::malloc_shared<float>((s_->Nx + 2) * (s_->Ny + 2), q_);

    q_.memset(phi, 0, sizeof(float)*(s_->Nx + 2)*(s_->Ny + 2));
    q_.memset(phi_new, 0, sizeof(float)*(s_->Nx + 2)*(s_->Ny + 2));
    q_.memset(temp, 0, sizeof(float)*(s_->Nx + 2)*(s_->Ny + 2));
    q_.memset(temp_new, 0, sizeof(float)*(s_->Nx + 2)*(s_->Ny + 2));
}

template <typename A>
void ryokobayashi1993<A>::Initialize_domain(){
    int Nx = s_->Nx;
    int Ny = s_->Ny;
    float* phi = this->phi;
    float* temp = this->temp;

    int centerX = Nx * 0.5;
    int centerY = Ny * 0.5;

    const sim_info *s_ = this->s_;

    q_.submit([&](sycl::handler &h){
        h.parallel_for(sycl::range<2>(s_->Nx+2 , s_->Ny+2),[=](sycl::id<2> indx){
            int ix = indx[0];
            int iy = indx[1];

            int c_indx = ix * (Ny+2) + iy;

            if((ix-centerX)*(ix-centerX) + (iy-centerY)*(iy-centerY) <= 4.0f){
                phi[c_indx] = 1.0;
                temp[c_indx] = 0.1;
            }
            
        });
    }).wait();
}

template <typename A>
void ryokobayashi1993<A>::set_default_Bcs(){
    
    auto set_face_default = [](FaceBCs& f, BCType t){
        f.phi  = t;
        f.temp = t;
    };

    set_face_default(bc_.left  , BC_PERIODIC);
    set_face_default(bc_.right , BC_PERIODIC);
    set_face_default(bc_.top   , BC_PERIODIC);
    set_face_default(bc_.bottom, BC_PERIODIC);
}

template <typename A>
void ryokobayashi1993<A>::Apply_boundary_conditions(float* data, const enum field fc){
    // 1. Get Dimensions locally
    int Nx = s_->Nx;
    int Ny = s_->Ny;
    int stride = Ny + 2;
    
    BCType left_type, right_type, top_type, bottom_type;

    if (fc == PHI) {
        left_type   = bc_.left.phi;
        right_type  = bc_.right.phi;
        top_type    = bc_.top.phi;
        bottom_type = bc_.bottom.phi;
    } else {
        left_type   = bc_.left.temp;
        right_type  = bc_.right.temp;
        top_type    = bc_.top.temp;
        bottom_type = bc_.bottom.temp;
    }

    q_.submit([&](sycl::handler &h){
        h.parallel_for(sycl::range<2>(Nx+2 , Ny+2), [=](sycl::id<2> indx){
            int ix = indx[0];
            int iy = indx[1];

            // 3. Check if this is a boundary cell
            bool is_boundary = (ix == 0 || iy == 0 || ix == Nx+1 || iy == Ny+1);
            if(!is_boundary) return;

            int current_indx = ix * stride + iy;
            int inner_indx   = -1;
            int periodic_indx= -1; 
            
            // Default to Neumann if something goes wrong
            BCType type = BC_NEUMANN; 

            if(ix == 0){
                type = left_type;
                inner_indx = 1 * stride + iy; 
                periodic_indx = Nx * stride + iy;
            } else if(ix == Nx+1){
                type = right_type;
                inner_indx = Nx * stride + iy;
                periodic_indx = 1 * stride + iy;
            } else if(iy == 0){
                type = bottom_type;
                inner_indx = ix * stride + 1;
                periodic_indx = ix * stride + Ny;
            } else if(iy == Ny+1){
                type = top_type;
                inner_indx = ix * stride + Ny;
                periodic_indx = ix * stride + 1;
            }

            // 5. Apply the value
            float val = data[inner_indx];

            if(type == BC_PERIODIC){
                val = data[periodic_indx];
            }

            data[current_indx] = val;
        });
    }).wait();
}


template <typename A>
void ryokobayashi1993<A>::solve_phi(){
    int Nx = s_->Nx;
    int Ny = s_->Ny;
    
    float dx      = kb_->dx;
    float dx2     = kb_->dx2;
    float dt      = kb_->dt;
    float tau     = kb_->Tau;
    float eps_bar = kb_->epsilon_bar;
    float Te      = kb_->Te;

    float* phi = this->phi;
    float* temp = this->temp;
    float* phi_new = this->phi_new;

    auto anisotropy_kernel = this->anisotropy_;

    q_.submit([&](sycl::handler &h){
        h.parallel_for(sycl::range<2>(Nx+2 , Ny+2),[=](sycl::id<2> indx){
            int ix = indx[0];
            int iy = indx[1];

            int c_indx = ix * (Ny + 2) + iy;
            
            // Boundary check
            bool boundary = ix == 0 || iy == 0 || ix == Nx+1 || iy == Ny+1;
            if(boundary){
                phi_new[c_indx] = phi[c_indx];
                return;
            }

            // Stencil
            int left   = (ix-1) * (Ny + 2) + iy;
            int right  = (ix+1) * (Ny + 2) + iy;
            int bottom = ix * (Ny + 2) + (iy-1);
            int top    = ix * (Ny + 2) + (iy+1);

            float grad_x = (phi[left] - phi[right]) / (2.0f * dx);
            float grad_y = (phi[top] - phi[bottom]) / (2.0f * dx);

            float lap_phi = (phi[left] + phi[right] + phi[top] + phi[bottom] - 4.0f * phi[c_indx]) / dx2;

            float eps = eps_bar * anisotropy_kernel.epsilon(grad_x, grad_y);
            float term1 = (eps * eps) * lap_phi;

            float m = (0.9f / 3.14159f) * sycl::atan(10.0f * (Te - temp[c_indx]));
            float term2 = phi[c_indx] * (1.0f - phi[c_indx]) * (phi[c_indx] - 0.5f + m);

            float dphi_dt = (term1 + term2) / tau;
            
            phi_new[c_indx] = phi[c_indx] + dt * dphi_dt;
        });
    }).wait();
}

template <typename A>
void ryokobayashi1993<A>::solve_temp(){
    int Nx = s_->Nx;
    int Ny = s_->Ny;


    float dx2 = kb_->dx2;
    float dt  = kb_->dt;
    float K   = kb_->K;

    float* phi = this->phi;
    float* temp = this->temp;
    float* phi_new = this->phi_new;
    float* temp_new = this->temp_new;

    q_.submit([&](sycl::handler &h){
        h.parallel_for(sycl::range<2>(Nx+2 , Ny+2),[=](sycl::id<2> indx){
            int ix = indx[0];
            int iy = indx[1];

            int c_indx = ix * (Ny + 2) + iy;

            bool boundary = ix == 0 || iy == 0 || ix == Nx+1 || iy == Ny+1;
            if(boundary){
                temp_new[c_indx] = temp[c_indx];
                return;
            }

            int left   = (ix-1) * (Ny + 2) + iy;
            int right  = (ix+1) * (Ny + 2) + iy;
            int bottom = ix * (Ny + 2) + (iy-1);
            int top    = ix * (Ny + 2) + (iy+1);

            float lap_temp = (temp[left] + temp[right] + temp[top] + temp[bottom] - 4.0f * temp[c_indx]) / dx2;

            float dT_dt = lap_temp + K * ((phi_new[c_indx] - phi[c_indx]) / dt);
            
            temp_new[c_indx] = temp[c_indx] + dt * dT_dt;
        });
    }).wait();
}


template<typename A>
void ryokobayashi1993<A>::swap_fields(float *&d, float *&d_new){
    float *temp = d;
    d = d_new;
    d_new = temp;
}

template<typename A>
void ryokobayashi1993<A>::Initiate_fields(){
    Allocate_mem();
    Initialize_domain();
    set_default_Bcs();
}


template<typename A>
void ryokobayashi1993<A>::evolve(){
    solve_phi();

    Apply_boundary_conditions(phi, PHI);

    solve_temp();

    Apply_boundary_conditions(temp, TEMP);

    swap_fields(phi, phi_new);
    swap_fields(temp, temp_new);
}
