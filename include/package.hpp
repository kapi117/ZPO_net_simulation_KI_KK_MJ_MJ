#ifndef NETSIM_PACKAGE_HPP
#define NETSIM_PACKAGE_HPP

/**
 * plik nagłówkowy "package.hpp" zawierający definicję klasy Package
*/

#include <set>
#include "types.hpp"

class Package {
public:
    Package();

    explicit Package(ElementID id) : id_(id) {};

    Package(Package &&package) = default;

    Package&  operator= (Package &&package) noexcept;

    ElementID get_id() const { return id_; };

    ~Package();
private:
    ElementID id_;
    static std::set<ElementID> freed_IDs;
    static std::set<ElementID> assigned_IDs;
};

#endif //NETSIM_PACKAGE_HPP