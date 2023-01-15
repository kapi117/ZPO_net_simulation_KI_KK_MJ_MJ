//
// Created by Hyperbook on 15.01.2023.
//

#ifndef NETSIM_NODES_HPP
#define NETSIM_NODES_HPP

#include <memory>
#include <vector>
#include <optional>
#include <map>
#include "package.hpp"
#include "storage_types.hpp"
#include "types.hpp"
#include "helpers.hpp"

extern std::optional<Package> buffer;

class IPackageReceiver {
    /**
     * Interfejs dla obiektów, które mogą odbierać paczki (Worker, Storehouse)
     */
public:
    virtual void receive_package(Package &&p) = 0;

    virtual ElementID get_id() const { return id_; };

    virtual IPackageStockpile::const_iterator begin() const = 0;

    virtual IPackageStockpile::const_iterator end() const = 0;

    virtual IPackageStockpile::const_iterator cbegin() const = 0;

    virtual IPackageStockpile::const_iterator cend() const = 0;

    virtual ~IPackageReceiver() = default;

protected:
    ElementID id_;
};

class Storehouse : public IPackageReceiver {
public:
    Storehouse(ElementID id,
               std::unique_ptr<IPackageStockpile> ptr = std::make_unique<PackageQueue>(PackageQueueType::LIFO)) {
        id_ = id;
        stockpile_ = std::move(ptr);
    };

    void receive_package(Package &&p) override {
        stockpile_->push(std::move(p));
    }

private:
    std::unique_ptr<IPackageStockpile> stockpile_;
};


class ReceiverPreferences {
    /**
     * klasa ReceiverPreferences zawiera preferencje nadawcy do przesyłania paczek
     */
public:
    /**
     * Typ prefernces_t
     * - std::map<IPackageReceiver *, double> - mapa z preferencjami, gdzie klucz to wskaźnik na odbiorcę,
     * a wartość to prawdopodobieństwo przesyłania paczki do tego odbiorcy
     */
    using preferences_t = std::map<IPackageReceiver *, double>;
    using const_iterator = preferences_t::const_iterator;

    ReceiverPreferences(ProbabilityGenerator generator = probability_generator) : generator_(generator) {};

    void add_receiver(IPackageReceiver *receiver); // TODO: KacIwi

    void remove_receiver(IPackageReceiver *receiver); // TODO: KacIwi

    const_iterator begin() const { return preferences_.begin(); }

    const_iterator end() const { return preferences_.end(); }

    const_iterator cbegin() const { return preferences_.cbegin(); }

    const_iterator cend() const { return preferences_.cend(); }


    /**
     * metoda choose_receiver() zwraca wskaźnik na odbiorcę, który został wylosowany zgodnie z preferencjami.
     * Polega to na wylosowaniu wartości z przedziału [0,1] i sprawdzeniu, do którego odbiorcy należy.
     *
     * @return wskaźnik na odbiorcę
     */
    IPackageReceiver *choose_receiver(); // TODO: KacIwi

    const preferences_t &get_preferences() const { return preferences_; };

private:
    ProbabilityGenerator generator_;
    preferences_t preferences_;
};

class PackageSender {
public:
    PackageSender(PackageSender &&) = default;

    PackageSender() {};

    /**
     * @brief Metoda send_package() wysyła paczkę z bufora do odbiorcy
     */
    void send_package(); // TODO: MarJac

    /**
     * @brief Metoda get_sending_buffer() zwraca odnośnik na odbiorcę
     * @return referencja na odbiorcę
     */
    std::optional<Package> &get_sending_buffer() { return sending_buffer_; };

    ReceiverPreferences receiver_preferences_;
protected:
    /**
     * @brief Przekazywanie paczki do bufora. Usuwa paczkę z kolejki paczek i wrzuca ją do bufora
     * @param p - paczka do przekazania
     */
    void push_package(Package &&p);
    std::optional<Package>& sending_buffer_ = buffer;
};

class Ramp : public PackageSender {
public:
    Ramp(ElementID id, TimeOffset di) : di_(di) { id_ = id; };

    /**
     * @brief Metoda deliver_goods() wywoływana jest przez symulację, w punkcie "Dostawa".
     * pozwala ona rampie zorientować się, kiedy powinna “wytworzyć” półprodukt
     * (na podstawie argumentu di typu TimeOffset przekazanego w konstruktorze klasy Ramp reprezentującego okres pomiędzy dostawami).
     * @param t - bieżący czas symulacji
     */
    void deliver_goods(Time t); // TODO: MarJan

    TimeOffset get_delivery_interval() const { return di_; };

    ElementID get_id() const { return id_; };

private:
    /**
     * @brief Czas między dostawami
     */
    TimeOffset di_;
    ElementID id_;
};

class Worker : public IPackageReceiver, public PackageSender {
public:
    Worker(ElementID id, TimeOffset pd, std::unique_ptr<IPackageQueue> packageQueue) : pd_(pd) {
        id_ = id;
        package_queue_ = std::move(packageQueue);
    };

    /**
     * @brief Metoda do_work() wywoływana jest przez symulację, w punkcie "Przetworzenie".
     * pozwala ona pracownikowi zorientować się, kiedy powinien zakończyć pracę nad aktualnie przetwarzaną paczką.
     * Na początku ustawia package_processing_start_time_ na bieżący czas symulacji, w celu odliczania czasu.
     * @param t - bieżący czas symulacji
     */
    void do_work(Time t); // TODO: KacKac

    TimeOffset get_processing_duration() const { return pd_; };

    Time get_package_processing_start_time() const { return package_processing_start_time_; };

private:
    TimeOffset pd_;
    Time package_processing_start_time_;
    std::unique_ptr<IPackageQueue> package_queue_;
};

#endif //NETSIM_NODES_HPP
