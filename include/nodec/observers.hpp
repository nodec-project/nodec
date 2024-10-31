#ifndef NODEC__OBSERVERS_HPP_
#define NODEC__OBSERVERS_HPP_

#include <mutex>
#include <vector>
#include <unordered_map>

/**
 * @note Implementation refs:
 * https://chromium.googlesource.com/chromium/src.git/+/master/base/observer_list.h
 * 
 */

namespace nodec {

template<class IObserver, typename Mutex = std::recursive_mutex>
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
        observers_[index] = nullptr;
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

        std::size_t n = observers_.size();
        for (std::size_t i = 0; i < n; ++i) {
            auto observer = observers_[i];
            if (observer == nullptr) continue;
            func(observer);
        }

        // Remove all empty slots.
        std::size_t sz = 0;
        for (std::size_t i = 0; i < observers_.size(); ++i) {
            if (observers_[i] == nullptr) continue;
            observers_[sz] = observers_[i];
            observer_indices_[observers_[i]] = sz;
            ++sz;
        }
        observers_.resize(sz);
    }

    std::size_t size() const {
        std::lock_guard<Mutex> lock(mutex_);
        return observer_indices_.size();
    }

private:
    Mutex mutex_;
    std::vector<IObserver *> observers_;
    std::unordered_map<IObserver *, std::size_t> observer_indices_;
};

}
#endif