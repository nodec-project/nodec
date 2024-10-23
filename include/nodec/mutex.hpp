#ifndef NODEC__MUTEX_HPP_
#define NODEC__MUTEX_HPP_

namespace nodec {

class null_mutex {
public:
    void lock() {}
    void unlock() {}
    bool try_lock() { return true; }
};

}

#endif