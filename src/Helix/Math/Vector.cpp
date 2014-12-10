#include <Math.h>
#include "Vector.h"

// ****************************************************************************
// Global operators
// ****************************************************************************
Helix::Vector3 operator*(const float lhs, const Helix::Vector3 &rhs)
{
	Helix::Vector3 result = rhs*lhs;
	return result;
}

Helix::Vector3 operator/(const float lhs, const Helix::Vector3 &rhs)
{
	Helix::Vector3 result = rhs*lhs;
	return result;
}

Helix::Vector4 operator*(const float lhs, const Helix::Vector4 &rhs)
{
	Helix::Vector4 result = rhs*lhs;
	return result;
}

Helix::Vector4 operator/(const float lhs, const Helix::Vector4 &rhs)
{
	Helix::Vector4 result = rhs*lhs;
	return result;
}


namespace Helix {

// ****************************************************************************
// Vector3
// ****************************************************************************
Vector3::Vector3()
: x(0.f)
, y(0.f)
, z(0.f)
{}

Vector3::Vector3(const float _x, const float _y, const float _z)
: x(_x)
, y(_y)
, z(_z)
{}

Vector3::Vector3(const Vector3 &other)
{
	x = other.x;
	y = other.y;
	z = other.z;
}

Vector3::Vector3(const float *in)
{
	x = in[0];
	y = in[1];
	z = in[2];
}

// ****************************************************************************
// ****************************************************************************
float Vector3::Dot(const Vector3 &v2) const
{
	float retVal = x*v2.x + y*v2.y + z*v2.z;
	return retVal;
}

// Static C-style version that takes two vectors
float Vector3::Dot(const Vector3 &v1, const Vector3 &v2)
{
	return v1.Dot(v2);
}

// ****************************************************************************
// Sets the vector to be the cross of the two given vectors.
// ****************************************************************************
Vector3 & Vector3::Cross(const Vector3 &v1, const Vector3 &v2 )
{
	x = v1.y*v2.z - v1.z*v2.y;
	y = v1.z*v2.x - v1.x*v2.z;
	z = v1.x*v2.y - v1.y*v2.z;
	return *this;
}

// ****************************************************************************
// ****************************************************************************
float Vector3::Length()
{
	float len = sqrtf(x*x + y*y + z*z);
	return len;
}
// ****************************************************************************
// ****************************************************************************
Vector3 & Vector3::Normalize()

{
	float len = Length();
	x = x / len;
	y = y / len;
	z = z / len;
	return *this;
}

// ****************************************************************************
// ****************************************************************************
Vector3 & Vector3::operator=(const Vector3 &other)
{
	x = other.x;
	y = other.y;
	z = other.z;
	return *this;
}

// ****************************************************************************
// ****************************************************************************
Vector3 Vector3::operator+(const Vector3 &other) const
{
	Vector3 result;

	result.x = x + other.x;
	result.y = y + other.y;
	result.z = z + other.z;

	return result;
}

Vector3 Vector3::operator-(const Vector3 &other) const
{
	Vector3 result;
	result.x = x - other.x;
	result.y = y - other.y;
	result.z = z - other.z;
	return result;
}

Vector3 Vector3::operator*(const float scalar) const
{
	Vector3 result;
	result.x = x * scalar;
	result.y = y * scalar;
	result.z = z * scalar;
	return result;
}

Vector3 Vector3::operator/(const float scalar) const
{
	Vector3 result;
	result.x = x / scalar;
	result.y = y / scalar;
	result.z = z / scalar;
	return result;
}

Vector3 & Vector3::operator+=(const Vector3 &other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}

Vector3 & Vector3::operator-=(const Vector3 &other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return *this;
}

Vector3 & Vector3::operator*=(const float scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	return *this;
}

Vector3 & Vector3::operator/=(const float scalar)
{
	x /= scalar;
	y /= scalar;
	z /= scalar;
	return *this;
}

// ****************************************************************************
// Vector4
// ****************************************************************************
Vector4::Vector4()
: x(0.f)
, y(0.f)
, z(0.f)
, w(0.f)
{}

Vector4::Vector4(const float _x, const float _y, const float _z, const float _w)
: x(_x)
, y(_y)
, z(_z)
, w(_w)
{}

Vector4::Vector4(const Vector4 &other)
{
	x = other.x;
	y = other.y;
	z = other.z;
	w = other.w;
}

Vector4::Vector4(const float *in)
{
	x = in[0];
	y = in[1];
	z = in[2];
	w = in[3];
}

// ****************************************************************************
// ****************************************************************************
float Vector4::Dot(const Vector4 &v2) const
{
	float result = x*v2.x + y*v2.y + z*v2.z + w*v2.w;
	return result;
}

float Vector4::Dot(const Vector4 &v1, const Vector4 &v2)
{
	float result = v1.x*v2.x + v1.y*v2.y + v1.z*v2.z + v1.w*v2.w;
	return result;
}

// ****************************************************************************
// ****************************************************************************
float Vector4::Length()
{
	float len = sqrtf(x*x + y*y + z*z + w*w);
	return len;
}

// ****************************************************************************
// ****************************************************************************
Vector4 & Vector4::Normalize()
{
	float len = Length();
	x = x / len;
	y = y / len;
	z = z / len;
	w = w / len;
	return *this;
}

// ****************************************************************************
// ****************************************************************************
Vector4 & Vector4::operator=(const Vector4 &other)
{
	x = other.x;
	y = other.y;
	z = other.z;
	w = other.w;
	return *this;
}

// ****************************************************************************
// ****************************************************************************
Vector4 Vector4::operator+(const Vector4 &other) const
{
	Vector4 result;
	result.x = x + other.x;
	result.y = y + other.y;
	result.z = z + other.z;
	result.w = w + other.w;
	return result;
}

Vector4 Vector4::operator-(const Vector4 &other) const
{
	Vector4 result;
	result.x = x - other.x;
	result.y = y - other.y;
	result.z = z - other.z;
	result.w = w - other.w;
	return result;
}

Vector4 Vector4::operator*(const float scalar) const
{
	Vector4 result;
	result.x = x * scalar;
	result.y = y * scalar;
	result.z = z * scalar;
	result.w = w * scalar;
	return result;
}

Vector4 Vector4::operator/(const float scalar) const
{
	Vector4 result;
	result.x = x / scalar;
	result.y = y / scalar;
	result.z = z / scalar;
	result.w = w / scalar;
	return result;
}

Vector4 & Vector4::operator+=(const Vector4 &other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	w += other.w;
	return *this;
}

Vector4 & Vector4::operator-=(const Vector4 &other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	w -= other.w;
	return *this;
}

Vector4 & Vector4::operator*=(const float scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	w *= scalar;
	return *this;
}

Vector4 & Vector4::operator/=(const float scalar)
{
	x /= scalar;
	y /= scalar;
	z /= scalar;
	w /= scalar;
	return *this;
}

} // namespace Helix
