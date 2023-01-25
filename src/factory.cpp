//
// Created by Hyperbook on 15.01.2023.
//

#include <istream>
#include <ostream>
#include <sstream>
#include "factory.hpp"

template<class Node>
void NodeCollection<Node>::add(Node &&node) {
    nodes_.push_back(std::move(node));
}

template void NodeCollection<Ramp>::add(Ramp &&ramp);

template void NodeCollection<Worker>::add(Worker &&worker);

template void NodeCollection<Storehouse>::add(Storehouse &&sender);

template<class Node>
typename NodeCollection<Node>::iterator NodeCollection<Node>::find_by_id(ElementID id) {
    return std::find_if(nodes_.begin(), nodes_.end(), [&id](const Node &node) { return node.get_id() == id; });
}

template NodeCollection<Ramp>::iterator NodeCollection<Ramp>::find_by_id(ElementID id);

template NodeCollection<Worker>::iterator NodeCollection<Worker>::find_by_id(ElementID id);

template NodeCollection<Storehouse>::iterator NodeCollection<Storehouse>::find_by_id(ElementID id);

template<class Node>
typename NodeCollection<Node>::const_iterator NodeCollection<Node>::find_by_id(ElementID id) const {
    return std::find_if(nodes_.begin(), nodes_.end(), [&id](const Node &node) { return node.get_id() == id; });
}

template NodeCollection<Ramp>::const_iterator NodeCollection<Ramp>::find_by_id(ElementID id) const;

template NodeCollection<Worker>::const_iterator NodeCollection<Worker>::find_by_id(ElementID id) const;

template NodeCollection<Storehouse>::const_iterator NodeCollection<Storehouse>::find_by_id(ElementID id) const;

template<class Node>
void NodeCollection<Node>::remove_by_id(ElementID id) {
    auto it = find_by_id(id);
    if (it != nodes_.end()) {
        nodes_.erase(it);
    }
}

template void NodeCollection<Ramp>::remove_by_id(ElementID id);

template void NodeCollection<Worker>::remove_by_id(ElementID id);

template void NodeCollection<Storehouse>::remove_by_id(ElementID id);

template<class Node>
void Factory::remove_receiver(NodeCollection<Node> &collection, ElementID id) {
    auto removed = collection.find_by_id(id);
    IPackageReceiver *receiver = dynamic_cast<IPackageReceiver *>(&(*removed));
    for (auto &worker: workers_) {
        worker.receiver_preferences_.remove_receiver(receiver);
    }
    for (auto &ramp: ramps_) {
        ramp.receiver_preferences_.remove_receiver(receiver);
    }
    collection.remove_by_id(id);
}

template void Factory::remove_receiver(NodeCollection<Worker> &collection, ElementID id);

template void Factory::remove_receiver(NodeCollection<Storehouse> &collection, ElementID id);


void Factory::do_deliveries(Time t) {
    for (auto &ramp: ramps_) {
        ramp.deliver_goods(t);
    }
}

void Factory::do_work(Time t) {
    for (auto &worker: workers_) {
        worker.do_work(t);
    }
}

void Factory::do_package_passing() {
    for (auto &ramp: ramps_) {
        ramp.send_package();
    }
    for (auto &worker: workers_) {
        worker.send_package();
    }
}

enum class NodeState {
    kNotVisited,
    kVisited,
    kFinished
};

bool is_storehouse_achievable(const PackageSender *node, std::map<const PackageSender *, NodeState> &node_states) {
    if (node_states[node] == NodeState::kFinished) {
        return true;
    }
    node_states[node] = NodeState::kVisited;

    if (node->receiver_preferences_.get_preferences().empty()) {
        throw std::logic_error("No receivers");
    }

    bool has_receiver = false;
    for (auto &receiver: node->receiver_preferences_.get_preferences()) {
        if (receiver.first->get_receiver_type() == ReceiverType::STOREHOUSE) {
            has_receiver = true;
        } else if (receiver.first->get_receiver_type() == ReceiverType::WORKER) {
            IPackageReceiver *receiver_ptr = receiver.first;
            auto worker_ptr = dynamic_cast<Worker *>(receiver_ptr);
            auto sendrecv_ptr = dynamic_cast<PackageSender *>(worker_ptr);
            if (sendrecv_ptr == node) {
                continue;
            }
            has_receiver = true;
            if (node_states[sendrecv_ptr] == NodeState::kNotVisited) {
                is_storehouse_achievable(sendrecv_ptr, node_states);
            }
        }
    }

    node_states[node] = NodeState::kFinished;

    if (!has_receiver) {
        throw std::logic_error("No receiver");
    }
    return true;
}

bool Factory::is_consistent() const {
    std::map<const PackageSender *, NodeState> node_states;

    for (const PackageSender &ramp: ramps_) {
        node_states[&ramp] = NodeState::kNotVisited;
    }

    for (const PackageSender &worker: workers_) {
        node_states[&worker] = NodeState::kNotVisited;
    }

    try {
        for (const PackageSender &ramp: ramps_) {
            is_storehouse_achievable(&ramp, node_states);
        }
    }
    catch (...) {
        return false;
    }
    return true;
}


ParsedLineData parse_line(const std::string &line) {
    std::istringstream iss(line);
    std::string tag;
    iss >> tag;
    if (ELEMENT_TYPE_NAMES.find(tag) == ELEMENT_TYPE_NAMES.end()) {
        throw std::logic_error("Unknown tag");
    }
    ParsedLineData data;
    ElementType type = ELEMENT_TYPE_NAMES.at(tag);

    data.type = type;

    std::string key_value;
    while (iss >> key_value) {
        std::istringstream iss2(key_value);
        std::string key;
        std::string value;
        std::getline(iss2, key, '=');
        std::getline(iss2, value, '=');
        if (key.empty() || value.empty()) {
            throw std::logic_error("Empty key or value");
        }
        if (std::find(REQUIRED_FIELDS.at(type).begin(), REQUIRED_FIELDS.at(type).end(), key) ==
            REQUIRED_FIELDS.at(type).end()) {
            throw std::logic_error("Unknown key");
        }
        data.data.insert({key, value});
    }

    return data;
}

Factory load_factory_structure(std::istream &is) {
    Factory factory;
    std::string line;
    while (std::getline(is, line)) {
        if (line.empty() || line[0] == ';') {
            continue;
        }
        ParsedLineData data = parse_line(line);
        switch (data.type) {
            case ElementType::RAMP:
                try {
                    ElementID id = std::stoi(data.data.at("id"));
                    TimeOffset di = std::stoi(data.data.at("delivery-interval"));
                    factory.add_ramp(Ramp(id, di));
                }
                catch (...) {
                    throw std::logic_error("Invalid values");
                }
                break;
            case ElementType::WORKER:
                try {
                    ElementID id = std::stoi(data.data.at("id"));
                    TimeOffset pd = std::stoi(data.data.at("processing-time"));
                    factory.add_worker(Worker(id, pd, std::make_unique<PackageQueue>(
                            QUEUE_TYPE_NAMES.at(data.data.at("queue-type")))));
                }
                catch (...) {
                    throw std::logic_error("Invalid values");
                }
                break;
            case ElementType::STOREHOUSE:
                try {
                    ElementID id = std::stoi(data.data.at("id"));
                    factory.add_storehouse(Storehouse(id));
                }
                catch (...) {
                    throw std::logic_error("Invalid values");
                }
                break;
            case ElementType::LINK:
                try {
                    std::istringstream src(data.data.at("src"));
                    std::string src_type;
                    std::string src_sid;
                    ElementID src_id;
                    std::getline(src, src_type, '-');
                    std::getline(src, src_sid, '-');
                    src_id = std::stoi(src_sid);
                    std::istringstream dst(data.data.at("dest"));
                    std::string dst_type;
                    std::string dst_sid;
                    ElementID dst_id;
                    std::getline(dst, dst_type, '-');
                    std::getline(dst, dst_sid, '-');
                    dst_id = std::stoi(dst_sid);

                    if (src_type == "ramp") {
                        auto ramp = factory.find_ramp_by_id(src_id);
                        if (ramp == factory.ramp_cend()) {
                            throw std::logic_error("Ramp not found");
                        }

                        if (dst_type == "worker") {
                            auto worker = factory.find_worker_by_id(dst_id);
                            if (worker == factory.worker_cend()) {
                                throw std::logic_error("Worker not found");
                            }
                            ramp->receiver_preferences_.add_receiver(&(*worker));
                        } else if (dst_type == "store") {
                            auto storehouse = factory.find_storehouse_by_id(dst_id);
                            if (storehouse == factory.storehouse_cend()) {
                                throw std::logic_error("Storehouse not found");
                            }
                            ramp->receiver_preferences_.add_receiver(&(*storehouse));
                        } else {
                            throw std::logic_error("Unknown destination type");
                        }
                    } else if (src_type == "worker") {
                        auto worker_src = factory.find_worker_by_id(src_id);
                        if (dst_type == "worker") {
                            auto worker = factory.find_worker_by_id(dst_id);
                            if (worker == factory.worker_cend()) {
                                throw std::logic_error("Worker not found");
                            }
                            worker_src->receiver_preferences_.add_receiver(&(*worker));
                        } else if (dst_type == "store") {
                            auto storehouse = factory.find_storehouse_by_id(dst_id);
                            if (storehouse == factory.storehouse_cend()) {
                                throw std::logic_error("Storehouse not found");
                            }
                            worker_src->receiver_preferences_.add_receiver(&(*storehouse));
                        } else {
                            throw std::logic_error("Unknown destination type");
                        }
                    } else {
                        throw std::logic_error("Unknown source type");
                    }
                }
                catch (...) {
                    throw std::logic_error("Invalid values");
                }
                break;
        }
    }
    return factory;
}

void save_factory_structure(const Factory &factory, const std::ostream &os) {

}
