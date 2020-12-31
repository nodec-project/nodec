#ifndef NODEC_EXTENTIONS__NODEC_SERIALIZATION__VECTOR4_HPP_
#define NODEC_EXTENTIONS__NODEC_SERIALIZATION__VECTOR4_HPP_

#include <nodec/vector4.hpp>

namespace nodec
{

template<class Archive, typename T>
void serialize(Archive& archive, Vector4<T>&vec)
{
    archive(vec.x, vec.y, vec.z, vec.w);
}

}


#endif