#ifndef NODEC_EXTENTIONS__NODEC_SERIALIZATION__VECTOR3_HPP_
#define NODEC_EXTENTIONS__NODEC_SERIALIZATION__VECTOR3_HPP_

#include <nodec/vector3.hpp>

namespace nodec
{

template<class Archive, typename T>
void serialize(Archive& archive, Vector3<T>&vec)
{
    archive(vec.x, vec.y, vec.z);
}

}


#endif