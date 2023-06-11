#ifndef NODEC__VECTOR3_HPP_
#define NODEC__VECTOR3_HPP_

#include <iostream>

namespace nodec {

/**
 * 3-dimensional vector
 * give just container and basic operation
 */
template<typename T>
class Vector3 {
public:
    union {
        struct {
            T x, y, z;
        };
        T v[3];
    };

public:
    Vector3();
    Vector3(T x, T y, T z);

    template<typename U>
    explicit Vector3(const Vector3<U> &vector);

    void set(T x, T y, T z);

public:
    static const Vector3<T> zero;
    static const Vector3<T> ones;
    // static const Vector3<T> forward;
    // static const Vector3<T> back;
    // static const Vector3<T> up;
    // static const Vector3<T> down;
    // static const Vector3<T> right;
    // static const Vector3<T> left;
};

using Vector3f = Vector3<float>;
using Vector3i = Vector3<int>;
using Vector3d = Vector3<double>;

template<typename T>
const Vector3<T> Vector3<T>::zero(0, 0, 0);

template<typename T>
const Vector3<T> Vector3<T>::ones(1, 1, 1);

template<typename T>
inline Vector3<T>::Vector3()
    : x(0), y(0), z(0) {}

template<typename T>
inline Vector3<T>::Vector3(T x, T y, T z)
    : x(x), y(y), z(z) {}

template<typename T>
template<typename U>
inline Vector3<T>::Vector3(const Vector3<U> &vector)
    : x(static_cast<T>(vector.x)),
      y(static_cast<T>(vector.y)),
      z(static_cast<T>(vector.z)) {
}

template<typename T>
inline void Vector3<T>::set(T _x, T _y, T _z) {
    x = _x;
    y = _y;
    z = _z;
}

template<typename T>
inline Vector3<T> operator-(const Vector3<T> &right) {
    return Vector3<T>(-right.x, -right.y, -right.z);
}

template<typename T>
inline Vector3<T> &operator+=(Vector3<T> &left, const Vector3<T> &right) {
    left.x += right.x;
    left.y += right.y;
    left.z += right.z;

    return left;
}

template<typename T>
inline Vector3<T> &operator-=(Vector3<T> &left, const Vector3<T> &right) {
    left.x -= right.x;
    left.y -= right.y;
    left.z -= right.z;

    return left;
}

template<typename T>
inline Vector3<T> operator+(const Vector3<T> &left, const Vector3<T> &right) {
    return Vector3<T>(left.x + right.x, left.y + right.y, left.z + right.z);
}

template<typename T>
inline Vector3<T> operator-(const Vector3<T> &left, const Vector3<T> &right) {
    return Vector3<T>(left.x - right.x, left.y - right.y, left.z - right.z);
}

template<typename T>
inline Vector3<T> operator*(const Vector3<T> &left, const Vector3<T> &right) {
    return Vector3<T>(left.x * right.x, left.y * right.y, left.z * right.z);
}

template<typename T>
inline Vector3<T> &operator*=(Vector3<T> &left, const Vector3<T> &right) {
    left.x *= right.x;
    left.y *= right.y;
    left.z *= right.z;

    return left;
}

template<typename T>
inline Vector3<T> operator*(const Vector3<T> &left, T right) {
    return Vector3<T>(left.x * right, left.y * right, left.z * right);
}

template<typename T>
inline Vector3<T> operator*(T left, const Vector3<T> &right) {
    return Vector3<T>(left * right.x, left * right.y, left * right.z);
}

template<typename T>
inline Vector3<T> &operator*=(Vector3<T> &left, T right) {
    left.x *= right;
    left.y *= right;
    left.z *= right;

    return left;
}

template<typename T>
inline Vector3<T> operator/(const Vector3<T> &left, T right) {
    return Vector3<T>(left.x / right, left.y / right, left.z / right);
}

template<typename T>
inline Vector3<T> &operator/=(Vector3<T> &left, T right) {
    left.x /= right;
    left.y /= right;
    left.z /= right;

    return left;
}

template<typename T>
inline bool operator==(const Vector3<T> &left, const Vector3<T> &right) {
    return (left.x == right.x) && (left.y == right.y) && (left.z == right.z);
}

template<typename T>
inline bool operator!=(const Vector3<T> &left, const Vector3<T> &right) {
    return (left.x != right.x) || (left.y != right.y) || (left.z != right.z);
}

template<typename T>
inline std::ostream &operator<<(std::ostream &stream, const Vector3<T> &vec) {
    return stream << "( " << vec.x << ", " << vec.y << ", " << vec.z << " )";
}

} // namespace nodec

#endif