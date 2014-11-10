#include "Vector.h"

// ****************************************************************************
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

// ****************************************************************************
// ****************************************************************************
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

Vector3 Vector3::Cross(const Vector3 &other) const
{
	Vector3 result;
	result.x = y*other.z - z*other.y;
	result.y = z*other.x - x*other.z;
	result.z = x*other.y - y*other.z;
	return result;
}

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

float Vector3::Dot(const Vector3 &v2) const
{
	float retVal = x*v2.x + y*v2.y + z*v2.z;
	return retVal;
}

float Vector3::Dot(const Vector3 &v1, const Vector3 &v2)
{
	return v1.Dot(v2);
}

// ****************************************************************************
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

Vector4 Vector4::operator+(const Vector4 &other) const
{
	Vector4 result;

	result.x = x + other.x;
	result.y = y + other.y;
	result.z = z + other.z;
	result.w = z + other.w;

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
	result.w = z / scalar;
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