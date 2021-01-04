#ifndef NODEC_EXTENTIONS__NODEC_SERIALIZATION__VECTOR2_HPP_
#define NODEC_EXTENTIONS__NODEC_SERIALIZATION__VECTOR2_HPP_

#include <nodec/vector2.hpp>

namespace nodec
{

template<class Archive, typename T>
void serialize(Archive& archive, Vector2<T>&vec)
{
    archive(vec.x, vec.y);
}

}


#endif