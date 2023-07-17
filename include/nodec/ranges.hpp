#ifndef NODEC__RANGES_HPP_
#define NODEC__RANGES_HPP_

#include <type_traits>
#include <utility>

namespace nodec {
namespace ranges {

template<typename Map>
class keys_view {
    template<typename It>
    class Iterator {
    public:
        using difference_type = typename It::difference_type;
        using iterator_category = typename It::iterator_category;
        using value_type = typename std::tuple_element<0, typename It::value_type>::type;
        using pointer = value_type*;
        using reference = value_type &;

        Iterator(It it)
            : it_(it) {}

        reference operator*() const {
            return std::get<0>(*it_);
        }

        Iterator &operator++() {
            ++it_;
            return *this;
        }

        Iterator operator++(int) {
            auto temp = *this;
            ++it_;
            return temp;
        }

        bool operator==(const Iterator &other) const {
            return it_ == other.it_;
        }

        bool operator!=(const Iterator &other) const {
            return it_ != other.it_;
        }

    private:
        It it_;
    };

public:
    using iterator = std::conditional_t<std::is_const<Map>::value,
                                        Iterator<typename Map::const_iterator>,
                                        Iterator<typename Map::iterator>>;
    keys_view(Map &map)
        : map_(map) {}

    iterator begin() const {
        return {map_.begin()};
    }

    iterator end() const {
        return {map_.end()};
    }

    auto size() const {
        return map_.size();
    }

private:
    Map &map_;
};

namespace views {
template<typename Map>
auto keys(Map &map) {
    return keys_view<Map>(map);
}

} // namespace views

} // namespace ranges

namespace views = ranges::views;
} // namespace nodec

#endif
