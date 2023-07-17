#ifndef NODEC__MATRIX4X4_HPP_
#define NODEC__MATRIX4X4_HPP_

#include <nodec/vector4.hpp>

#include <iomanip>
#include <iostream>

namespace nodec {

/**
 * Column-major representation.
 * Column-major order
 *
 * cf)
 *   OpenGL:
 *       Column-major representation
 *       Column-major order
 *
 *       <https://stackoverflow.com/questions/17717600/confusion-between-c-and-opengl-matrix-order-row-major-vs-column-major>
 *
 *   DirectX-Math:
 *       Row-major representation
 *       Row-major order
 *       <https://docs.microsoft.com/en-us/windows/win32/api/directxmath/ns-directxmath-xmmatrix>
 *
 *   DirectX-HLSL:
 *       Column-major representation
 *       Maybe Row-major
 *          * https://docs.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-per-component-math
 *
 *
 */
template<typename T>
class Matrix4x4 {
public:
    union {
        T m[16];
        Vector4<T> c[4];
        struct {
            T m11, m21, m31, m41, m12, m22, m32, m42, m13, m23, m33, m43, m14, m24, m34, m44;
        };
    };

public:
    Matrix4x4();
    Matrix4x4(const T _m[16]);
    Matrix4x4(const Vector4<T> &c1, const Vector4<T> &c2, const Vector4<T> &c3, const Vector4<T> &c4);
    Matrix4x4(
        T m11, T m12, T m13, T m14,
        T m21, T m22, T m23, T m24,
        T m31, T m32, T m33, T m34,
        T m41, T m42, T m43, T m44);

    template<typename U>
    explicit Matrix4x4(const Matrix4x4<U> &mat);

    void set(const T c1[4], const T c2[4], const T c3[4], const T c4[4]);

public:
    static const Matrix4x4<T> identity;
    static const Matrix4x4<T> zero;
};

using Matrix4x4f = Matrix4x4<float>;
using Matrix4x4i = Matrix4x4<int>;
using Matrix4x4d = Matrix4x4<double>;

template<typename T>
const Matrix4x4<T> Matrix4x4<T>::identity{
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1};

template<typename T>
const Matrix4x4<T> Matrix4x4<T>::zero{
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0};

template<typename T>
Matrix4x4<T>::Matrix4x4()
    : m11(1), m12(0), m13(0), m14(0),
      m21(0), m22(1), m23(0), m24(0),
      m31(0), m32(0), m33(1), m34(0),
      m41(0), m42(0), m43(0), m44(1) {
}

template<typename T>
Matrix4x4<T>::Matrix4x4(const T _m[16])
    : m{
        _m[0], _m[1], _m[2], _m[3],
        _m[4], _m[5], _m[6], _m[7],
        _m[8], _m[9], _m[10], _m[11],
        _m[12], _m[13], _m[14], _m[15]} {
}
template<typename T>
Matrix4x4<T>::Matrix4x4(
    T m11, T m12, T m13, T m14,
    T m21, T m22, T m23, T m24,
    T m31, T m32, T m33, T m34,
    T m41, T m42, T m43, T m44)
    : m11(m11), m12(m12), m13(m13), m14(m14),
      m21(m21), m22(m22), m23(m23), m24(m24),
      m31(m31), m32(m32), m33(m33), m34(m34),
      m41(m41), m42(m42), m43(m43), m44(m44) {
}

template<typename T>
inline void Matrix4x4<T>::set(const T c1[4], const T c2[4], const T c3[4], const T c4[4]) {
    c[0].set(c1);
    c[1].set(c2);
    c[2].set(c3);
    c[3].set(c4);
}

template<typename T>
Matrix4x4<T>::Matrix4x4(const Vector4<T> &c1, const Vector4<T> &c2, const Vector4<T> &c3, const Vector4<T> &c4)
    : c({c1, c2, c3, c4}) {
}

template<typename T>
template<typename U>
inline Matrix4x4<T>::Matrix4x4(const Matrix4x4<U> &mat)
    : m{
        static_cast<T>(mat.m[0]), static_cast<T>(mat.m[1]), static_cast<T>(mat.m[2]), static_cast<T>(mat.m[3]),
        static_cast<T>(mat.m[4]), static_cast<T>(mat.m[5]), static_cast<T>(mat.m[6]), static_cast<T>(mat.m[7]),
        static_cast<T>(mat.m[8]), static_cast<T>(mat.m[9]), static_cast<T>(mat.m[10]), static_cast<T>(mat.m[11]),
        static_cast<T>(mat.m[12]), static_cast<T>(mat.m[13]), static_cast<T>(mat.m[14]), static_cast<T>(mat.m[15])} {
}

template<typename T>
inline Matrix4x4<T> &operator*=(Matrix4x4<T> &left, const Matrix4x4<T> &right) {
    *left = left * right;
    return left;
}

template<typename T>
inline Matrix4x4<T> operator*(const Matrix4x4<T> &left, const Matrix4x4<T> &right) {
    return {
        left.m11 * right.m11 + left.m12 * right.m21 + left.m13 * right.m31 + left.m14 * right.m41,
        left.m11 * right.m12 + left.m12 * right.m22 + left.m13 * right.m32 + left.m14 * right.m42,
        left.m11 * right.m13 + left.m12 * right.m23 + left.m13 * right.m33 + left.m14 * right.m43,
        left.m11 * right.m14 + left.m12 * right.m24 + left.m13 * right.m34 + left.m14 * right.m44,

        left.m21 * right.m11 + left.m22 * right.m21 + left.m23 * right.m31 + left.m24 * right.m41,
        left.m21 * right.m12 + left.m22 * right.m22 + left.m23 * right.m32 + left.m24 * right.m42,
        left.m21 * right.m13 + left.m22 * right.m23 + left.m23 * right.m33 + left.m24 * right.m43,
        left.m21 * right.m14 + left.m22 * right.m24 + left.m23 * right.m34 + left.m24 * right.m44,

        left.m31 * right.m11 + left.m32 * right.m21 + left.m33 * right.m31 + left.m34 * right.m41,
        left.m31 * right.m12 + left.m32 * right.m22 + left.m33 * right.m32 + left.m34 * right.m42,
        left.m31 * right.m13 + left.m32 * right.m23 + left.m33 * right.m33 + left.m34 * right.m43,
        left.m31 * right.m14 + left.m32 * right.m24 + left.m33 * right.m34 + left.m34 * right.m44,

        left.m41 * right.m11 + left.m42 * right.m21 + left.m43 * right.m31 + left.m44 * right.m41,
        left.m41 * right.m12 + left.m42 * right.m22 + left.m43 * right.m32 + left.m44 * right.m42,
        left.m41 * right.m13 + left.m42 * right.m23 + left.m43 * right.m33 + left.m44 * right.m43,
        left.m41 * right.m14 + left.m42 * right.m24 + left.m43 * right.m34 + left.m44 * right.m44};
}

template<typename T>
inline Vector4<T> operator*(const Matrix4x4<T> &mat, const Vector4<T> &vec) {
    return {
        mat.m11 * vec.v[0] + mat.m12 * vec.v[1] + mat.m13 * vec.v[2] + mat.m14 * vec.v[3],
        mat.m21 * vec.v[0] + mat.m22 * vec.v[1] + mat.m23 * vec.v[2] + mat.m24 * vec.v[3],
        mat.m31 * vec.v[0] + mat.m32 * vec.v[1] + mat.m33 * vec.v[2] + mat.m34 * vec.v[3],
        mat.m41 * vec.v[0] + mat.m42 * vec.v[1] + mat.m43 * vec.v[2] + mat.m44 * vec.v[3]};
}

template<typename T>
inline Matrix4x4<T> operator*(T left, const Matrix4x4<T> &right) {
    return {
        {left * right.m[0], left * right.m[1], left * right.m[2], left * right.m[3],
         left * right.m[4], left * right.m[5], left * right.m[6], left * right.m[7],
         left * right.m[8], left * right.m[9], left * right.m[10], left * right.m[11],
         left * right.m[12], left * right.m[13], left * right.m[14], left * right.m[15]}};
}

template<typename T>
inline Matrix4x4<T> operator*(const Matrix4x4<T> &left, T right) {
    return {
        {left.m[0] * right, left.m[1] * right, left.m[2] * right, left.m[3] * right,
         left.m[4] * right, left.m[5] * right, left.m[6] * right, left.m[7] * right,
         left.m[8] * right, left.m[9] * right, left.m[10] * right, left.m[11] * right,
         left.m[12] * right, left.m[13] * right, left.m[14] * right, left.m[15] * right}};
}

template<typename T>
inline Matrix4x4<T> &operator*=(Matrix4x4<T> &left, T right) {
    left.m[0] *= right;
    left.m[1] *= right;
    left.m[2] *= right;
    left.m[3] *= right;
    left.m[4] *= right;
    left.m[5] *= right;
    left.m[6] *= right;
    left.m[7] *= right;
    left.m[8] *= right;
    left.m[9] *= right;
    left.m[10] *= right;
    left.m[11] *= right;
    left.m[12] *= right;
    left.m[13] *= right;
    left.m[14] *= right;
    left.m[15] *= right;
    return left;
}

template<typename T>
inline Matrix4x4<T> operator/(const Matrix4x4<T> &left, T right) {
    return {
        {left.m[0] / right, left.m[1] / right, left.m[2] / right, left.m[3] / right,
         left.m[4] / right, left.m[5] / right, left.m[6] / right, left.m[7] / right,
         left.m[8] / right, left.m[9] / right, left.m[10] / right, left.m[11] / right,
         left.m[12] / right, left.m[13] / right, left.m[14] / right, left.m[15] / right}};
}

template<typename T>
inline Matrix4x4<T> &operator/=(Matrix4x4<T> &left, T right) {
    left.m[0] /= right;
    left.m[1] /= right;
    left.m[2] /= right;
    left.m[3] /= right;
    left.m[4] /= right;
    left.m[5] /= right;
    left.m[6] /= right;
    left.m[7] /= right;
    left.m[8] /= right;
    left.m[9] /= right;
    left.m[10] /= right;
    left.m[11] /= right;
    left.m[12] /= right;
    left.m[13] /= right;
    left.m[14] /= right;
    left.m[15] /= right;
    return left;
}

template<typename T>
inline Matrix4x4<T> &operator+=(Matrix4x4<T> &left, const Matrix4x4<T> &right) {
    left.m[0] += right.m[0];
    left.m[1] += right.m[1];
    left.m[2] += right.m[2];
    left.m[3] += right.m[3];
    left.m[4] += right.m[4];
    left.m[5] += right.m[5];
    left.m[6] += right.m[6];
    left.m[7] += right.m[7];
    left.m[8] += right.m[8];
    left.m[9] += right.m[9];
    left.m[10] += right.m[10];
    left.m[11] += right.m[11];
    left.m[12] += right.m[12];
    left.m[13] += right.m[13];
    left.m[14] += right.m[14];
    left.m[15] += right.m[15];
    return left;
}

template<typename T>
inline Matrix4x4<T> operator+(const Matrix4x4<T> &left, const Matrix4x4<T> &right) {
    return {
        {left.m[0] + right.m[0], left.m[1] + right.m[1], left.m[2] + right.m[2], left.m[3] + right.m[3],
         left.m[4] + right.m[4], left.m[5] + right.m[5], left.m[6] + right.m[6], left.m[7] + right.m[7],
         left.m[8] + right.m[8], left.m[9] + right.m[9], left.m[10] + right.m[10], left.m[11] + right.m[11],
         left.m[12] + right.m[12], left.m[13] + right.m[13], left.m[14] + right.m[14], left.m[15] + right.m[15]}};
}

template<typename T>
inline Matrix4x4<T> &operator-=(Matrix4x4<T> &left, const Matrix4x4<T> &right) {
    left.m[0] -= right.m[0];
    left.m[1] -= right.m[1];
    left.m[2] -= right.m[2];
    left.m[3] -= right.m[3];
    left.m[4] -= right.m[4];
    left.m[5] -= right.m[5];
    left.m[6] -= right.m[6];
    left.m[7] -= right.m[7];
    left.m[8] -= right.m[8];
    left.m[9] -= right.m[9];
    left.m[10] -= right.m[10];
    left.m[11] -= right.m[11];
    left.m[12] -= right.m[12];
    left.m[13] -= right.m[13];
    left.m[14] -= right.m[14];
    left.m[15] -= right.m[15];
    return left;
}

template<typename T>
inline Matrix4x4<T> operator-(const Matrix4x4<T> &left, const Matrix4x4<T> &right) {
    return {
        {left.m[0] - right.m[0], left.m[1] - right.m[1], left.m[2] - right.m[2], left.m[3] - right.m[3],
         left.m[4] - right.m[4], left.m[5] - right.m[5], left.m[6] - right.m[6], left.m[7] - right.m[7],
         left.m[8] - right.m[8], left.m[9] - right.m[9], left.m[10] - right.m[10], left.m[11] - right.m[11],
         left.m[12] - right.m[12], left.m[13] - right.m[13], left.m[14] - right.m[14], left.m[15] - right.m[15]}};
}

template<typename T>
inline bool operator==(const Matrix4x4<T> &left, const Matrix4x4<T> &right) {
    return (left.m11 == right.m11)
           && (left.m12 == right.m12)
           && (left.m13 == right.m13)
           && (left.m14 == right.m14)
           && (left.m21 == right.m21)
           && (left.m22 == right.m22)
           && (left.m23 == right.m23)
           && (left.m24 == right.m24)
           && (left.m31 == right.m31)
           && (left.m32 == right.m32)
           && (left.m33 == right.m33)
           && (left.m34 == right.m34)
           && (left.m41 == right.m41)
           && (left.m42 == right.m42)
           && (left.m43 == right.m43)
           && (left.m44 == right.m44);
}

template<typename T>
inline bool operator!=(const Matrix4x4<T> &left, const Matrix4x4<T> &right) {
    return (left.m11 != right.m11)
           || (left.m12 != right.m12)
           || (left.m13 != right.m13)
           || (left.m14 != right.m14)
           || (left.m21 != right.m21)
           || (left.m22 != right.m22)
           || (left.m23 != right.m23)
           || (left.m24 != right.m24)
           || (left.m31 != right.m31)
           || (left.m32 != right.m32)
           || (left.m33 != right.m33)
           || (left.m34 != right.m34)
           || (left.m41 != right.m41)
           || (left.m42 != right.m42)
           || (left.m43 != right.m43)
           || (left.m44 != right.m44);
}

template<typename T>
inline std::ostream &operator<<(std::ostream &stream, const Matrix4x4<T> &mat) {
    return stream << "\n"
                  << "( " << mat.m11 << ",\t" << mat.m12 << ",\t" << mat.m13 << ",\t" << mat.m14 << "\n"
                  << "  " << mat.m21 << ",\t" << mat.m22 << ",\t" << mat.m23 << ",\t" << mat.m24 << "\n"
                  << "  " << mat.m31 << ",\t" << mat.m32 << ",\t" << mat.m33 << ",\t" << mat.m34 << "\n"
                  << "  " << mat.m41 << ",\t" << mat.m42 << ",\t" << mat.m43 << ",\t" << mat.m44 << " )";
}

} // namespace nodec

#endif