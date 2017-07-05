#ifndef SPELFYSIK_SLUTUPPGIFT_VECTOR3F_H
#define SPELFYSIK_SLUTUPPGIFT_VECTOR3F_H

#include <cmath>
#include <cassert>

class Vector3f
{
public:
	inline Vector3f(float x, float y, float z)
			: x(x), y(y), z(z) {}
	inline Vector3f(const Vector3f& rhs)
			: x(rhs.x), y(rhs.y), z(rhs.z) {}
	inline Vector3f& operator=(const Vector3f &rhs)
	{
		x = rhs.x; y = rhs.y; z = rhs.z;
		return *this;
	}
	inline Vector3f& operator+=(const Vector3f &rhs)
	{
		x += rhs.x; y += rhs.y; z += rhs.z;
		return *this;
	}
	inline Vector3f& operator-=(const Vector3f &rhs)
	{
		x -= rhs.x; y -= rhs.y; z -= rhs.z;
		return *this;
	}
	inline Vector3f& operator*=(const float &scalar)
	{
		x *= scalar; y *= scalar; z *= scalar;
		return *this;
	}
	inline float length() const
	{
		return std::sqrt(x*x + y*y + z*z);
	}
	inline float length_squared() const
	{
		return (x*x + y*y + z*z);
	}
	inline void normalize()
	{
		assert(!(x == 0.0f && y == 0.0f && z == 0.0f));
		float inv_length = 1.0f/length();
		x *= inv_length;
		y *= inv_length;
		z *= inv_length;
	}
	inline Vector3f get_normalized() const
	{
		Vector3f retval(*this);
		retval.normalize();
		return retval;
	}
	inline float get_x() const
	{
		return x;
	}
	inline float get_y() const
	{
		return y;
	}
	inline float get_z() const
	{
		return z;
	}
	// Dot product
	inline friend const float operator*(const Vector3f &lhs, const Vector3f &rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
	}
	inline friend bool operator==(const Vector3f &lhs, const Vector3f &rhs)
	{
		return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
	}

private:
	float x, y, z;
};


inline const Vector3f operator+(const Vector3f &lhs, const Vector3f &rhs)
{
	return Vector3f(lhs) += rhs;
}
inline const Vector3f operator-(const Vector3f &lhs, const Vector3f &rhs)
{
	return Vector3f(lhs) -= rhs;
}
inline const Vector3f operator*(const Vector3f &vector, float scalar)
{
	return Vector3f(vector) *= scalar;
}
inline const Vector3f operator*(float scalar, const Vector3f &vector)
{
	return Vector3f(vector) *= scalar;
}
inline bool operator!=(const Vector3f &lhs, const Vector3f &rhs)
{
	return !(lhs == rhs);
}

#endif //SPELFYSIK_SLUTUPPGIFT_VECTOR3F_H
