#ifndef NETSIM_STORAGE_TYPES_HPP
#define NETSIM_STORAGE_TYPES_HPP

/**
 * plik nagłówkowy "storage_types.hpp" zawierający definicje klas IPackageStockpile, IPackageQueue
 * i PackageQueue oraz typu wyliczeniowego PackageQueueType
*/

#include <list>
#include "package.hpp"
#include "types.hpp"

enum class PackageQueueType {
    FIFO,
    LIFO
};

/*!
 * IPackageStockpile
 */
class IPackageStockpile {
public:
    using const_iterator = std::list<Package>::const_iterator;

    virtual void push(Package &&) = 0;

    virtual bool empty() const = 0;

    virtual size_t size() const = 0;

    virtual const_iterator cbegin() const = 0;

    virtual const_iterator cend() const = 0;

    virtual const_iterator begin() const = 0;

    virtual const_iterator end() const = 0;

    virtual ~IPackageStockpile() = 0;
};

class IPackageQueue : public IPackageStockpile {
public:
    virtual Package pop() = 0;

    virtual PackageQueueType get_queue_type() const = 0;
};

class PackageQueue : public IPackageQueue {
public:
    PackageQueue(PackageQueueType type) : type_(type) {};

    void push(Package &&) override; //TODO: KacIwi

    bool empty() const override; //TODO: MarJan

    size_t size() const override; //TODO: MarJac

    const_iterator cbegin() const override; //TODO: KacKac

    const_iterator cend() const override; //TODO: KacIwi

    const_iterator begin() const override; //TODO: MarJan

    const_iterator end() const override; //TODO: MarJac

    Package pop() override; //TODO: MarJac

    PackageQueueType get_queue_type() const override; //TODO: KacKac

private:
    PackageQueueType type_;
};

#endif //NETSIM_STORAGE_TYPES_HPP