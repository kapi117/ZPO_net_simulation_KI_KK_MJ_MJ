//
// Created by Hyperbook on 15.01.2023.
//

#include "nodes.hpp"

void Worker::do_work(Time t) {
    if (package_processing_start_time_ == 0){
        package_processing_start_time_ = t;
    }
    if (package_processing_start_time_ + pd_ == t) {
        send_package();
        push_package(package_queue_->pop());
        package_processing_start_time_ = 0;
    }
}