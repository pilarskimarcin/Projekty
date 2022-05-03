//
// Created by Marcin on 14.12.2020.
//
// Kacper Szmajdel, 402019
// Joanna Nużka, 400561
// Marcin Pilarski, 401802
#include "factory.hpp"

Factory::Factory() {
    _ramps = NodeCollection<Ramp>();
    _workers = NodeCollection<Worker>();
    _storehouses = NodeCollection<Storehouse>();
}

NodeCollection<Ramp>::iterator Factory::find_ramp_by_id(ElementID id) {
    return _ramps.find_by_id(id);
}

NodeCollection<Ramp>::const_iterator Factory::find_ramp_by_id(ElementID id) const {
    return _ramps.find_by_id(id);
}

NodeCollection<Worker>::iterator Factory::find_worker_by_id(ElementID id) {
    return _workers.find_by_id(id);
}

NodeCollection<Worker>::const_iterator Factory::find_worker_by_id(ElementID id) const {
    return _workers.find_by_id(id);
}

NodeCollection<Storehouse>::iterator Factory::find_storehouse_by_id(ElementID id) {
    return _storehouses.find_by_id(id);
}

NodeCollection<Storehouse>::const_iterator Factory::find_storehouse_by_id(ElementID id) const {
    return _storehouses.find_by_id(id);
}

void Factory::do_deliveries(Time time) {
    for(auto &ramp : _ramps){
        ramp.deliver_goods(time);
    }
}

void Factory::do_package_passing(void) {
    for(auto &worker : _workers){
        worker.send_package();
    }
    for(auto &ramp : _ramps){
        ramp.send_package();
    }
}

void Factory::do_work(Time time) {
    for(auto &worker : _workers){
        worker.do_work(time);
    }
}

bool has_reachable_storehouse(const PackageSender* sender, std::map<const PackageSender*, NodeColor>& node_colors){
    if(node_colors[sender] == NodeColor::VERIFIED){
        return true;
    }
    node_colors[sender] = NodeColor::VISITED;
    if (sender->receiver_preferences_.get_preferences().empty()){
        throw std::logic_error("The sender has no defined receivers");
    }
    bool has_at_least_one_receiver_other_than_itself = false;
    for (auto &element : sender->receiver_preferences_.get_preferences()){
        IPackageReceiver* receiver = element.first;
        if (receiver->get_receiver_type() == ReceiverType::STOREHOUSE){
            has_at_least_one_receiver_other_than_itself = true;
        } else if (receiver->get_receiver_type() == ReceiverType::WORKER){
            auto worker_ptr = dynamic_cast<Worker*>(receiver);
            auto sendrecv_ptr = dynamic_cast<PackageSender*>(worker_ptr);
            if (sendrecv_ptr == sender){
                continue;
            }
            has_at_least_one_receiver_other_than_itself = true;
            if (node_colors[sendrecv_ptr] == NodeColor::UNVISITED){
                has_reachable_storehouse(sendrecv_ptr, node_colors);
            }
        }
    }
    node_colors[sender] = NodeColor::VERIFIED;
    if (has_at_least_one_receiver_other_than_itself){
        return true;
    } else {
        throw std::logic_error("No storehouse is reachable");
    }
}

bool Factory::is_consistent(void) {
    std::map<const PackageSender*, NodeColor> colours;
    for (const auto &ramp : _ramps){
        colours[&ramp] = NodeColor::UNVISITED;
    }
    for (const auto &worker : _workers) {
        colours[&worker] = NodeColor::UNVISITED;
    }
    try {
        for (const auto &ramp : _ramps){
            has_reachable_storehouse(&ramp, colours);
        }
    }
    catch (std::logic_error&) {
        return false;
    }
    return true;
}

Factory& Factory::operator=(Factory &&factory) noexcept {
    _ramps = std::move(factory._ramps);
    _workers = std::move(factory._workers);
    _storehouses = std::move(factory._storehouses);
    return *this;
}

Factory::Factory(Factory&& factory)  noexcept {
    _ramps = std::move(factory._ramps);
    _workers = std::move(factory._workers);
    _storehouses = std::move(factory._storehouses);
}

void Factory::add_storehouse(Storehouse&& storehouse) {
    _storehouses.add(std::move(storehouse));
    _storehouses.sort();
}

void Factory::add_worker(Worker&& worker) {
    _workers.add(std::move(worker));
    _workers.sort();
}

void Factory::add_ramp(Ramp&& ramp) {
    _ramps.add(std::move(ramp));
    _ramps.sort();
}

ParsedLineData parse_line(std::string line) {
    ParsedLineData tokens;
    std::string token;
    char delimiter1 = ' ';
    char delimiter2 = '=';

    size_t position = line.find(delimiter1);
    tokens.element_type = conversion[line.substr(0, position)];
    line.erase(0, position + 1);

    std::istringstream all_tokens(line);

    while (std::getline(all_tokens, token, delimiter1)) {
        position = token.find(delimiter2);
        std::string key = token.substr(0, position);
        token.erase(0, position + 1);
        tokens.parameters[key] = token;
    }
    return tokens;
}

Factory load_factory_structure(std::istream& is) {
    Factory factory;
    std::string line;
    while (std::getline(is, line)) {
        if (line.front() == ';' || line.front() == '\0') continue;

        ParsedLineData parsed_line = parse_line(line);

        switch (parsed_line.element_type) {
            case ElementType::LOADING_RAMP: {
                ElementID id = std::stoi(parsed_line.parameters["id"]);
                TimeOffset offset = std::stoi(parsed_line.parameters["delivery-interval"]);
                Ramp ramp(id, offset);
                factory.add_ramp(std::move(ramp));
            }break;

            case ElementType::WORKER: {
                ElementID id = std::stoi(parsed_line.parameters["id"]);
                TimeOffset offset = std::stoi(parsed_line.parameters["processing-time"]);
                if (parsed_line.parameters["queue-type"] == "FIFO") {
                    Worker worker(id, offset, std::make_unique<PackageQueue>(PackageQueueType::FIFO));
                    factory.add_worker(std::move(worker));
                } else if (parsed_line.parameters["queue-type"] == "LIFO") {
                    Worker worker(id, offset, std::make_unique<PackageQueue>(PackageQueueType::LIFO));
                    factory.add_worker(std::move(worker));
                }
            }break;

            case ElementType::STOREHOUSE: {
                ElementID id = std::stoi(parsed_line.parameters["id"]);
                Storehouse storehouse(id);
                factory.add_storehouse(std::move(storehouse));
            }break;

            case ElementType::LINK: {
                std::string src = parsed_line.parameters["src"];
                std::string dest = parsed_line.parameters["dest"];

                size_t position;
                char delimiter = '-';

                position = src.find(delimiter);
                std::string src_name = src.substr(0, position);
                src.erase(0, position + 1);
                ElementID src_id = std::stoi(src);

                position = dest.find(delimiter);
                std::string dest_name = dest.substr(0, position);
                dest.erase(0, position + 1);
                ElementID dest_id = std::stoi(dest);

                ReceiverPreferences *prefs;
                if (src_name == "ramp")
                    prefs = &factory.find_ramp_by_id(src_id)->receiver_preferences_;
                else if (src_name == "worker")
                    prefs = &factory.find_worker_by_id(src_id)->receiver_preferences_;

                if (dest_name == "worker")
                    prefs->add_receiver(&(*factory.find_worker_by_id(dest_id)));
                else if (dest_name == "store")
                    prefs->add_receiver(&(*factory.find_storehouse_by_id(dest_id)));
            }break;
        }
    }
    return factory;
}

void save_factory_structure(Factory& factory, std::ostringstream& os)
{
    std::string output;
    std::for_each(factory.ramp_cbegin(), factory.ramp_cend(),  [&os](const auto& elem) {
        os << "LOADING_RAMP id=" << std::to_string(elem.get_id()) << " delivery-interval=" << std::to_string(elem.get_delivery_interval()) << "\n";
    });

    std::for_each(factory.worker_cbegin(), factory.worker_cend(),  [&os](const auto& elem) {
        os << "WORKER id=" << std::to_string(elem.get_id()) << " processing-time=" << std::to_string(elem.get_processing_duration()) << " queue-type=";
        const auto& type = elem.get_queue()->get_queue_type();
        if(type == PackageQueueType::FIFO)
            os << "FIFO\n";
        else if(type == PackageQueueType::LIFO)
            os << "LIFO\n";
    });

    std::for_each(factory.storehouse_cbegin(), factory.storehouse_cend(),  [&os](const auto& elem) {
        os << "STOREHOUSE id=" << std::to_string(elem.get_id()) << "\n";
    });

    std::for_each(factory.ramp_cbegin(), factory.ramp_cend(),  [&os](const auto& elem) {
        auto& prefs = elem.receiver_preferences_.get_preferences();
        for (auto & pref : prefs)
        {
            os << "LINK src=ramp-" << std::to_string(elem.get_id()) << " dest=";
            const auto& type = pref.first->get_receiver_type();
            if(type == ReceiverType::WORKER)
                os << "worker-";
            else if(type == ReceiverType::STOREHOUSE)
                os << "store-";
            os << std::to_string(pref.first->get_id()) << "\n";
        }
    });

    std::for_each(factory.worker_cbegin(), factory.worker_cend(),  [&os](const auto& elem) {
        auto& prefs = elem.receiver_preferences_.get_preferences();
        for (auto & pref : prefs)
        {
            os << "LINK src=worker-" << std::to_string(elem.get_id()) << " dest=";
            const auto& type = pref.first->get_receiver_type();
            if(type == ReceiverType::WORKER)
                os << "worker-";
            else if(type == ReceiverType::STOREHOUSE)
                os << "store-";
            os << std::to_string(pref.first->get_id()) << "\n";
        }
    });
}
