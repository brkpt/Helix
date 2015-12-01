#ifndef MATRIX_H
#define MATRIX_H

#include "Vector.h"

// Matrices are post multiplied by column vectors
// Correct chaining is left to right
//
// eg: result = trans * scale * rot * pos

namespace Helix {

// 3x3 Matrix
class Matrix3x3
{
public:
	Matrix3x3();
	Matrix3x3(const Matrix3x3 &other);
	Matrix3x3(const Vector3 &r1, const Vector3 &r2, const Vector3 &r3);

	// Matrix setting
	Matrix3x3 & SetIdentity();
	Matrix3x3 &	SetXRotation(float radians);
	Matrix3x3 & SetYRotation(float radians);
	Matrix3x3 & SetZRotation(float radians);
	Matrix3x3 & Transpose();

	// Matrix concatenation
	Matrix3x3		operator*(const Matrix3x3 &rhs);

	// Vector transform
	Vector3			operator*(const Vector3 &rhs) const;

	static const int	NUM_ELEMENTS = 9;
	static const int	NUM_ROWS = 3;
	static const int	NUM_COLS = 3;
	union
	{
		float	e[NUM_ELEMENTS];
		float	r[NUM_ROWS][NUM_COLS];
	};
};

// 4x4 Matrix
class Matrix4x4
{
public:
	Matrix4x4();
	Matrix4x4(const Matrix4x4 &other);
	Matrix4x4(const Vector4 &r1, const Vector4 &r2, const Vector4 &r3, const Vector4 &r4);

	// Matrix setting
	Matrix4x4 & SetIdentity();

	// Left handed rotations
	Matrix4x4 &	SetXRotation(float radians);
	Matrix4x4 & SetYRotation(float radians);
	Matrix4x4 & SetZRotation(float radians);

	// Projection matrix - left handed (view down +z)
	Matrix4x4 & SetProjection(float width, float height, float nearZ, float farZ);
	Matrix4x4 & SetProjectionFOV(float fovY, float aspect, float nearZ, float farZ);

	// Translation matrix
	Matrix4x4 & SetTranslation(float x, float y, float z);
	Matrix4x4 & SetTranslation(Vector4 &vec);

	// Scaling
	Matrix4x4 & SetScale(float x, float y, float z, float w=1.0f);

	// Transpose
	Matrix4x4 & Transpose();

	// Matrix concatenation
	Matrix4x4		operator*(const Matrix4x4 &rhs);

	// Vector transform
	Vector4			operator*(const Vector4 &rhs) const;

	// Scale operation
	void Scale(float factor);

	// Determinant/Inversion
	float		Determinant() const;
	Matrix4x4 &	Cofactor(const Matrix4x4 & other);
	bool		Invert();

	static const int	NUM_ELEMENTS = 16;
	static const int	NUM_ROWS = 4;
	static const int	NUM_COLS = 4;
	union
	{
		float	e[NUM_ELEMENTS];
		float	r[NUM_ROWS][NUM_COLS];
	};
};
} // namespace Helix

#endif // MATRIX_H

