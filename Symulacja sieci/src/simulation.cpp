//
// Created by Marcin on 14.12.2020.
//
// Kacper Szmajdel, 402019
// Joanna Nużka, 400561
// Marcin Pilarski, 401802
#include "simulation.hpp"

void simulate(Factory& f, TimeOffset d, std::function<void(Factory&, Time)> rf) {
    if (!f.is_consistent()) {
        throw std::logic_error("The factory is not consistent");
    }
    for (Time current_turn = 1; current_turn <= d; ++current_turn) {
        f.do_deliveries(current_turn);
        f.do_package_passing();
        f.do_work(current_turn);
        rf(f, current_turn);
    }
}