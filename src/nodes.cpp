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
    return nullptr;
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

void PackageSender::send_package() {
    if (sending_buffer_.has_value()) {
        auto receiver = receiver_preferences_.choose_receiver();
        if (receiver != nullptr) {
            receiver->receive_package(std::move(sending_buffer_.value()));
            sending_buffer_.reset();
        }
    }
}

void Worker::do_work(Time t) {
    if(sending_buffer_.has_value()) {
        if (package_processing_start_time_ + pd_ == t) {
            send_package();
            package_processing_start_time_ = 0;
        }
    } else {
        if (not package_queue_->empty()) {
            push_package(package_queue_->pop());
            if (package_processing_start_time_ == 0) {
                package_processing_start_time_ = t;
            }
        }
    }
}