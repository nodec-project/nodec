#define _USE_MATH_DEFINES
#include <cmath>

#include <nodec/gmath.hpp>

namespace nodec
{
namespace gmath
{

const float pi = M_PI;
const float deg2rad = pi / 180.0f;
const float rad2deg = 180.0f / pi;

void set_angle_axis(Quaternionf& quaternion, float angle_deg, const Vector3f& axis)
{
    auto a = angle_deg * deg2rad / 2.0f;
    auto s = std::sin(a);
    quaternion.x = axis.x * s;
    quaternion.y = axis.y * s;
    quaternion.z = axis.z * s;
    quaternion.w = std::cos(a);
}

float sqr_magnitude(const Vector3f& vector)
{
    return (vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}

float sqr_magnitude(const Vector2f& vector)
{
    return (vector.x * vector.x + vector.y * vector.y);
}

float magnitude(const Vector3f& vector)
{
    return std::sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}

float magnitude(const Vector2f& vector)
{
    return std::sqrt(vector.x * vector.x + vector.y * vector.y);
}

}
}
