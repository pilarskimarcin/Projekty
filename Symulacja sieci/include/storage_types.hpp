//
// Created by Marcin on 14.12.2020.
//
// Marcin Pilarski, 401802
// Kacper Szmajdel, 402019
// Joanna Nużka, 400561
#ifndef NET_SIMULATION_STORAGE_TYPES_HPP
#define NET_SIMULATION_STORAGE_TYPES_HPP
#include <list>
#include <iterator>
#include "package.hpp"

enum class PackageQueueType{
    FIFO, LIFO
};
class IPackageStockpile{
public:
    using const_iterator = std::list<Package>::const_iterator;
    virtual void push(Package&&) = 0;
    virtual const_iterator begin() = 0;
    virtual const_iterator cbegin() const = 0;
    virtual const_iterator end() = 0;
    virtual const_iterator cend() const = 0;
    virtual std::size_t size() const = 0;
    virtual bool empty() const = 0;
    virtual ~IPackageStockpile() = default;
};

class IPackageQueue: public IPackageStockpile{
public:
    virtual Package pop() = 0;
    virtual PackageQueueType get_queue_type() const = 0;
};
class PackageQueue: public IPackageQueue{
public:
    PackageQueue(PackageQueueType q_type) {_queue_type = q_type;}
    void push(Package&& package) override {_products_list.emplace_back(std::move(package));}
    IPackageStockpile::const_iterator begin() override {return  _products_list.begin();}
    IPackageStockpile::const_iterator cbegin() const override {return _products_list.cbegin();}
    IPackageStockpile::const_iterator end() override {return _products_list.end();}
    IPackageStockpile::const_iterator cend() const override {return _products_list.cend();}
    std::size_t size() const override  {return _products_list.size();}
    bool empty() const override {return _products_list.empty();}
    Package pop() override;
    PackageQueueType get_queue_type() const override {return _queue_type;}
private:
    PackageQueueType _queue_type;
    std::list<Package> _products_list;
};

#endif //NET_SIMULATION_STORAGE_TYPES_HPP
