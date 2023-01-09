#include "package.hpp"

Package& Package::operator= (Package &&package) noexcept{
    id_ = package.id_;
    return (*this);
}

Package::Package() {
    if (freed_IDs.empty()) {
        if (assigned_IDs.empty()) {
            Package(1);
        } else {
            id_ = *(assigned_IDs.rbegin()) + 1;
        }
    } else {
        id_ = *freed_IDs.begin();
        freed_IDs.erase(freed_IDs.begin());
    }
    assigned_IDs.insert(id_);
}