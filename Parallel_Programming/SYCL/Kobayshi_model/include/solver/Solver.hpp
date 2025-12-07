#pragma once

#include "physics/Kobayshi.hpp"
#include "utilities/IO.hpp"

template <typename Anisotropy>
class solver{
    private:
        ryokobayashi1993<Anisotropy> &kobayshi_;
        const sim_info *s_;
        const Kobayashi_Parameters *kb_;

        IO *io_;

    public:
        solver(ryokobayashi1993<Anisotropy> &kobayshi, const sim_info *s, const Kobayashi_Parameters *kb, IO *io);

        void time_stepping();
};

#include "../src/solver/Solver.tpp"