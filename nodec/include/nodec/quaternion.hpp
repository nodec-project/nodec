#ifndef NODEC__QUATERNION_HPP_
#define NODEC__QUATERNION_HPP_

#include <iostream>

namespace nodec
{

/**
* Quaternion
* q = x i + y j + z k + w
*
* <https://en.wikipedia.org/wiki/Quaternion>
*/
template<typename T>
class Quaternion
{
public:
    union
    {
        struct { T x, y, z, w; };
        T q[4];
    };
public:
    Quaternion();
    Quaternion(T x, T y, T z, T w);

    template<typename U>
    Quaternion(const Quaternion<U>& quaterion);

    void set(T x, T y, T z, T w);

public:
    static const Quaternion<T> identity;

};

using Quaternionf = Quaternion<float>;
using Quaterniond = Quaternion<double>;

template<typename T>
const Quaternion<T> Quaternion<T>::identity(0, 0, 0, 1);

template<typename T>
inline Quaternion<T>::Quaternion() :
    x(0),
    y(0),
    z(0),
    w(1)
{

}

template<typename T>
inline Quaternion<T>::Quaternion(T x, T y, T z, T w) :
    x(x),
    y(y),
    z(z),
    w(w)
{

}

template<typename T>
template<typename U>
inline Quaternion<T>::Quaternion(const Quaternion<U>& quaternion) :
    x(quaternion.x),
    y(quaternion.y),
    z(quaternion.z),
    w(quaternion.w)
{

}

template<typename T>
inline void Quaternion<T>::set(T _x, T _y, T _z, T _w)
{
    x = _x;
    y = _y;
    z = _z;
    w = _w;
}

template<typename T>
inline Quaternion<T> operator* (const Quaternion<T>& left, const Quaternion<T>& right)
{
    return Quaternion<T>(
        +left.x * right.w + left.y * right.z - left.z * right.y + left.w * right.x,
        -left.x * right.z + left.y * right.w + left.z * right.x + left.w * right.y,
        +left.x * right.y - left.y * right.x + left.z * right.w + left.w * right.z,
        -left.x * right.x - left.y * right.y - left.z * right.z + left.w * right.w
        );
}

template<typename T>
inline Quaternion<T> operator+ (const Quaternion<T>& left, const Quaternion<T>& right)
{
    return Quaternion<T>(
        left.x + right.x,
        left.y + right.y,
        left.z + right.z,
        left.w + right.w
        );
}

template<typename T>
inline std::ostream& operator<<(std::ostream& stream, const Quaternion<T>& quaternion)
{
    return stream << "( " << quaternion.x << " )i + ( " << quaternion.y << " )j + ( " << quaternion.z << " )k + ( " << quaternion.w << " )";
}
}

#endif