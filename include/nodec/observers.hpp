#ifndef NODEC__OBSERVERS_HPP_
#define NODEC__OBSERVERS_HPP_

#include <mutex>
#include <vector>
#include <unordered_map>

namespace nodec {

template<class IObserver, typename Mutex = std::mutex>
class Observers {

public:
    void add_observer(IObserver *observer) {
        std::lock_guard<Mutex> lock(mutex_);
        if (observer_indices_.find(observer) != observer_indices_.end()) {
            return;
        }
        observers_.push_back(observer);
        observer_indices_[observer] = observers_.size() - 1;
    }

    void remove_observer(IObserver *observer) {
        std::lock_guard<Mutex> lock(mutex_);
        auto iter = observer_indices_.find(observer);
        if (iter == observer_indices_.end()) {
            return;
        }
        auto index = iter->second;
        observers_[index] = observers_.back();
        observer_indices_[observers_[index]] = index;
        observers_.pop_back();
        observer_indices_.erase(iter);
    }

    void clear() {
        std::lock_guard<Mutex> lock(mutex_);
        observers_.clear();
        observer_indices_.clear();
    }

    template<typename Func>
    void each(Func func) {
        std::lock_guard<Mutex> lock(mutex_);
        for (auto observer : observers_) {
            func(observer);
        }
    }

private:
    Mutex mutex_;
    std::vector<IObserver *> observers_;
    std::unordered_map<IObserver *, std::size_t> observer_indices_;
};

}
#endif