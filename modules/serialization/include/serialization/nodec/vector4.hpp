#ifndef SERIALIZATION__NODEC__VECTOR4_HPP_
#define SERIALIZATION__NODEC__VECTOR4_HPP_

#include <nodec/vector4.hpp>

namespace nodec
{

template<class Archive, typename T>
void serialize(Archive& archive, Vector4<T>&vec)
{
    archive(vec.v);
}

}


#endif