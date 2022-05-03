//
// Created by Marcin on 14.12.2020.
//
// Marcin Pilarski, 401802
// Kacper Szmajdel, 402019
// Joanna Nużka, 400561
#include "storage_types.hpp"
#include "package.hpp"

Package PackageQueue::pop() {
    PackageQueueType queue_type = get_queue_type();
    Package package;
    switch (queue_type) {
        case PackageQueueType::FIFO:
            package = std::move(_products_list.front());
            _products_list.pop_front();
            break;
        case PackageQueueType::LIFO:
            package = std::move(_products_list.back());
            _products_list.pop_back();
            break;
    }
    return package;
}