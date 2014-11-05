#ifndef HVECTOR_H 
#define HVECTOR_H

namespace Helix
{
// Generic form
// VT = underlying type
// D = dimensions
template <typename VT, int D>
struct Vector
{
	Vector();
	Vector(const Vector<VT, D> &other);
	Vector(VT init[D]);
	Vector<VT, D>	operator+(const Vector<VT, D> &rhs) const;
	Vector<VT, D>	operator-(const Vector<VT, D> &rhs) const;

	template <typename T1> Vector<VT,D>	operator*(const T1 scalar) const;
	template <typename T1> Vector<VT,D>	operator/(const T1 scalar) const;

	void Zero();

	VT data[D];
};

// Generic methods
template <typename VT, int D>
Vector<VT, D>::Vector()
{
	for (int i = 0; i < D; i++)
		data[i] = (VT)0;
}

template <typename VT, int D>
Vector<VT, D>::Vector(const Vector<VT, D> &other)
{
	for (int i = 0; i < D; i++)
		data[i] = other.data[i];
}

template <typename VT, int D>
Vector<VT,D>::Vector(VT init[D])
{
	for (int i = 0; i < D; i++)
		data[i] = init[i];
}

template <typename VT, int D>
Vector<VT, D> Vector<VT,D>::operator+(const Vector<VT, D> &rhs) const
{
	Vector<VT, D> result;
	for (int i = 0; i < D; i++)
		result.data[i] = data[i] + rhs.data[i];

	return result;
}

template <typename VT, int D>
Vector<VT,D> Vector<VT,D>::operator-(const Vector<VT,D> &rhs) const
{
	Vector<VT, D> result;
	for (int i = 0; i < D; i++)
		result.data[i] = data[i] - rhs.data[i];
	return result;
}

template <typename VT, int D>
template <typename T>
Vector<VT, D> Vector<VT, D>::operator*(const T rhs) const 
{
	Vector<VT, D> result;
	for (int i = 0; i < D; i++)
		result.data[i] = data[i] * rhs;

	return result;
}

template <typename VT, int D>
template <typename T>
Vector<VT, D> Vector<VT, D>::operator/(const T rhs) const 
{
	Vector<VT, D> result;
	for (int i = 0; i < D; i++)
		result.data[i] = data[i] / rhs;

	return result;
}

template <typename VT, int D>
void Vector<VT, D>::Zero()
{
	for (int i = 0; i < D; i++)
		data[i] = (VT)0;
}


// Specializations

// // 2D float
// template <>
// struct Vector<float, 2>
// {
// 	union {
// 		float x, y;
// 		float data[2];
// 	};
// };

// 3D float
template <>
struct Vector<float, 3>
{
	union {
		float x, y, z;
		float data[3];
	};
};

// Constructor functions
Vector<float, 3> MakeFloatVector3(float x, float y, float z)
{
	Vector<float, 3> result;
	result.data[0] = x;
	result.data[1] = y;
	result.data[2] = z;
	return result;
}

// Short hand constructors
#define Vector3(x,y,z) Helix::MakeFloatVector3(x,y,z)

// Free standing operators

template <typename T1, typename VT, int D>
Vector<VT, D> operator+(const T1 scalar, const Vector<VT, D> &rhs)
{
	Vector<VT, D> result = rhs*scalar;
	return result;
}

template <typename T1, typename VT, int D>
Vector<VT, D> operator*(const T1 scalar, const Vector<VT, D> &rhs)
{
	// Flip the parameters and call the class version
	Vector<VT, D> result = rhs*scalar;
	return result;
}

// Short hand typedefs
typedef Vector<float, 3>	Vector3;
typedef Vector<float, 2>	Vector2;

} // namespace Helix
#endif // HVECTOR_H

