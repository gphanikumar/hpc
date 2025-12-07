#pragma once

#include <sycl/sycl.hpp>
#include <iostream>

#include "physics/Anisotropy.hpp"
#include "utilities/Input_parameters.hpp"

template<typename Anisotropy_model>
class ryokobayashi1993 {
    /// @brief Solver class for Kobayashi 1993 Phase-Field Model
    private:
        const sim_info *s_;
        const Kobayashi_Parameters *kb_;

        Anisotropy_model anisotropy_;

        float *phi; float* phi_new;
        float *temp; float* temp_new;

        sycl::queue &q_;

        void Allocate_mem();

        void Initialize_domain();

        void set_default_Bcs();

        void Apply_boundary_conditions(float *data, const enum field fc);

        void solve_phi();

        void solve_temp();

        void swap_fields(float *&d, float *&d_new);

    public:
        
        ryokobayashi1993(const sim_info *s, const Kobayashi_Parameters *k, sycl::queue &q);

        DomainBCs bc_;

        void Initiate_fields();
        
        void evolve();

        float* get_phi(){return phi;}
        float* get_temp(){return temp;}

};

#include "../src/physics/Kobayshi.tpp"