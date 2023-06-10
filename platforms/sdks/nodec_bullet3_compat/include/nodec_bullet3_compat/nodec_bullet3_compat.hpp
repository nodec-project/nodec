#ifndef NODEC_BULLET3_COMPAT__NODEC_BULLET3_COMPAT_HPP_
#define NODEC_BULLET3_COMPAT__NODEC_BULLET3_COMPAT_HPP_

#include <btBulletDynamicsCommon.h>

#include <nodec/quaternion.hpp>
#include <nodec/vector3.hpp>

namespace nodec {

inline Vector3<btScalar> to_vector3(const btVector3 &v) {
    return {v.x(), v.y(), v.z()};
}

inline Quaternion<btScalar> to_quaternion(const btQuaternion &q) {
    return {q.x(), q.y(), q.z(), q.w()};
}

inline btVector3 to_bt_vector3(const Vector3<btScalar> &v) {
    return {v.x, v.y, v.z};
}

inline btQuaternion to_bt_quaternion(const Quaternion<btScalar> &q) {
    return btQuaternion(q.x, q.y, q.z, q.w);
}

} // namespace nodec

#endif