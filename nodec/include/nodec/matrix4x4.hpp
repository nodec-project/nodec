#ifndef NODEC__MATRIX4X4_HPP_
#define NODEC__MATRIX4x4_HPP_

#include <nodec/vector4.hpp>

#include <iostream>
#include <iomanip>

namespace nodec
{


/**
* Column-major representation.
* Column-major ordar
*
* cf)
*   OpenGL:
*       Column-major representation
*       Column-major ordar
*
*       <https://stackoverflow.com/questions/17717600/confusion-between-c-and-opengl-matrix-order-row-major-vs-column-major>
*
*   DirectX-Math:
*       Row-major representation
*       Row-major ordar
*       <https://docs.microsoft.com/en-us/windows/win32/api/directxmath/ns-directxmath-xmmatrix>
*
*   DirectX-HLSL:
*       Column-major representation
*       Column-major ordar
*/
template<typename T>
class Matrix4x4
{
public:
    union
    {
        T m[16];
        Vector4<T> c[4];
        struct
        {
            T m11, m21, m31, m41, m12, m22, m32, m42, m13, m23, m33, m43, m14, m24, m34, m44;
        };
    };

public:
    Matrix4x4();
    Matrix4x4(const T _m[16]);
    Matrix4x4(const Vector4<T>& c1, const Vector4<T>& c2, const Vector4<T>& c3, const Vector4<T>& c4);
    Matrix4x4(
        T m11, T m12, T m13, T m14,
        T m21, T m22, T m23, T m24,
        T m31, T m32, T m33, T m34,
        T m41, T m42, T m43, T m44);
public:
    static const Matrix4x4<T> identity;
};

using Matrix4x4f = Matrix4x4<float>;
using Matrix4x4i = Matrix4x4<int>;
using Matrix4x4d = Matrix4x4<double>;

template <typename T>
const Matrix4x4<T> Matrix4x4<T>::identity;

template<typename T>
Matrix4x4<T>::Matrix4x4() :
    m11(1), m12(0), m13(0), m14(0),
    m21(0), m22(1), m23(0), m24(0),
    m31(0), m32(0), m33(1), m34(0),
    m41(0), m42(0), m43(0), m44(1)
{
}

template<typename T>
Matrix4x4<T>::Matrix4x4(const T _m[16]) :
    m({
        _m[0] , _m[1] , _m[2] , _m[3] ,
        _m[4] , _m[5] , _m[6] , _m[7] ,
        _m[8] , _m[9] , _m[10], _m[11],
        _m[12], _m[13], _m[14], _m[15]
      })
{
}
template<typename T>
Matrix4x4<T>::Matrix4x4(
    T m11, T m12, T m13, T m14,
    T m21, T m22, T m23, T m24,
    T m31, T m32, T m33, T m34,
    T m41, T m42, T m43, T m44) :
    m11(m11), m12(m12), m13(m13), m14(m14),
    m21(m21), m22(m22), m23(m23), m24(m24),
    m31(m31), m32(m32), m33(m33), m34(m34),
    m41(m41), m42(m42), m43(m43), m44(m44)
{
}

template<typename T>
Matrix4x4<T>::Matrix4x4(const Vector4<T>& c1, const Vector4<T>& c2, const Vector4<T>& c3, const Vector4<T>& c4) :
    c({ c1, c2, c3, c4 })
{
}

//template <typename T>
//inline Matrix4x4<T> operator* (const Matrix4x4<T>& left, const Matrix4x4<T>& right)
//{
//    return Matrix4x4<T>(
//
//        );
//}

template <typename T>
inline std::ostream& operator<< (std::ostream& stream, const Matrix4x4<T>& mat)
{
    return stream << "\n"
        << "( " << mat.m11 << ",\t" << mat.m12 << ",\t" << mat.m13 << ",\t" << mat.m14 << "\n"
        << "  " << mat.m21 << ",\t" << mat.m22 << ",\t" << mat.m23 << ",\t" << mat.m24 << "\n"
        << "  " << mat.m31 << ",\t" << mat.m32 << ",\t" << mat.m33 << ",\t" << mat.m34 << "\n"
        << "  " << mat.m41 << ",\t" << mat.m42 << ",\t" << mat.m43 << ",\t" << mat.m44 << " )";
}


} // namespace nodec

#endif