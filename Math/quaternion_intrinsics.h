#pragma once

#include "vectors_intrinsics.h"
#include "matrices_intrinsics.h"

class Quaternion_Intrinsics
{
public:
	Quaternion_Intrinsics();
	Quaternion_Intrinsics(float x, float y, float z, float scalar);

	float GetX() const;
	float GetY() const;
	float GetZ() const;
	float GetScalar() const;

	void SetX(float x);
	void SetY(float y);
	void SetZ(float z);
	void SetScalar(float scalar);
	void Set(float x, float y, float z, float scalar);

	//Returns the result of this + b.
	Quaternion_Intrinsics& operator+=(const Quaternion_Intrinsics& b);

	//Returns the result of this - b.
	Quaternion_Intrinsics& operator-=(const Quaternion_Intrinsics& b);

	//Returns the result of this * b (component multiplication).
	Quaternion_Intrinsics& operator*=(const Quaternion_Intrinsics& b);

	//Returns the result of this * k.
	Quaternion_Intrinsics& operator*=(float k);

	//Returns the result of this / k.
	Quaternion_Intrinsics& operator/=(float k);

	//Returns true if this equals to vec.
	bool operator==(const Quaternion_Intrinsics& b) const;

	//Returns true if this does not equal to vec.
	bool operator!=(const Quaternion_Intrinsics& b) const;

	//Returns the identity Quaternion_Intrinsics
	static Quaternion_Intrinsics MakeIdentity();

	//Returns a Quaternion_Intrinsics representing a rotation about an axis.
	//Angle should be given in degrees.
	static Quaternion_Intrinsics MakeRotation(float angle, float x, float y, float z);

	//Returns a Quaternion_Intrinsics representing a rotation about an axis.
	//Angle should be given in degrees.
	static Quaternion_Intrinsics MakeRotation(float angle, const Vector3_Intrinsics& axis);

	//Returns the result of a + b.
	friend Quaternion_Intrinsics operator+(const Quaternion_Intrinsics& a, const Quaternion_Intrinsics& b);

	//Returns the result of a - b.
	friend Quaternion_Intrinsics operator-(const Quaternion_Intrinsics& a, const Quaternion_Intrinsics& b);

	//Returns the result of the negation of a;
	friend Quaternion_Intrinsics operator-(const Quaternion_Intrinsics& a);

	//Returns the result of the component multiplication between a and b.
	friend Quaternion_Intrinsics operator*(const Quaternion_Intrinsics& a, const Quaternion_Intrinsics& b);

	//Returns the result of a * k.
	friend Quaternion_Intrinsics operator*(const Quaternion_Intrinsics& a, float k);

	//Returns the result of k * a.
	friend Quaternion_Intrinsics operator*(float k, const Quaternion_Intrinsics& a);

	//Returns the result of a / k.
	friend Quaternion_Intrinsics operator/(const Quaternion_Intrinsics& a, float k);

	//Returns the result of a dot b.
	friend float DotProduct(const Quaternion_Intrinsics& a, const Quaternion_Intrinsics& b);

	//Returns the magntidue of a.
	friend float Length(const Quaternion_Intrinsics& a);

	//Returns a unit quaternion by normalizing a.
	friend Quaternion_Intrinsics Normalize(const Quaternion_Intrinsics& a);

	//Returns the conjugate of a.
	friend Quaternion_Intrinsics Conjugate(const Quaternion_Intrinsics& a);

	//Returns the inverse of a.
	friend Quaternion_Intrinsics Inverse(const Quaternion_Intrinsics& a);

	//Returns true if the quaternion is the identity quaternion, false otherwise.
	friend bool IsIdentity(const Quaternion_Intrinsics& a);

	//Rotates the vector p by quaternion q.
	//Quaternion_Intrinsics q must be a rotation quaternion.
	friend Vector3_Intrinsics Rotate(const Quaternion_Intrinsics& q, const Vector3_Intrinsics& p);

	//Rotates the vector p by quaternion q.
	//Quaternion_Intrinsics q must be a rotation quaternion.
	friend Vector4_Intrinsics Rotate(const Quaternion_Intrinsics& q, const Vector4_Intrinsics& p);

	//Returns the matrix representation of quaterion q.
	friend Matrix4x4_Intrinsics QuaternionToMatrix(const Quaternion_Intrinsics& q);

private:
	Quaternion_Intrinsics(__m128 quat);

private:
	union
	{
		__m128 quat;
		float quatA[4];
	};
};

#ifdef _DEBUG
#include <iostream>
inline void Print(const Quaternion_Intrinsics& a)
{
	std::cout << "(" << a.GetX() << ", ";
	std::cout << a.GetY() << ", ";
	std::cout << a.GetZ() << ", ";
	std::cout << a.GetScalar() << ")" << std::endl;
}
#endif

typedef Quaternion_Intrinsics quat;
