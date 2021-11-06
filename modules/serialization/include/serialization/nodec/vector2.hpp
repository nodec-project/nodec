#ifndef SERIALIZATION__NODEC__VECTOR2_HPP_
#define SERIALIZATION__NODEC__VECTOR2_HPP_

#include <nodec/vector2.hpp>

namespace nodec
{

template<class Archive, typename T>
void serialize(Archive& archive, Vector2<T>&vec)
{
    archive(vec.v);
}

}


#endif