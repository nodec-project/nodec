#ifndef NODEC_SERIALIZATION__NODEC__VECTOR2_HPP_
#define NODEC_SERIALIZATION__NODEC__VECTOR2_HPP_

#include <nodec/vector2.hpp>

#include <cereal/cereal.hpp>

namespace nodec {

template<class Archive, typename T>
void serialize(Archive &archive, Vector2<T> &vec) {
    archive(
        cereal::make_nvp("x", vec.x),
        cereal::make_nvp("y", vec.y));
}

} // namespace nodec

#endif