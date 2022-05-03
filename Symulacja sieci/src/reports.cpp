//
// Created by Marcin on 14.12.2020.
//
// Joanna Nużka, 400561
// Kacper Szmajdel, 402019
// Marcin Pilarski, 401802
#include "reports.hpp"

using std::endl;

bool SpecificTurnsReportNotifier::should_generate_report(Time t) const {
    auto it = _turns.find(t);
    return (it != _turns.end());
}

void generate_structure_report(const Factory& f, std::ostream& os) {
    os << "\n== LOADING RAMPS ==\n" << endl;
    for (auto it = f.ramp_cbegin(); it != f.ramp_cend(); ++it) {
        os << "LOADING RAMP #" << it->get_id() << endl;
        os << "  Delivery interval: " << it->get_delivery_interval() << endl;
        os << "  Receivers:" << endl;
        receivers_to_report(it, os);
        os << endl;
    }
    os << "\n== WORKERS ==\n" << endl;
    for (auto it = f.worker_cbegin(); it != f.worker_cend(); ++it) {
        os << "WORKER #" << it->get_id() << endl;
        os << "  Processing time: " << it->get_processing_duration() << endl;
        switch (it->get_queue()->get_queue_type()) {
            case PackageQueueType::FIFO:
                os << "  Queue type: " << "FIFO" << endl;
                break;
            case PackageQueueType::LIFO:
                os << "  Queue type: " << "LIFO" << endl;
                break;
        }
        os << "  Receivers:" << endl;
        receivers_to_report(it, os);
        os << endl;
    }
    os << "\n== STOREHOUSES ==\n" << endl;
    for (auto it = f.storehouse_cbegin(); it != f.storehouse_cend(); ++it){
        os << "STOREHOUSE #" << it->get_id() << "\n" << endl;
    }
}

void generate_simulation_turn_report(const Factory& f, std::ostream& os, Time t) {
    os << "=== [ Turn: " << std::to_string(t) << " ] ===" << endl;
    os << "\n== WORKERS ==\n" << endl;
    for (auto it = f.worker_cbegin(); it != f.worker_cend(); ++it) {
        os << "WORKER #" << it->get_id() << endl;
        os << "  PBuffer: ";
        const std::optional<Package>& buffer = it->get_processing_buffer();
        if (buffer.has_value()){
            os << "#" << buffer->get_id() << " (pt = " << it->get_package_processing_start_time() << ")" << endl;
        } else {
            os << "(empty)" << endl;
        }
        os << "  Queue: ";
        IPackageQueue* queue = it->get_queue();
        if(queue->empty()){
            os << "(empty)" << endl;
        } else {
            for (auto it_queue = queue->begin(), it_end = --queue->end(); it_queue != it_end; ++it_queue){
                os << "#" << it_queue->get_id() << ", ";
            }
            os << "#" << (--queue->end())->get_id() << endl;
        }
        os << "  SBuffer: ";
        const std::optional<Package>& send_buffer = it->get_sending_buffer();
        if (send_buffer.has_value()){
            os << "#" << send_buffer->get_id() << endl;
        } else {
            os << "(empty)" << endl;
        }
        os << endl;
    }
    os << "\n== STOREHOUSES ==\n" << endl;
    for (auto it = f.storehouse_cbegin(); it != f.storehouse_cend(); ++it){
        os << "STOREHOUSE #" << it->get_id() << endl;
        os << "  Stock: ";
        if (it->begin() == it->end()){
            os << "(empty)" << endl;
        } else {
            for (auto it_stock = it->begin(), it_end = --it->end(); it_stock != it_end ; ++it_stock) {
                os << "#" << it_stock->get_id() << ", ";
            }
            os << "#" << (--it->end())->get_id() << endl;
        }
        os << endl;
    }
}
