/**
 * @file random.hpp
 *
 * @note API design is based on the followings.
 * * https://github.com/python/cpython/blob/main/Lib/random.py
 * * https://github.com/effolkronium/random
 *
 */
#ifndef NODEC__RANDOM_HPP_
#define NODEC__RANDOM_HPP_

#include <algorithm>
#include <chrono>
#include <iterator>
#include <random>
#include <type_traits>

namespace nodec {
namespace random {

struct seeder_default {
    std::seed_seq &operator()() {
        // MinGW issue, std::random_device returns constant value
        // Use std::seed_seq with additional seed from C++ chrono.
        return seed_seq_;
    }

private:
    std::seed_seq seed_seq_{
        {
            static_cast<std::uintmax_t>(std::random_device{}()),
            static_cast<std::uintmax_t>(std::chrono::steady_clock::now()
                                            .time_since_epoch()
                                            .count()),
        }};
};

template<
    typename Engine = std::mt19937>
class BasicRandom {
public:
    using engine_type = Engine;

    BasicRandom() {
        seed(seeder_default{}());
    }

    template<typename SeedSeq>
    void seed(SeedSeq &seed_seq) {
        engine_.seed(seed_seq);
    }

    /**
     * @brief Returns a random integer N such that a <= N <= b.
     */
    template<typename Int,
             std::enable_if_t<std::is_integral<Int>::value> * = nullptr>
    Int rand_int(Int a, Int b) {
        std::uniform_int_distribution<Int> dist(a, b);
        return dist(engine_);
    }

    /**
     * @brief Returns a random floating point number N such that a <= N < b.
     */
    template<typename Real,
             std::enable_if_t<std::is_floating_point<Real>::value> * = nullptr>
    Real uniform(Real a = 0.0, Real b = 1.0) {
        std::uniform_real_distribution<Real> dist(a, b);
        return dist(engine_);
    }

    /**
     * @brief Returns a normal distributed random number.
     * mu is the mean, and sigma is the standard deviation.
     */
    template<typename Real,
             std::enable_if_t<std::is_floating_point<Real>::value> * = nullptr>
    Real gauss(Real mu = 0.0, Real sigma = 1.0) {
        std::normal_distribution<Real> dist(mu, sigma);
        return dist(engine_);
    }

    /**
     * @brief Returns a random iterator from the sequence [first, last).
     */
    template<typename Iter>
    Iter choice(Iter first, Iter last) {
        const auto size = std::distance(first, last);
        if (size == 0) return last;
        using diff_t = typename std::iterator_traits<Iter>::difference_type;
        return std::next(first, rand_int<diff_t>(0, size - 1));
    }

    template<typename Iter>
    void shuffle(Iter first, Iter last) {
        std::shuffle(first, last, engine_);
    }

private:
    Engine engine_;
};

using Random = BasicRandom<>;

inline Random &global() {
    thread_local Random instance;
    return instance;
}

} // namespace random
} // namespace nodec

#endif