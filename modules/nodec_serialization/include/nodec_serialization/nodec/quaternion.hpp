#ifndef NODEC_SERIALIZATION__NODEC__QUATERNION_HPP_
#define NODEC_SERIALIZATION__NODEC__QUATERNION_HPP_

#include <cereal/cereal.hpp>

#include <nodec/quaternion.hpp>

namespace nodec {

template<class Archive, typename T>
void serialize(Archive &archive, Quaternion<T> &q) {
    archive(
        cereal::make_nvp("x", q.x),
        cereal::make_nvp("y", q.y),
        cereal::make_nvp("z", q.z),
        cereal::make_nvp("w", q.w));
}

} // namespace nodec

#endif