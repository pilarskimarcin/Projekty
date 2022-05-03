//
// Created by Marcin on 14.12.2020.
//
// Marcin Pilarski, 401802
// Kacper Szmajdel, 402019
// Joanna Nużka, 400561

#ifndef NET_SIMULATION_NODES_HPP
#define NET_SIMULATION_NODES_HPP

#include "types.hpp"
#include <memory>
#include <optional>
#include "storage_types.hpp"
#include "package.hpp"
#include "config.hpp"
#include "helpers.hpp"
#include <map>

enum class ReceiverType{
    WORKER, STOREHOUSE
};


class IPackageReceiver{
public:
    virtual void receive_package(Package&& p) = 0;
    virtual IPackageStockpile::const_iterator begin() const = 0;
    virtual IPackageStockpile::const_iterator cbegin() const = 0;
    virtual IPackageStockpile::const_iterator end() const = 0;
    virtual IPackageStockpile::const_iterator cend() const = 0;
    virtual ElementID get_id() const = 0;
#if (defined EXERCISE_ID && EXERCISE_ID != EXERCISE_ID_NODES)
    virtual ReceiverType get_receiver_type() const = 0;
#endif
};


class Storehouse: public IPackageReceiver{
public:
    Storehouse(ElementID id, std::unique_ptr<IPackageStockpile> d = std::make_unique<PackageQueue>(PackageQueueType::FIFO));
    void receive_package(Package&& p) override {_d->push(std::move(p));}
    IPackageStockpile::const_iterator begin() const override {return _d->begin();}
    IPackageStockpile::const_iterator cbegin() const  override {return _d->cbegin();}
    IPackageStockpile::const_iterator end() const override {return _d->end();}
    IPackageStockpile::const_iterator cend() const override {return _d->cend();}
    ElementID get_id() const override {return _id;}
#if (defined EXERCISE_ID && EXERCISE_ID != EXERCISE_ID_NODES)
    ReceiverType get_receiver_type() const override {return _type;}
#endif
private:
    ReceiverType _type = ReceiverType::STOREHOUSE;
    ElementID _id;
    std::unique_ptr<IPackageStockpile> _d;
};


class ReceiverPreferences {
    using preferences_t = std::map<IPackageReceiver*, double>;
    using const_iterator = preferences_t::const_iterator;
    preferences_t preferences_;
    const ProbabilityGenerator& probability_generator_;
public:
    explicit ReceiverPreferences(const ProbabilityGenerator& pg = probability_generator) : probability_generator_(pg) {}
    void add_receiver(IPackageReceiver* r);
    void remove_receiver(IPackageReceiver* r);
    IPackageReceiver* choose_receiver();
    const preferences_t& get_preferences() const { return preferences_; }

    const_iterator begin() { return preferences_.begin(); }
    const_iterator cbegin() { return preferences_.cbegin(); }
    const_iterator end() { return preferences_.end(); }
    const_iterator cend() { return preferences_.cend(); }
};


class PackageSender{
    std::optional<Package> buffer_ = std::nullopt;
public:
    ReceiverPreferences receiver_preferences_;
    explicit PackageSender(const ProbabilityGenerator& pg = probability_generator) : receiver_preferences_(ReceiverPreferences(pg)){}
    PackageSender(PackageSender&& other) noexcept : receiver_preferences_(other.receiver_preferences_) {}
    PackageSender(const PackageSender& other) : receiver_preferences_(other.receiver_preferences_) {}
    void send_package();
    const std::optional<Package>& get_sending_buffer() const { return buffer_; }
protected:
    void push_package (Package&& p) { buffer_ = std::move(p); }
};


class Ramp: public PackageSender{
    ElementID id_;
    TimeOffset di_;
public:
    Ramp(ElementID id, TimeOffset di) : id_(id), di_(di) {}
    void deliver_goods(Time t);
    TimeOffset get_delivery_interval() const{ return di_; }
    ElementID get_id() const{ return id_; }
};


class Worker: public IPackageReceiver, public PackageSender {
public:
    Worker(ElementID id, TimeOffset pd, std::unique_ptr<IPackageQueue> q);
    void do_work (Time t);
    TimeOffset get_processing_duration () const {return _pd;}
    Time get_package_processing_start_time() const {return _package_processing_start_time;}
    void receive_package(Package&& p) override {_q->push(std::move(p));}
    IPackageStockpile::const_iterator begin() const override {return _q->begin();}
    IPackageStockpile::const_iterator cbegin() const  override {return _q->cbegin();}
    IPackageStockpile::const_iterator end() const override {return _q->end();}
    IPackageStockpile::const_iterator cend() const override {return _q->cend();}
    ElementID get_id() const override {return _id;}
    IPackageQueue *get_queue() const {return &(*_q);}
    const std::optional<Package>& get_processing_buffer() const {return _processing_buffer;}
#if (defined EXERCISE_ID && EXERCISE_ID != EXERCISE_ID_NODES)
    ReceiverType get_receiver_type() const override {return _type;}
#endif
private:
    ReceiverType _type = ReceiverType::WORKER;
    ElementID _id;
    TimeOffset _pd;
    Time _package_processing_start_time = 0;
    std::unique_ptr<IPackageQueue> _q;
    std::optional<Package> _processing_buffer;
};

#endif //NET_SIMULATION_NODES_HPP
