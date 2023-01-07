#include "package.hpp"

Package& Package::operator= (Package &&package) noexcept{
    id_ = package.id_;
    return (*this);
}