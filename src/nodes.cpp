//
// Created by Hyperbook on 15.01.2023.
//

#include "nodes.hpp"

void Worker::do_work(Time t) {
    if(!sending_buffer_) {
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