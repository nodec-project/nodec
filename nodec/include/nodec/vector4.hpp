#ifndef NODEC__VECTOR4_HPP_
#define NODEC__VECTOR4_HPP_

#include <iostream>

namespace nodec
{
    template<typename T>
    class Vector4
    {
    public:
        union
        {
            struct { T x, y, z, w; };
            float v[4];
        };
    public:
        Vector4();
        Vector4(T x, T y, T z, T w);

        template<typename U>
        explicit Vector4(const Vector4<U>& vector);

        void set(T x, T y, T z, T w);

    public:
        static const Vector4<T> one;
        static const Vector4<T> zero;
    };

    using Vector4f = Vector4<float>;
    using Vector4i = Vector4<int>;
    using Vector4d = Vector4<double>;


    template<typename T>
    const Vector4<T> Vector4<T>::zero(0, 0, 0);

    template<typename T>
    const Vector4<T> Vector4<T>::one(1, 1, 1);


    template<typename T>
    inline Vector4<T>::Vector4() :
        x(0),
        y(0),
        z(0),
        w(0)
    {

    }

    template<typename T>
    inline Vector4<T>::Vector4(T x, T y, T z, T w) :
        x(x),
        y(y),
        z(z),
        w(w)
    {

    }

    template<typename T>
    template<typename U>
    inline Vector4<T>::Vector4(const Vector4<U>& vector) :
        x(static_cast<T>(vector.x)),
        y(static_cast<T>(vector.y)),
        z(static_cast<T>(vector.z)),
        w(static_cast<T>(vector.w)),
    {

    }


    template<typename T>
    void Vector4<T>::set(T _x, T _y, T _z, T _w)
    {
        x = _x;
        y = _y;
        z = _z;
        w = _w;
    }

    template<typename T>
    inline Vector4<T> operator-(const Vector4<T>& right)
    {
        return Vector3<T>(-right.x, -right.y, -right.z, -right.w);
    }

    template<typename T>
    inline Vector4<T>& operator+=(Vector4<T>& left, const Vector4<T>& right)
    {
        left.x += right.x;
        left.y += right.y;
        left.z += right.z;
        left.w += right.w;

        return left;
    }

    template<typename T>
    inline Vector4<T>& operator-=(Vector4<T>& left, const Vector4<T>& right)
    {
        left.x -= right.x;
        left.y -= right.y;
        left.z -= right.z;
        left.w -= right.w;

        return left;
    }


    template<typename T>
    inline Vector4<T> operator+(const Vector4<T>& left, const Vector4<T>& right)
    {
        return Vector4<T>(left.x + right.x, left.y + right.y, left.z + right.z, left.w + right.w);
    }

    template<typename T>
    inline Vector4<T> operator-(const Vector4<T>& left, const Vector4<T>& right)
    {
        return Vector4<T>(left.x - right.x, left.y - right.y, left.z - right.z, left.w - right.w);
    }

    template<typename T>
    inline Vector4<T> operator*(const Vector4<T>& left, T right)
    {
        return Vector4<T>(left.x * right, left.y * right, left.z * right, left.w * right);
    }

    template<typename T>
    inline Vector4<T> operator*(T left, const Vector4<T>& right)
    {
        return Vector4<T>(left * right.x, left * right.y, left * right.z, left * right.w);
    }

    template<typename T>
    inline Vector4<T>& operator*=(Vector4<T>& left, T right)
    {
        left.x *= right;
        left.y *= right;
        left.z *= right;
        left.w *= right;

        return left;
    }

    template<typename T>
    inline Vector4<T> operator/(const Vector4<T>& left, T right)
    {
        return Vector4<T>(left.x / right, left.y / right, left.z / right, left.w / right);
    }

    template<typename T>
    inline Vector4<T>& operator/=(Vector4<T>& left, T right)
    {
        left.x /= right;
        left.y /= right;
        left.z /= right;
        left.w /= right;

        return left;
    }

    template<typename T>
    inline bool operator==(const Vector4<T>& left, const Vector4<T>& right)
    {
        return (left.x == right.x) && (left.y == right.y) && (left.z == right.z) && (left.w == right.w);
    }

    template<typename T>
    inline bool operator!=(const Vector4<T>& left, const Vector4<T>& right)
    {
        return (left.x != right.x) || (left.y != right.y) || (left.z != right.z) || (left.w != right.w);
    }

    template<typename T>
    inline std::ostream& operator<<(std::ostream& stream, const Vector4<T>& vec)
    {
        return stream << "( " << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << " )";
    }
}

#endif