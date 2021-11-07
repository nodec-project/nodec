#ifndef SERIALIZATION__NODEC__VECTOR3_HPP_
#define SERIALIZATION__NODEC__VECTOR3_HPP_

#include <cereal/cereal.hpp>

#include <nodec/vector3.hpp>


namespace nodec {

template<class Archive, typename T>
void serialize(Archive& archive, Vector3<T>& vec) {
    archive(
        cereal::make_nvp("x", vec.x),
        cereal::make_nvp("y", vec.y),
        cereal::make_nvp("z", vec.z)
    );
}

}


#endif