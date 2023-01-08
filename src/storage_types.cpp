#include "storage_types.hpp"

Package PackageQueue::pop(){
    Package result;
    result = std::move(queue_.front());
    queue_.pop_front();
    return result;

}

