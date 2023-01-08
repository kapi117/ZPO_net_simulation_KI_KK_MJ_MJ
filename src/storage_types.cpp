#include "storage_types.hpp"


void PackageQueue::push(Package && package) {
    switch(type_){
        case PackageQueueType::LIFO:
            queue_.push_front(std::move(package));
            break;
        case PackageQueueType::FIFO:
            queue_.push_back(std::move(package));
            break;
    }
}

IPackageStockpile::const_iterator PackageQueue::cend() const {
    return queue_.cend();
}
