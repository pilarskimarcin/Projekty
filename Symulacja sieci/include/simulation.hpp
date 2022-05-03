//
// Created by Marcin on 14.12.2020.
//
// Kacper Szmajdel, 402019
// Joanna Nużka, 400561
// Marcin Pilarski, 401802
#ifndef NET_SIMULATION_SIMULATION_HPP
#define NET_SIMULATION_SIMULATION_HPP

#include "reports.hpp"

void simulate(Factory& f, TimeOffset d, std::function<void(Factory&, Time)> rf);

#endif //NET_SIMULATION_SIMULATION_HPP
