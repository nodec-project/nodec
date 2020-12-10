#ifndef NODEC__VECTOR2_HPP_
#define NODEC__VECTOR2_HPP_

#include <iostream>

namespace nodec
{
/**
* 2-dimentional vector
* give just container and basic operation
*/
template <typename T>
class Vector2
{
public:
    union
    {
        struct { T x, y; };
        T v[2];
    };

public:
    Vector2();
    Vector2(T x, T y);

    template <typename U>
    explicit Vector2(const Vector2<U>& vector);

    void set(T x, T y);


public:
    static const Vector2<T> zero;
    static const Vector2<T> one;
    static const Vector2<T> up;
    static const Vector2<T> down;
    static const Vector2<T> right;
    static const Vector2<T> left;

};


// @note
//  declare friend global function in class, define it out of class
//      -> Raise LINK ERROR
//  It means We should friend class must be declared and defined in class.
//  HOOOOOO
// 
//  it's related with namespace..., I think.


using Vector2f = Vector2<float>;
using Vector2i = Vector2<int>;
using Vector2d = Vector2<double>;


template<typename T>
const Vector2<T> Vector2<T>::zero(0, 0);

template<typename T>
const Vector2<T> Vector2<T>::one(1, 1);

template<typename T>
const Vector2<T> Vector2<T>::up(0, 1);

template<typename T>
const Vector2<T> Vector2<T>::down(0, -1);

template<typename T>
const Vector2<T> Vector2<T>::right(1, 0);

template<typename T>
const Vector2<T> Vector2<T>::left(-1, 0);

template<typename T>
inline Vector2<T>::Vector2() :
    x(0),
    y(0)
{

}

template<typename T>
inline Vector2<T>::Vector2(T x, T y) :
    x(x),
    y(y)
{

}

template<typename T>
template<typename U>
inline Vector2<T>::Vector2(const Vector2<U>& vector)
    :
    x(static_cast<T>(vector.x)),
    y(static_cast<T>(vector.y))
{

}


template<typename T>
void Vector2<T>::set(T _x, T _y)
{
    x = _x;
    y = _y;
}

template<typename T>
inline Vector2<T> operator-(const Vector2<T>& right)
{
    return Vector2<T>(-right.x, -right.y);
}

template<typename T>
inline Vector2<T>& operator+=(Vector2<T>& left, const Vector2<T>& right)
{
    left.x += right.x;
    left.y += right.y;

    return left;
}

template<typename T>
inline Vector2<T>& operator-=(Vector2<T>& left, const Vector2<T>& right)
{
    left.x -= right.x;
    left.y -= right.y;

    return left;
}


template<typename T>
inline Vector2<T> operator+(const Vector2<T>& left, const Vector2<T>& right)
{
    return Vector2<T>(left.x + right.x, left.y + right.y);
}

template<typename T>
inline Vector2<T> operator-(const Vector2<T>& left, const Vector2<T>& right)
{
    return Vector2<T>(left.x - right.x, left.y - right.y);
}

template<typename T>
inline Vector2<T> operator*(const Vector2<T>& left, T right)
{
    return Vector2<T>(left.x * right, left.y * right);
}

template<typename T>
inline Vector2<T> operator*(T left, const Vector2<T>& right)
{
    return Vector2<T>(left * right.x, left * right.y);
}

template<typename T>
inline Vector2<T>& operator*=(Vector2<T>& left, T right)
{
    left.x *= right;
    left.y *= right;

    return left;
}

template<typename T>
inline Vector2<T> operator/(const Vector2<T>& left, T right)
{
    return Vector2<T>(left.x / right, left.y / right);
}

template<typename T>
inline Vector2<T>& operator/=(Vector2<T>& left, T right)
{
    left.x /= right;
    left.y /= right;

    return left;
}

template<typename T>
inline bool operator==(const Vector2<T>& left, const Vector2<T>& right)
{
    return (left.x == right.x) && (left.y == right.y);
}

template<typename T>
inline bool operator!=(const Vector2<T>& left, const Vector2<T>& right)
{
    return (left.x != right.x) || (left.y != right.y);
}

template<typename T>
inline std::ostream& operator<<(std::ostream& stream, const Vector2<T>& vec)
{
    return stream << "( " << vec.x << ", " << vec.y << " )";
}

}
#endif