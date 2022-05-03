// Marcin Pilarski, 401802
// Kacper Szmajdel, 402019
// Joanna Nużka, 400561
#include "package.hpp"
#include <stdexcept>

IDs_set Package::assigned_IDs = {};
IDs_set Package::freed_IDs = {};


Package::Package(Package&& package) noexcept
{
    id_ = package.get_id();
    package.set_incorrect_id();
    package.~Package();
}

Package& Package::operator=(Package&& package) noexcept
{
    assigned_IDs.erase(id_);
    freed_IDs.insert(id_);
    id_ = package.get_id();
    package.set_incorrect_id();
    return *this;
}

Package::Package(ElementID id) : id_(id)
{
    if(assigned_IDs.find(id_) != assigned_IDs.end())
        throw std::logic_error("ID already in use");
    if(id_ <= 0)
        throw std::logic_error("ID can only be a positive integer");
    assigned_IDs.insert(id_);
}

Package::Package()
{
    if (freed_IDs.empty())
    {
        if(assigned_IDs.empty()) id_ = 1;
        else id_ = *assigned_IDs.rbegin()+1;
        assigned_IDs.insert(id_);
    }
    else
    {
        id_ = *freed_IDs.begin();
        freed_IDs.erase(id_);
        assigned_IDs.insert(id_);
    }
}

Package::~Package()
{
    if(id_ != incorrect_id_) {
        assigned_IDs.erase(id_);
        freed_IDs.insert(id_);
    }
}
