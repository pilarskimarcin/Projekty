// Kacper Szmajdel, 402019
// Joanna Nużka, 400561
// Marcin Pilarski, 401802
#ifndef NET_SIMULATION_TYPES_HPP
#define NET_SIMULATION_TYPES_HPP

#include <set>
#include <functional>
using ElementID = int;
using IDs_set = std::set<ElementID>;
using ProbabilityGenerator = std::function<double()>;
using Time = int;
using TimeOffset = int;

#endif //NET_SIMULATION_TYPES_HPP
