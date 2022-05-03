//
// Created by Marcin on 14.12.2020.
//
// Joanna Nużka, 400561
// Kacper Szmajdel, 402019
// Marcin Pilarski, 401802
#ifndef NET_SIMULATION_REPORTS_HPP
#define NET_SIMULATION_REPORTS_HPP


#include "factory.hpp"

class SpecificTurnsReportNotifier{
public:
    SpecificTurnsReportNotifier(std::set<Time> turns) {_turns = std::move(turns);}
    bool should_generate_report(Time t) const;
private:
    std::set<Time> _turns;
};
class IntervalReportNotifier{
public:
    IntervalReportNotifier(TimeOffset to) {_to = to;}
    bool should_generate_report(Time t) const {return (t % _to == 1);}
private:
    TimeOffset _to;
};

void generate_structure_report(const Factory& f, std::ostream& os);
void generate_simulation_turn_report(const Factory& f, std::ostream& os, Time t);

template<class Node>
void receivers_to_report(std::_List_const_iterator<Node> it, std::ostream& os){
    const std::map<IPackageReceiver *, double> preferences = it->receiver_preferences_.get_preferences();
    std::list<ElementID> worker_receivers{};
    std::list<ElementID> storehouse_receivers{};
    for (auto preference : preferences) {
        switch (preference.first->get_receiver_type()) {
            case ReceiverType::WORKER:
                worker_receivers.push_back(preference.first->get_id());
                break;
            case ReceiverType::STOREHOUSE:
                storehouse_receivers.push_back(preference.first->get_id());
                break;
        }
    }
    worker_receivers.sort();
    storehouse_receivers.sort();
    for (ElementID id : storehouse_receivers){
        os <<  "    storehouse #" << id << std::endl;
    }
    for (ElementID id : worker_receivers){
        os <<  "    worker #" << id << std::endl;
    }
}
#endif //NET_SIMULATION_REPORTS_HPP
