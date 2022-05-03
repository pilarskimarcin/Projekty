//
// Created by Marcin on 14.12.2020.
//
// Marcin Pilarski, 401802
// Kacper Szmajdel, 402019
// Joanna Nużka, 400561
#include "nodes.hpp"
#include <stdexcept>

Storehouse::Storehouse(ElementID id, std::unique_ptr<IPackageStockpile> d):_d(std::move(d)) {
    _id = id;}


Worker::Worker(ElementID id, TimeOffset pd, std::unique_ptr<IPackageQueue> q) : _q(std::move(q)) {
    _id = id;
    _pd = pd;
}

void Worker::do_work(Time t) {
    if(!_processing_buffer && !_q->empty()) {
        _processing_buffer = _q->pop();
        _package_processing_start_time = t;
    }
    if(_processing_buffer && t - _package_processing_start_time >= _pd - 1) {
        PackageSender::push_package(std::move(*_processing_buffer));
        _package_processing_start_time = 0;
        if(_processing_buffer->get_id() == 0){
            _processing_buffer.reset();
        }
    }
}


void ReceiverPreferences::add_receiver(IPackageReceiver *r) {
    double sum = preferences_.size() + 1.;
    for (auto & preference : preferences_)
        preference.second = 1. / sum;
    preferences_[r] = 1. / sum;
}

void ReceiverPreferences::remove_receiver(IPackageReceiver *r) {
    double sum = preferences_.size() - 1.;
    preferences_.erase(r);
    for (auto & preference : preferences_)
        preference.second = 1. / sum;
}

IPackageReceiver *ReceiverPreferences::choose_receiver() {
    double probability = probability_generator_();
    double sum = 0;
    for (auto & preference : preferences_) {
        if (probability >= sum && probability <= sum + preference.second) return preference.first;
        else sum += preference.second;
    }
    throw std::logic_error("Choosing a receiver failed");
}

void PackageSender::send_package() {
    if(buffer_ != std::nullopt) {
        IPackageReceiver* receiver = receiver_preferences_.choose_receiver();
        receiver->receive_package(std::move(*buffer_));
        buffer_ = std::nullopt;
    }
}

void Ramp::deliver_goods(Time t) {
    if(!((t-1)%di_)) {
        push_package(Package());
    }
}

