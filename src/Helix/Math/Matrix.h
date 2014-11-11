#ifndef MATRIX_H
#define MATRIX_H

#include "Vector.h"

namespace Helix {
class Matrix3x3
{
public:
	Matrix3x3();
	Matrix3x3(const Matrix3x3 &other);
	Matrix3x3(const Vector3 &r1, const Vector3 &r2, const Vector3 &r3);

	Matrix3x3 & SetIdentity();
	Matrix3x3 &	SetTransformXRotation(float radians);
	Matrix3x3 & SetTransformYRotation(float radians);
	Matrix3x3 & SetTransformZRotation(float radians);

	Matrix3x3		operator*(const Matrix3x3 &rhs);
	Vector3			operator*(const Vector3 &rhs);

	union
	{
		float	e[9];
		float	r[3][3];
	};
};

} // namespace Helix

#endif // MATRIX_H

