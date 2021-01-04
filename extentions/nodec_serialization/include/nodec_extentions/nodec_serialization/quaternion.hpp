#ifndef NODEC_EXTENTIONS__NODEC_SERIALIZATION__QUATERNION_HPP_
#define NODEC_EXTENTIONS__NODEC_SERIALIZATION__QUATERNION_HPP_

#include <nodec/quaternion.hpp>

namespace nodec
{

template<class Archive, typename T>
void serialize(Archive& archive, Quaternion<T>& q)
{
    archive(q.x, q.y, q.z, q.w);
}

}


#endif