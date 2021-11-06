#ifndef SERIALIZATION__NODEC__VECTOR3_HPP_
#define SERIALIZATION__NODEC__VECTOR3_HPP_

#include <nodec/vector3.hpp>

namespace nodec
{

template<class Archive, typename T>
void serialize(Archive& archive, Vector3<T>&vec)
{
    archive(vec.v);
}

}


#endif