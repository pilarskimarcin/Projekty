//
// Created by Marcin on 14.12.2020.
//
// Kacper Szmajdel, 402019
// Joanna Nużka, 400561
// Marcin Pilarski, 401802
#ifndef NET_SIMULATION_FACTORY_HPP
#define NET_SIMULATION_FACTORY_HPP

#include "types.hpp"
#include "storage_types.hpp"
#include "package.hpp"
#include "config.hpp"
#include "helpers.hpp"
#include "nodes.hpp"
#include <list>
#include <iterator>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <string>


enum class NodeColor {
    UNVISITED, VISITED, VERIFIED
};
bool has_reachable_storehouse(const PackageSender* sender, std::map<const PackageSender*, NodeColor>& node_colors);

template <class Node>
        class NodeCollection{
        public:
            using container_t = typename std::list<Node>;
            using iterator = typename container_t::iterator;
            using const_iterator = typename container_t::const_iterator;
            NodeCollection(): _nodes{}{}

            NodeCollection<Node>::iterator begin(){return _nodes.begin();}
            NodeCollection<Node>::const_iterator cbegin() const {return _nodes.cbegin();}
            NodeCollection<Node>::iterator end(){return _nodes.end();}
            NodeCollection<Node>::const_iterator cend() const {return _nodes.cend();}
            NodeCollection<Node>::iterator find_by_id(ElementID id){
                return std::find_if(begin(), end(), [id](auto& node){return (node.get_id() == id);});
            }
            NodeCollection<Node>::const_iterator find_by_id(ElementID id) const{
                return std::find_if(cbegin(), cend(), [id](auto& node){return (node.get_id() == id);});
            }
            void add(Node&& node) {_nodes.push_back(std::move(node));}
            void remove_by_id(ElementID id){
                NodeCollection<Node>::const_iterator position = find_by_id(id);
                if (position != cend()){
                    _nodes.erase(position);
                }
            }
            NodeCollection<Node>& operator=(NodeCollection<Node>&& node_collection) noexcept{
                _nodes = std::move(node_collection._nodes);
                return *this;
            }
            void sort(){
                _nodes.sort([](const Node& l, const Node& r) {return l.get_id() < r.get_id();});
            }
        private:
            container_t _nodes;
        };

class Factory{
public:
    Factory();
    Factory(Factory&& factory) noexcept;
    Factory& operator=(Factory&& factory) noexcept;
    void add_ramp(Ramp&& ramp);
    void remove_ramp(ElementID id){_ramps.remove_by_id(id);}
    NodeCollection<Ramp>::iterator find_ramp_by_id(ElementID id);
    NodeCollection<Ramp>::const_iterator find_ramp_by_id(ElementID id) const;
    NodeCollection<Ramp>::iterator ramp_begin(){return _ramps.begin();}
    NodeCollection<Ramp>::const_iterator ramp_cbegin() const {return _ramps.cbegin();}
    NodeCollection<Ramp>::iterator ramp_end(){return _ramps.end();}
    NodeCollection<Ramp>::const_iterator ramp_cend() const {return _ramps.cend();}

    void add_worker(Worker&& worker);
    void remove_worker(ElementID id){remove_receiver(_workers, id);}
    NodeCollection<Worker>::iterator find_worker_by_id(ElementID id);
    NodeCollection<Worker>::const_iterator find_worker_by_id(ElementID id) const;
    NodeCollection<Worker>::iterator worker_begin() {return _workers.begin();}
    NodeCollection<Worker>::const_iterator worker_cbegin() const {return _workers.cbegin();}
    NodeCollection<Worker>::iterator worker_end() {return _workers.end();}
    NodeCollection<Worker>::const_iterator worker_cend() const {return _workers.cend();}

    void add_storehouse(Storehouse&& storehouse);
    void remove_storehouse(ElementID id){remove_receiver(_storehouses, id);}
    NodeCollection<Storehouse>::iterator find_storehouse_by_id(ElementID id);
    NodeCollection<Storehouse>::const_iterator find_storehouse_by_id(ElementID id) const;
    NodeCollection<Storehouse>::iterator storehouse_begin(){return _storehouses.begin();}
    NodeCollection<Storehouse>::const_iterator storehouse_cbegin() const {return _storehouses.cbegin();}
    NodeCollection<Storehouse>::iterator storehouse_end(){return _storehouses.end();}
    NodeCollection<Storehouse>::const_iterator storehouse_cend() const {return _storehouses.cend();}

    bool is_consistent(void);
    void do_deliveries(Time time);
    void do_package_passing(void);
    void do_work(Time time);
private:
    template <class Node>
    void remove_receiver(NodeCollection<Node>& collection, ElementID id){
        using preferences_t = std::map<IPackageReceiver*, double>;
        Node& receiver = *collection.find_by_id(id);
        Node* receiver_ptr = &receiver;
        for (auto &worker : _workers){
            const preferences_t preffs = worker.receiver_preferences_.get_preferences();
            if (preffs.find(receiver_ptr) != preffs.end()){
                worker.receiver_preferences_.remove_receiver(receiver_ptr);
            }
        }
        for (auto &ramp : _ramps){
            const preferences_t preffs = ramp.receiver_preferences_.get_preferences();
            if (preffs.find(receiver_ptr) != preffs.end()){
                ramp.receiver_preferences_.remove_receiver(receiver_ptr);
            }
        }
        collection.remove_by_id(id);
    }

    NodeCollection<Ramp> _ramps;
    NodeCollection<Worker> _workers;
    NodeCollection<Storehouse> _storehouses;
};


/////////////////////////////
/// Input - Output System
enum class ElementType {
    LOADING_RAMP, WORKER, STOREHOUSE, LINK
};

static std::unordered_map<std::string,ElementType> conversion = {
        {"LOADING_RAMP",ElementType::LOADING_RAMP},
        {"WORKER",ElementType::WORKER},
        {"STOREHOUSE",ElementType::STOREHOUSE},
        {"LINK",ElementType::LINK}};

struct ParsedLineData {
    ElementType element_type;
    std::map<std::string,std::string> parameters;
};

ParsedLineData parse_line(std::string line);

Factory load_factory_structure(std::istream& is);

void save_factory_structure(Factory& factory, std::ostringstream& os);

#endif //NET_SIMULATION_FACTORY_HPP
