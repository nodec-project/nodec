#ifndef NODEC__GMATH_HPP_
#define NODEC__GMATH_HPP_

#include <nodec/quaternion.hpp>
#include <nodec/vector3.hpp>
#include <nodec/vector2.hpp>

#include <cmath>

namespace nodec
{
/**
* math utils for graphics
*/
namespace gmath
{

extern const float pi;
extern const float deg2rad;
extern const float rad2deg;


void set_angle_axis(Quaternionf& quaternion, float angle, const Vector3f& axis);


float sqr_magnitude(const Vector3f& vector);
float sqr_magnitude(const Vector2f& vector);

float magnitude(const Vector3f& vector);
float magnitude(const Vector2f& vector);


} // End namespace gmath
}

#endif