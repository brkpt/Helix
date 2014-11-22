#ifndef HVECTOR_H 
#define HVECTOR_H


namespace Helix
{
class Matrix3x3;

struct Vector3
{
	Vector3();
	Vector3(const float x, const float y, const float z);
	Vector3(const Vector3 &other);
	Vector3(const float init[3]);

	float			Dot(const Vector3 &v2) const;
	static float	Dot(const Vector3 &v1, const Vector3 &v2);
	Vector3			Cross(const Vector3 &v2) const;

	// Vector operations
	Vector3		operator+(const Vector3 &rhs) const;
	Vector3		operator-(const Vector3 &rhs) const;
	Vector3		operator*(const float scalar) const;
	Vector3		operator/(const float scalar) const;
	Vector3 &	operator+=(const Vector3 &rhs);
	Vector3 &	operator-=(const Vector3 &rhs);
	Vector3 &	operator*=(const float scalar);
	Vector3 &	operator/=(const float scalar);

	void Zero() { x = 0.f; y = 0.f; z = 0.f; }

	// statics
	float	x, y, z;
};

struct Vector4
{
	Vector4();
	Vector4(const float x, const float y, const float z, float w=1.0f);
	Vector4(const Vector4 &other);
	Vector4(const float init[4]);

	float			Dot(const Vector4 &rhs) const;
	static float	Dot(const Vector4 &v1, const Vector4 &v2);

	// Operators
	Vector4		operator+(const Vector4 &rhs) const;
	Vector4		operator-(const Vector4 &rhs) const;
	Vector4		operator*(const float scalar) const;
	Vector4		operator/(const float scalar) const;
	Vector4 &	operator+=(const Vector4 &rhs);
	Vector4 &	operator-=(const Vector4 &rhs);
	Vector4 &	operator*=(const float scalar);
	Vector4 &	operator/=(const float scalar);

	void Zero() { x = 0.f; y = 0.f; z = 0.f; w = 0.f; }

	float	x, y, z, w;
};

} // namespace Helix

Helix::Vector3 operator*(const float lhs, const Helix::Vector3 &rhs);
Helix::Vector3 operator/(const float lhs, const Helix::Vector3 &rhs);
Helix::Vector4 operator*(const float lhs, const Helix::Vector4 &rhs);
Helix::Vector4 operator/(const float lhs, const Helix::Vector4 &rhs);

#endif // HVECTOR_H

