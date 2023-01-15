//
// Created by Hyperbook on 15.01.2023.
//

#include "nodes.hpp"

IPackageReceiver *ReceiverPreferences::choose_receiver() {
    double random = generator_();
    double sum = 0;
    for (auto &receiver : preferences_) {
        sum += receiver.second;
        if (random <= sum) {
            return receiver.first;
        }
    }
}

void ReceiverPreferences::add_receiver(IPackageReceiver *receiver) {
    double prob = 1.0 / (preferences_.size() + 1.0);
    for (auto &pref : preferences_) {
        pref.second = prob;
    }
    preferences_.insert({receiver, prob});
}

void ReceiverPreferences::remove_receiver(IPackageReceiver *receiver) {
    preferences_.erase(receiver);
    double prob = 1.0 / preferences_.size();
    for (auto &pref : preferences_) {
        pref.second = prob;
    }
}
