#include <iostream>
#include <variant>
#include <type_traits>

#include "utilities/Input_parameters.hpp"
#include "utilities/IO.hpp"
#include "physics/Anisotropy.hpp"
#include "physics/Kobayshi.hpp"
#include "solver/Solver.hpp"

using PhysicsVariant = std::variant<FourFold, SixFold>;

int main(){

    mkdir("DATA", 0777);

    // Select device
    sycl::queue q(sycl::default_selector_v);
    std::cout<<"Running on: "<<q.get_device().get_info<sycl::info::device::name>()<<"\n";

    sim_info sm;
    sm.Nx = 300;
    sm.Ny = 300;
    sm.time_steps = 5000;
    sm.saveT = 100;

    Kobayashi_Parameters kbm;
    kbm.Tau = 0.0003f;
    kbm.K = 1.6f;
    kbm.aniso_strength = 0.04;
    kbm.epsilon_bar = 0.01f;
    kbm.dt = 0.0001f;
    kbm.dx = 0.03f;
    kbm.dx2 = kbm.dx * kbm.dx;
    kbm.Te = 1.0f;

    std::string fn = "Output";
    std::string anis_model = "6fold"; // 4fold or "6fold"

    PhysicsVariant anisotropy = (anis_model == "4fold") 
        ? PhysicsVariant(FourFold(kbm.aniso_strength))
        : PhysicsVariant(SixFold(kbm.aniso_strength));


    std::visit([&](auto&& model) {
        using ModelType = std::decay_t<decltype(model)>;
        ryokobayashi1993<ModelType> k_model(&sm, &kbm, q);

        k_model.Initiate_fields();
        IO io(fn, sm.Nx, sm.Ny);
        solver<ModelType> solve(k_model, &sm, &kbm, &io);
        solve.time_stepping();

    }, anisotropy);

    std::cout<<"Simulation completed.\n";

}
