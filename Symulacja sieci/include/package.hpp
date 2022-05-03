// Marcin Pilarski, 401802
// Kacper Szmajdel, 402019
// Joanna Nużka, 400561
#ifndef NET_SIMULATION_PACKAGE_HPP
#define NET_SIMULATION_PACKAGE_HPP

#include "types.hpp"

class Package
{
    static IDs_set assigned_IDs;
    static IDs_set freed_IDs;
    const ElementID incorrect_id_ = 0;
    ElementID id_;
public:
    Package();
    explicit Package(ElementID id);
    Package(const Package&) = delete;
    Package& operator=(const Package&) = delete;
    Package(Package&& package) noexcept;
    Package& operator=(Package&& package) noexcept;
    ~Package();
    ElementID get_id() const { return id_; }
    void set_incorrect_id() { id_ = incorrect_id_; }
};

#endif //NET_SIMULATION_PACKAGE_HPP
