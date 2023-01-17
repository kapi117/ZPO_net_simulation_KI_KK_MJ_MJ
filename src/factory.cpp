//
// Created by Hyperbook on 15.01.2023.
//

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

