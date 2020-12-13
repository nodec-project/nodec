#ifndef NODEC__MATRIX4X4_HPP_
#define NODEC__MATRIX4x4_HPP_

namespace nodec
{
template<typename T>
class Matrix4x4
{
public:
    union
    {
        T m[16];
    };
public:

};

using Matrix4x4f = Matrix4x4<float>;
using Matrix4x4i = Matrix4x4<int>;
using Matrix4x4d = Matrix4x4<double>;
}

#endif