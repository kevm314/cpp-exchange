#pragma once

#include <list>
#include <mutex>

namespace comms {

template <class T1>
class ScopeLockedQueue {
    private:
        std::list<T1> scope_locked_queue_;
        std::mutex queue_mutex_;
    public:
        ScopeLockedQueue() {};
        bool Enqueue(T1&& item) {
            std::scoped_lock lock(queue_mutex_);
            scope_locked_queue_.push_back(std::move(item));
            return true;
        }
        bool Dequeue(T1& item) {
            std::scoped_lock lock(queue_mutex_);
            if (scope_locked_queue_.empty())
                return false;
            item = std::move(scope_locked_queue_.front());
            scope_locked_queue_.pop_front();
            return true;
        }
        size_t Size() {
            std::scoped_lock lock(queue_mutex_);
            return scope_locked_queue_.size();
        }
};

}