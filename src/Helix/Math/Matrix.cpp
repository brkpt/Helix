#include <Math.h>
#include "Matrix.h"

// ****************************************************************************
// 2x2 matrix determinant
//
// | a  b | = a*d - b*c
// | c  d |
// ****************************************************************************
inline float Matrix2x2_Determinant(float a,float b, float c, float d)
{
	return a*d - b*c;
}

// ****************************************************************************
// 3x3 matrix determinant
// Recursive calculation using 2x2 matrix determinants
//
// | a  b  c | = a * Det(e,f,h,i) - b * Det(d,f,g,i) + c * Det(d,e,g,h)
// | d  e  f | 
// | g  h  i |
// ****************************************************************************
inline float Matrix3x3_Determinant(float a, float b, float c, float d, float e, float f, float g, float h, float i)
{
	return a * Matrix2x2_Determinant(e,f,h,i) - b * Matrix2x2_Determinant(d,f,g,i) + c * Matrix2x2_Determinant(d,e,g,h);
}

// ****************************************************************************
// 4x4 matrix determinant
// Recursive calculation using 3x3 matrix determinants
//
// | a  b  c  d | = a * Det(f,g,h,j,k,l,n,o,p) - b * Det(e,g,h,i,k,l,m,o,p) + 
// | e  f  g  h |   c * Det(e,f,h,i,j,l,m,n,p) - d * Det(e,f,g,i,j,k,m,n,o)
// | i  j  k  l |
// | m  n  o  p |
// ****************************************************************************
inline float Matrix4x4_Determinant(float a,float b,float c,float d,float e,float f,float g,float h,float i,float j,float k,float l,float m,float n,float o,float p) 
{
	return a * Matrix3x3_Determinant(f,g,h,j,k,l,n,o,p) - b * Matrix3x3_Determinant(e,g,h,i,k,l,m,o,p) + c * Matrix3x3_Determinant(e,f,h,i,j,l,m,n,p) - d * Matrix3x3_Determinant(e,f,g,i,j,k,m,n,o);
}

namespace Helix {

// ****************************************************************************
// ****************************************************************************
Matrix3x3::Matrix3x3()
{
	SetIdentity();
}

// ****************************************************************************
// ****************************************************************************
Matrix3x3::Matrix3x3(const Matrix3x3 &other)
{
	for (int i = 0; i < NUM_ELEMENTS; i++)
		e[i] = other.e[i];
}

// ****************************************************************************
// ****************************************************************************
Matrix3x3::Matrix3x3(const Vector3 &r1, const Vector3 &r2, const Vector3 &r3)
{
	r[0][0] = r1.x;
	r[0][1] = r1.y;
	r[0][2] = r1.z;

	r[1][0] = r2.x;
	r[1][1] = r2.y;
	r[1][2] = r2.z;

	r[2][0] = r3.x;
	r[2][1] = r3.y;
	r[2][2] = r3.z;
}

// ****************************************************************************
// ****************************************************************************
Matrix3x3 & Matrix3x3::SetIdentity()
{
	r[0][0] = 1.0f;
	r[0][1] = 0.0f;
	r[0][2] = 0.0f;

	r[1][0] = 0.0f;
	r[1][1] = 1.0f;
	r[1][2] = 0.0f;

	r[2][0] = 0.0f;
	r[2][1] = 0.0f;
	r[2][2] = 1.0f;

	return *this;
}

// ****************************************************************************
// ****************************************************************************
Matrix3x3 & Matrix3x3::SetXRotation(float radians)
{
	SetIdentity();
	float cosTheta = cos(radians);
	float sinTheta = sin(radians);
	r[1][1] = cosTheta;
	r[1][2] = -1.0f * sinTheta;
	r[2][1] = sinTheta;
	r[2][2] = cosTheta;
	return *this;
}

// ****************************************************************************
// ****************************************************************************
Matrix3x3 & Matrix3x3::SetYRotation(float radians)
{
	SetIdentity();
	float cosTheta = cos(radians);
	float sinTheta = sin(radians);
	r[0][0] = cosTheta;
	r[0][2] = sinTheta;
	r[2][0] = -1.0f * sinTheta;
	r[2][2] = cosTheta;
	return *this;
}

// ****************************************************************************
// ****************************************************************************
Matrix3x3 & Matrix3x3::SetZRotation(float radians)
{
	SetIdentity();
	float cosTheta = cos(radians);
	float sinTheta = sin(radians);
	r[0][0] = cosTheta;
	r[0][1] = -1.0f * sinTheta;
	r[1][0] = sinTheta;
	r[1][1] = cosTheta;
	return *this;
}

// ****************************************************************************
// ****************************************************************************
Matrix3x3 & Matrix3x3::Transpose()
{
	Matrix3x3 t(*this);

	r[0][1] = t.r[1][0];
	r[0][2] = t.r[2][0];

	r[1][0] = t.r[0][1];
	r[1][2] = t.r[2][1];

	r[2][0] = t.r[0][2];
	r[2][1] = t.r[1][2];

	return *this;
}
// ****************************************************************************
// Transform vector by matrix
// ****************************************************************************
Matrix3x3 Matrix3x3::operator*(const Matrix3x3 &other)
{
	Matrix3x3 result;
	result.r[0][0] =	r[0][0] * other.r[0][0] + 
						r[0][1] * other.r[1][0] + 
						r[0][2] * other.r[2][0];

	result.r[0][1] =	r[0][0] * other.r[0][1] + 
						r[0][1] * other.r[1][1] + 
						r[0][2] * other.r[2][1];

	result.r[0][2] =	r[0][0] * other.r[0][2] + 
						r[0][1] * other.r[1][2] + 
						r[0][2] * other.r[2][2];

	result.r[1][0] =	r[1][0] * other.r[0][0] + 
						r[1][1] * other.r[1][0] + 
						r[1][2] * other.r[2][0];

	result.r[1][1] =	r[1][0] * other.r[0][1] + 
						r[1][1] * other.r[1][1] + 
						r[1][2] * other.r[2][1];
	
	result.r[1][2] =	r[1][0] * other.r[0][2] + 
						r[1][1] * other.r[1][2] + 
						r[1][2] * other.r[2][2];

	result.r[2][0] =	r[2][0] * other.r[0][0] + 
						r[2][1] * other.r[1][0] + 
						r[2][2] * other.r[2][0];

	result.r[2][1] =	r[2][0] * other.r[0][1] + 
						r[2][1] * other.r[1][1] + 
						r[2][2] * other.r[2][1];

	result.r[2][2] =	r[2][0] * other.r[0][2] + 
						r[2][1] * other.r[1][2] + 
						r[2][2] * other.r[2][2];

	return result;
}

// ****************************************************************************
// Transform this vector by matrix
// ****************************************************************************
Vector3 Matrix3x3::operator*(const Vector3 & rhs) const
{
	Vector3 result;
	result.x = r[0][0] * rhs.x + r[0][1] * rhs.y + r[0][2] * rhs.z;
	result.y = r[1][0] * rhs.x + r[1][1] * rhs.y + r[1][2] * rhs.z;
	result.z = r[2][0] * rhs.x + r[2][1] * rhs.y + r[2][2] * rhs.z;
	return result;
}

// ****************************************************************************
// 4x4 Matrix
// ****************************************************************************
Matrix4x4::Matrix4x4()
{
	SetIdentity();
}

// ****************************************************************************
// ****************************************************************************
Matrix4x4::Matrix4x4(const Matrix4x4 &other)
{
	for (int i = 0; i < NUM_ELEMENTS; i++)
		e[i] = other.e[i];
}

// ****************************************************************************
// ****************************************************************************
Matrix4x4::Matrix4x4(const Vector4 &r1, const Vector4 &r2, const Vector4 &r3, const Vector4 &r4)
{
	r[0][0] = r1.x;
	r[0][1] = r1.y;
	r[0][2] = r1.z;
	r[0][3] = r1.w;

	r[1][0] = r2.x;
	r[1][1] = r2.y;
	r[1][2] = r2.z;
	r[1][3] = r2.w;

	r[2][0] = r3.x;
	r[2][1] = r3.y;
	r[2][2] = r3.z;
	r[2][3] = r3.w;

	r[3][0] = r4.x;
	r[3][1] = r4.y;
	r[3][2] = r4.z;
	r[3][3] = r4.w;
}

// ****************************************************************************
// ****************************************************************************
Matrix4x4 & Matrix4x4::SetIdentity()
{
	r[0][0] = 1.0f;
	r[0][1] = 0.0f;
	r[0][2] = 0.0f;
	r[0][3] = 0.0f;

	r[1][0] = 0.0f;
	r[1][1] = 1.0f;
	r[1][2] = 0.0f;
	r[1][3] = 0.0f;

	r[2][0] = 0.0f;
	r[2][1] = 0.0f;
	r[2][2] = 1.0f;
	r[2][3] = 0.0f;

	r[3][0] = 0.0f;
	r[3][1] = 0.0f;
	r[3][2] = 0.0f;
	r[3][3] = 1.0f;
	return *this;
}

// ****************************************************************************
// ****************************************************************************
Matrix4x4 & Matrix4x4::SetXRotation(float radians)
{
	SetIdentity();
	float cosTheta = cos(radians);
	float sinTheta = sin(radians);
	r[1][1] = cosTheta;
	r[1][2] = -1.0f * sinTheta;
	r[2][1] = sinTheta;
	r[2][2] = cosTheta;
	return *this;
}

// ****************************************************************************
// ****************************************************************************
Matrix4x4 & Matrix4x4::SetYRotation(float radians)
{
	SetIdentity();
	float cosTheta = cos(radians);
	float sinTheta = sin(radians);
	r[0][0] = cosTheta;
	r[0][2] = sinTheta;
	r[2][0] = -1.0f * sinTheta;
	r[2][2] = cosTheta;
	return *this;
}

// ****************************************************************************
// ****************************************************************************
Matrix4x4 & Matrix4x4::SetZRotation(float radians)
{
	SetIdentity();
	float cosTheta = cos(radians);
	float sinTheta = sin(radians);
	r[0][0] = cosTheta;
	r[0][1] = -1.0f * sinTheta;
	r[1][0] = sinTheta;
	r[1][1] = cosTheta;
	return *this;
}

// ****************************************************************************
// Matrix concatenate
// ****************************************************************************
Matrix4x4 Matrix4x4::operator*(const Matrix4x4 &other)
{
	Matrix4x4 result;
	result.r[0][0] =	r[0][0] * other.r[0][0] + 
						r[0][1] * other.r[1][0] + 
						r[0][2] * other.r[2][0] + 
						r[0][3] * other.r[3][0];

	result.r[0][1] =	r[0][0] * other.r[0][1] + 
						r[0][1] * other.r[1][1] + 
						r[0][2] * other.r[2][1] + 
						r[0][3] * other.r[3][1];

	result.r[0][2] =	r[0][0] * other.r[0][2] + 
						r[0][1] * other.r[1][2] + 
						r[0][2] * other.r[2][2] + 
						r[0][3] * other.r[3][2];

	result.r[0][3] =	r[0][0] * other.r[0][3] + 
						r[0][1] * other.r[1][3] + 
						r[0][2] * other.r[2][3] + 
						r[0][3] * other.r[3][3];

	result.r[1][0] =	r[1][0] * other.r[0][0] + 
						r[1][1] * other.r[1][0] + 
						r[1][2] * other.r[2][0] +
						r[1][3] * other.r[3][0];

	result.r[1][1] =	r[1][0] * other.r[0][1] + 
						r[1][1] * other.r[1][1] + 
						r[1][2] * other.r[2][1] +
						r[1][3] * other.r[3][1];

	result.r[1][2] =	r[1][0] * other.r[0][2] + 
						r[1][1] * other.r[1][2] + 
						r[1][2] * other.r[2][2] +
						r[1][3] * other.r[3][2];

	result.r[1][3] =	r[1][0] * other.r[0][3] + 
						r[1][1] * other.r[1][3] + 
						r[1][2] * other.r[2][3] +
						r[1][3] * other.r[3][3];

	result.r[2][0] =	r[2][0] * other.r[0][0] + 
						r[2][1] * other.r[1][0] + 
						r[2][2] * other.r[2][0] +
						r[2][3] * other.r[3][0];

	result.r[2][1] =	r[2][0] * other.r[0][1] + 
						r[2][1] * other.r[1][1] + 
						r[2][2] * other.r[2][1] +
						r[2][3] * other.r[3][1];

	result.r[2][2] =	r[2][0] * other.r[0][2] + 
						r[2][1] * other.r[1][2] + 
						r[2][2] * other.r[2][2] +
						r[2][3] * other.r[3][2];

	result.r[2][3] =	r[2][0] * other.r[0][3] + 
						r[2][1] * other.r[1][3] + 
						r[2][2] * other.r[2][3] +
						r[2][3] * other.r[3][3];

	result.r[3][0] =	r[3][0] * other.r[0][0] + 
						r[3][1] * other.r[1][0] + 
						r[3][2] * other.r[2][0] +
						r[3][3] * other.r[3][0];

	result.r[3][1] =	r[3][0] * other.r[0][1] + 
						r[3][1] * other.r[1][1] + 
						r[3][2] * other.r[2][1] +
						r[3][3] * other.r[3][1];

	result.r[3][2] =	r[3][0] * other.r[0][2] + 
						r[3][1] * other.r[1][2] + 
						r[3][2] * other.r[2][2] +
						r[3][3] * other.r[3][2];

	result.r[3][3] =	r[3][0] * other.r[0][3] + 
						r[3][1] * other.r[1][3] + 
						r[3][2] * other.r[2][3] +
						r[3][3] * other.r[3][3];

	return result;
}

// ****************************************************************************
// Transform vector by matrix
// ****************************************************************************
Vector4 Matrix4x4::operator*(const Vector4 & rhs) const
{
	Vector4 result;
	result.x = r[0][0] * rhs.x + r[0][1] * rhs.y + r[0][2] * rhs.z + r[0][3] * rhs.w;
	result.y = r[1][0] * rhs.x + r[1][1] * rhs.y + r[1][2] * rhs.z + r[1][3] * rhs.w;
	result.z = r[2][0] * rhs.x + r[2][1] * rhs.y + r[2][2] * rhs.z + r[2][3] * rhs.w;
	result.w = r[3][0] * rhs.x + r[3][1] * rhs.y + r[3][2] * rhs.z + r[3][3] * rhs.w;
	return result;
}

// ****************************************************************************
// ****************************************************************************
Matrix4x4 & Matrix4x4::SetProjectionFOV(float fovY, float aspect, float nearZ, float farZ)
{
	SetIdentity();
	float yScale = 1.0f / tan(fovY / 2.0f);
	float xScale = yScale / aspect;

	r[0][0] = xScale;
	r[1][1] = yScale;
	r[2][2] = farZ / (farZ - nearZ);
	r[2][3] = (-1.0f * farZ) / (farZ - nearZ);
	r[3][2] = 1.0f;
	r[3][3] = 0.0f;

	return *this;
}

// ****************************************************************************
// ***************************************************************************
Matrix4x4 & Matrix4x4::SetProjection(float width, float height, float nearZ, float farZ)
{
	SetIdentity();
	r[0][0] = 2.0f * nearZ / width;
	r[1][1] = 2.0f * nearZ / height;
	r[2][2] = farZ / (farZ - nearZ);
	r[2][3] = (nearZ * farZ) / (nearZ - farZ);
	r[3][2] = 1.0f;
	r[3][3] = 0.0f;

	return *this;
}

// ****************************************************************************
// ***************************************************************************
Matrix4x4 & Matrix4x4::SetTranslation(float x, float y, float z)
{
	SetIdentity();
	r[0][3] = x;
	r[1][3] = y;
	r[2][3] = z;

	return *this;
}

// ****************************************************************************
// ***************************************************************************
Matrix4x4 & Matrix4x4::SetTranslation(Vector4 &vec)
{
	SetIdentity();
	r[0][3] = vec.x;
	r[1][3] = vec.y;
	r[2][3] = vec.z;

	return *this;
}

// ****************************************************************************
// ***************************************************************************
Matrix4x4 &  Matrix4x4::SetScale(float x, float y, float z, float w)
{
	SetIdentity();
	r[0][0] = x;
	r[1][1] = y;
	r[2][2] = z;
	r[3][3] = w;

	return *this;
}

// ****************************************************************************
// ***************************************************************************
inline float Matrix4x4::Determinant() const
{
	float result = Matrix4x4_Determinant(r[0][0], r[0][1], r[0][2], r[0][3],
							r[1][0], r[1][1], r[1][2], r[1][3],
							r[2][0], r[2][1], r[2][2], r[2][3],
							r[3][0], r[3][1], r[3][2], r[3][3]);
	return result;
}

// ****************************************************************************
// ****************************************************************************
inline void Matrix4x4::Scale(float factor)
{
	for(int i=0;i<NUM_ELEMENTS;i++)
		e[i] *= factor;


}

// ****************************************************************************
// ****************************************************************************
inline Matrix4x4 & Matrix4x4::Transpose()
{
	Matrix4x4 t(*this);

	r[0][0] = t.r[0][0];
	r[0][1] = t.r[1][0];
	r[0][2] = t.r[2][0];
	r[0][3] = t.r[3][0];

	r[1][0] = t.r[0][1];
	r[1][1] = t.r[1][1];
	r[1][2] = t.r[2][1];
	r[1][3] = t.r[3][1];

	r[2][0] = t.r[0][2];
	r[2][1] = t.r[1][2];
	r[2][2] = t.r[2][2];
	r[2][3] = t.r[3][2];

	r[3][0] = t.r[0][3];
	r[3][1] = t.r[1][3];
	r[3][2] = t.r[2][3];
	r[3][3] = t.r[3][3];

	return *this;
}

// ****************************************************************************
// ****************************************************************************
Matrix4x4 & Matrix4x4::Cofactor(const Matrix4x4 &other)
{
	r[0][0] = Matrix3x3_Determinant(	other.r[1][1], other.r[1][2], other.r[1][3],
							other.r[2][1], other.r[2][2], other.r[2][3],
							other.r[3][1], other.r[3][2], other.r[3][3]);
	r[0][1] = -1.0f * Matrix3x3_Determinant(	other.r[1][0], other.r[1][2], other.r[1][3],
									other.r[2][0], other.r[2][2], other.r[2][3],
									other.r[3][0], other.r[3][2], other.r[3][3]);
	r[0][2] = Matrix3x3_Determinant(	other.r[1][0], other.r[1][1], other.r[1][3],
							other.r[2][0], other.r[2][1], other.r[2][3], 
							other.r[3][0], other.r[3][1], other.r[3][3]);
	r[0][3] = -1.0f * Matrix3x3_Determinant(	other.r[1][0], other.r[1][1], other.r[1][2],
									other.r[2][0], other.r[2][1], other.r[2][2],
									other.r[3][0], other.r[3][1], other.r[3][2]);

	r[1][0] = -1.0f * Matrix3x3_Determinant(	other.r[0][1], other.r[0][2], other.r[0][3],
									other.r[2][1], other.r[2][2], other.r[2][3],
									other.r[3][1], other.r[3][2], other.r[3][3]);
	r[1][1] = Matrix3x3_Determinant(	other.r[0][0], other.r[0][2], other.r[0][3],
							other.r[2][0], other.r[2][2], other.r[2][3],
							other.r[3][0], other.r[3][2], other.r[3][3]);
	r[1][2] = -1.0f * Matrix3x3_Determinant(	other.r[0][0], other.r[0][1], other.r[0][3],
									other.r[2][0], other.r[2][1], other.r[2][3],
									other.r[3][0], other.r[3][1], other.r[3][3]);
	r[1][3] = Matrix3x3_Determinant(	other.r[0][0], other.r[0][1], other.r[0][2],
							other.r[2][0], other.r[2][1], other.r[2][2],
							other.r[3][0], other.r[3][1], other.r[3][2]);

	r[2][0] = Matrix3x3_Determinant(	other.r[0][1], other.r[0][2], other.r[0][3],
							other.r[1][1], other.r[1][2], other.r[1][3],
							other.r[3][1], other.r[3][2], other.r[3][3]);
	r[2][1] = -1.0f * Matrix3x3_Determinant(	other.r[0][0], other.r[0][2], other.r[0][3],
									other.r[1][0], other.r[1][2], other.r[1][3],
									other.r[3][0], other.r[3][2], other.r[3][3]);
	r[2][2] = Matrix3x3_Determinant(	other.r[0][0], other.r[0][1], other.r[0][3],
							other.r[1][0], other.r[1][1], other.r[1][3],
							other.r[3][0], other.r[3][1], other.r[3][3]);
	r[2][3] = -1.0f * Matrix3x3_Determinant(	other.r[0][0], other.r[0][1], other.r[0][2],
									other.r[1][0], other.r[1][1], other.r[1][2],
									other.r[3][0], other.r[3][1], other.r[3][2]);

	r[3][0] = -1.0f * Matrix3x3_Determinant(	other.r[0][1], other.r[0][2], other.r[0][3],
									other.r[1][1], other.r[1][2], other.r[1][3],
									other.r[2][1], other.r[2][2], other.r[2][3]);
	r[3][1] = Matrix3x3_Determinant(	other.r[0][0], other.r[0][2], other.r[0][3],
							other.r[1][0], other.r[1][2], other.r[1][3],
							other.r[2][0], other.r[2][2], other.r[2][3]);
	r[3][2] = -1.0f * Matrix3x3_Determinant(	other.r[0][0], other.r[0][1], other.r[0][3],
									other.r[1][0], other.r[1][1], other.r[1][3],
									other.r[2][0], other.r[2][1], other.r[2][3]);
	r[3][3] = Matrix3x3_Determinant(	other.r[0][0], other.r[0][1], other.r[0][2],
							other.r[1][0], other.r[1][1], other.r[1][2],
							other.r[2][0], other.r[2][1], other.r[2][2]);

	return *this;
}

// ****************************************************************************
// ****************************************************************************
bool Matrix4x4::Invert()
{
	// Transposed cofactors
// 	r[0][0] = r[1][2]*r[2][3]*r[3][1] - r[1][3]*r[2][2]*r[3][1] + r[1][3]*r[2][1]*r[3][2] - r[1][1]*r[2][3]*r[3][2] - r[1][2]*r[2][1]*r[3][3] + r[1][1]*r[2][2]*r[3][3];
// 	r[0][1] = r[0][3]*r[2][2]*r[3][1] - r[0][2]*r[2][3]*r[3][1] - r[0][3]*r[2][1]*r[3][2] + r[0][1]*r[2][3]*r[3][2] + r[0][2]*r[2][1]*r[3][3] - r[0][1]*r[2][2]*r[3][3];
// 	r[0][2] = r[0][2]*r[1][3]*r[3][1] - r[0][3]*r[1][2]*r[3][1] + r[0][3]*r[1][1]*r[3][2] - r[0][1]*r[1][3]*r[3][2] - r[0][2]*r[1][1]*r[3][3] + r[0][1]*r[1][2]*r[3][3];
// 	r[0][3] = r[0][3]*r[1][2]*r[2][1] - r[0][2]*r[1][3]*r[2][1] - r[0][3]*r[1][1]*r[2][2] + r[0][1]*r[1][3]*r[2][2] + r[0][2]*r[1][1]*r[2][3] - r[0][1]*r[1][2]*r[2][3];
// 	r[1][0] = r[1][3]*r[2][2]*r[3][0] - r[1][2]*r[2][3]*r[3][0] - r[1][3]*r[2][0]*r[3][2] + r[1][0]*r[2][3]*r[3][2] + r[1][2]*r[2][0]*r[3][3] - r[1][0]*r[2][2]*r[3][3];
// 	r[1][1] = r[0][2]*r[2][3]*r[3][0] - r[0][3]*r[2][2]*r[3][0] + r[0][3]*r[2][0]*r[3][2] - r[0][0]*r[2][3]*r[3][2] - r[0][2]*r[2][0]*r[3][3] + r[0][0]*r[2][2]*r[3][3];
// 	r[1][2] = r[0][3]*r[1][2]*r[3][0] - r[0][2]*r[1][3]*r[3][0] - r[0][3]*r[1][0]*r[3][2] + r[0][0]*r[1][3]*r[3][2] + r[0][2]*r[1][0]*r[3][3] - r[0][0]*r[1][2]*r[3][3];
// 	r[1][3] = r[0][2]*r[1][3]*r[2][0] - r[0][3]*r[1][2]*r[2][0] + r[0][3]*r[1][0]*r[2][2] - r[0][0]*r[1][3]*r[2][2] - r[0][2]*r[1][0]*r[2][3] + r[0][0]*r[1][2]*r[2][3];
// 	r[2][0] = r[1][1]*r[2][3]*r[3][0] - r[1][3]*r[2][1]*r[3][0] + r[1][3]*r[2][0]*r[3][1] - r[1][0]*r[2][3]*r[3][1] - r[1][1]*r[2][0]*r[3][3] + r[1][0]*r[2][1]*r[3][3];
// 	r[2][1] = r[0][3]*r[2][1]*r[3][0] - r[0][1]*r[2][3]*r[3][0] - r[0][3]*r[2][0]*r[3][1] + r[0][0]*r[2][3]*r[3][1] + r[0][1]*r[2][0]*r[3][3] - r[0][0]*r[2][1]*r[3][3];
// 	r[2][2] = r[0][1]*r[1][3]*r[3][0] - r[0][3]*r[1][1]*r[3][0] + r[0][3]*r[1][0]*r[3][1] - r[0][0]*r[1][3]*r[3][1] - r[0][1]*r[1][0]*r[3][3] + r[0][0]*r[1][1]*r[3][3];
// 	r[2][3] = r[0][3]*r[1][1]*r[2][0] - r[0][1]*r[1][3]*r[2][0] - r[0][3]*r[1][0]*r[2][1] + r[0][0]*r[1][3]*r[2][1] + r[0][1]*r[1][0]*r[2][3] - r[0][0]*r[1][1]*r[2][3];
// 	r[3][0] = r[1][2]*r[2][1]*r[3][0] - r[1][1]*r[2][2]*r[3][0] - r[1][2]*r[2][0]*r[3][1] + r[1][0]*r[2][2]*r[3][1] + r[1][1]*r[2][0]*r[3][2] - r[1][0]*r[2][1]*r[3][2];
// 	r[3][1] = r[0][1]*r[2][2]*r[3][0] - r[0][2]*r[2][1]*r[3][0] + r[0][2]*r[2][0]*r[3][1] - r[0][0]*r[2][2]*r[3][1] - r[0][1]*r[2][0]*r[3][2] + r[0][0]*r[2][1]*r[3][2];
// 	r[3][2] = r[0][2]*r[1][1]*r[3][0] - r[0][1]*r[1][2]*r[3][0] - r[0][2]*r[1][0]*r[3][1] + r[0][0]*r[1][2]*r[3][1] + r[0][1]*r[1][0]*r[3][2] - r[0][0]*r[1][1]*r[3][2];
// 	r[3][3] = r[0][1]*r[1][2]*r[2][0] - r[0][2]*r[1][1]*r[2][0] + r[0][2]*r[1][0]*r[2][1] - r[0][0]*r[1][2]*r[2][1] - r[0][1]*r[1][0]*r[2][2] + r[0][0]*r[1][1]*r[2][2];

	float det = Determinant();
	if(det == 0.0f)
		return false;

	Matrix4x4 t = *this;
	Cofactor(t);
	Transpose();
	Scale(1.0f/det);
	
	return true;
}
} // namespace Helix
