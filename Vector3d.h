#ifndef SPELFYSIK_SLUTUPPGIFT_VECTOR3D_H
#define SPELFYSIK_SLUTUPPGIFT_VECTOR3D_H

#include <cmath>
#include <cassert>

class Vector3d
{
public:
	inline Vector3d(double x, double y, double z)
			: x(x), y(y), z(z) {}
	inline Vector3d(const Vector3d& rhs)
			: x(rhs.x), y(rhs.y), z(rhs.z) {}
	inline Vector3d& operator=(const Vector3d &rhs)
	{
		x = rhs.x; y = rhs.y; z = rhs.z;
		return *this;
	}
	inline Vector3d& operator+=(const Vector3d &rhs)
	{
		x += rhs.x; y += rhs.y; z += rhs.z;
		return *this;
	}
	inline Vector3d& operator-=(const Vector3d &rhs)
	{
		x -= rhs.x; y -= rhs.y; z -= rhs.z;
		return *this;
	}
	inline Vector3d& operator*=(const double &scalar)
	{
		x *= scalar; y *= scalar; z *= scalar;
		return *this;
	}
	inline double length() const
	{
		return std::sqrt(x*x + y*y + z*z);
	}
	inline double length_squared() const
	{
		return (x*x + y*y + z*z);
	}
	inline void normalize()
	{
		assert(!(x == 0.0 && y == 0.0 && z == 0.0));
		double inv_length = 1.0/length();
		x *= inv_length;
		y *= inv_length;
		z *= inv_length;
	}
	inline Vector3d get_normalized() const
	{
		Vector3d retval(*this);
		retval.normalize();
		return retval;
	}
	inline double get_x() const
	{
		return x;
	}
	inline double get_y() const
	{
		return y;
	}
	inline double get_z() const
	{
		return z;
	}
	// Dot product
	inline friend const double operator*(const Vector3d &lhs, const Vector3d &rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
	}
	inline friend bool operator==(const Vector3d &lhs, const Vector3d &rhs)
	{
		return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
	}

private:
	double x, y, z;
};


inline const Vector3d operator+(const Vector3d &lhs, const Vector3d &rhs)
{
	return Vector3d(lhs) += rhs;
}
inline const Vector3d operator-(const Vector3d &lhs, const Vector3d &rhs)
{
	return Vector3d(lhs) -= rhs;
}
inline const Vector3d operator*(const Vector3d &vector, double scalar)
{
	return Vector3d(vector) *= scalar;
}
inline const Vector3d operator*(double scalar, const Vector3d &vector)
{
	return Vector3d(vector) *= scalar;
}
inline bool operator!=(const Vector3d &lhs, const Vector3d &rhs)
{
	return !(lhs == rhs);
}

#endif //SPELFYSIK_SLUTUPPGIFT_VECTOR3D_H
