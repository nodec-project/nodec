#ifndef SERIALIZATION__NODEC__QUATERNION_HPP_
#define SERIALIZATION__NODEC__QUATERNION_HPP_

#include <nodec/quaternion.hpp>

namespace nodec
{

template<class Archive, typename T>
void serialize(Archive& archive, Quaternion<T>& q)
{
    archive(q.v);
}

}


#endif