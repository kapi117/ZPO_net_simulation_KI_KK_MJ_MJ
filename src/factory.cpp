//
// Created by Hyperbook on 15.01.2023.
//

#include "factory.hpp"

template<class Node>
void NodeCollection<Node>::add(Node &&node) {
    nodes_.emplace_back(node);
}

template<class Node>
typename NodeCollection<Node>::iterator NodeCollection<Node>::find_by_id(ElementID id) {
    return std::find_if(nodes_.begin(), nodes_.end(), [&id](const Node &node) { return node.get_id() == id; });
}

template<class Node>
typename NodeCollection<Node>::const_iterator NodeCollection<Node>::find_by_id(ElementID id) const {
    return std::find_if(nodes_.begin(), nodes_.end(), [&id](const Node &node) { return node.get_id() == id; });
}

template<class Node>
void NodeCollection<Node>::remove_by_id(ElementID id) {
    auto it = find_by_id(id);
    if (it == nodes_.end()) {
        throw std::logic_error("Node not found");
    }
    nodes_.erase(it);
}
template<class Node>
void Factory::remove_receiver(NodeCollection<Node> &collection, ElementID id){
    Node removed = collection.find_by_id(id);
    for (auto &worker : workers_) {
        worker.receiver_preferences_.remove_receiver(removed);
    }
    for (auto &ramp : ramps_) {
        ramp.receiver_preferences_.remove_receiver(removed);
    }
    collection.remove_by_id(id);
}

void Factory::do_deliveries(Time t) {
    for (auto &ramp: ramps_) {
        ramp.deliver_goods(t);
    }
}
