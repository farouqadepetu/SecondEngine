#pragma once

#include "vectors.h"
#include "matrices.h"

class Quaternion
{
public:
	Quaternion();
	Quaternion(float x, float y, float z, float scalar);

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
	Quaternion& operator+=(const Quaternion& b);

	//Returns the result of this - b.
	Quaternion& operator-=(const Quaternion& b);

	//Returns the result of this * b (component multiplication).
	Quaternion& operator*=(const Quaternion& b);

	//Returns the result of this * k.
	Quaternion& operator*=(float k);

	//Returns the result of this / k.
	Quaternion& operator/=(float k);

	//Returns true if this equals to vec.
	bool operator==(const Quaternion& b) const;

	//Returns true if this does not equal to vec.
	bool operator!=(const Quaternion& b) const;

	//Returns the identity quaternion
	static Quaternion MakeIdentity();

	//Returns a quaternion representing a rotation about an axis.
	//Angle should be given in degrees.
	static Quaternion MakeRotation(float angle, float x, float y, float z);

	//Returns a quaternion representing a rotation about an axis.
	//Angle should be given in degrees.
	static Quaternion MakeRotation(float angle, const Vector3& axis);

	//Returns the result of a + b.
	friend Quaternion operator+(const Quaternion& a, const Quaternion& b);

	//Returns the result of a - b.
	friend Quaternion operator-(const Quaternion& a, const Quaternion& b);

	//Returns the result of the negation of a;
	friend Quaternion operator-(const Quaternion& a);

	//Returns the result of the component multiplication between a and b.
	friend Quaternion operator*(const Quaternion& a, const Quaternion& b);

	//Returns the result of a * k.
	friend Quaternion operator*(const Quaternion& a, float k);

	//Returns the result of k * a.
	friend Quaternion operator*(float k, const Quaternion& a);

	//Returns the result of a / k.
	friend Quaternion operator/(const Quaternion& a, float k);

	//Returns the result of a dot b.
	friend float DotProduct(const Quaternion& a, const Quaternion& b);

	//Returns the magntidue of a.
	friend float Length(const Quaternion& a);

	//Returns a unit quaternion by normalizing a.
	friend Quaternion Normalize(const Quaternion& a);

	//Returns the conjugate of a.
	friend Quaternion Conjugate(const Quaternion& a);

	//Returns the inverse of a.
	friend Quaternion Inverse(const Quaternion& a);

	//Returns true if the quaternion is an identity quaternion, false otherwise.
	friend bool IsIdentity(const Quaternion& a);

	//Rotates the vector p by quaternion q.
	//Quaternion q must be a rotation quaternion.
	friend Vector3 Rotate(const Quaternion& q, const Vector3& p);

	//Rotates the vector p by quaternion q.
	//Quaternion q must be a rotation quaternion.
	friend Vector4 Rotate(const Quaternion& q, const Vector4& p);

	//Returns the matrix representation of quaterion q.
	friend Matrix4x4 QuaternionToMatrix(const Quaternion& q);

private:
	float x;
	float y;
	float z;
	float scalar;
};

#ifdef _DEBUG
#include <iostream>
inline void Print(const Quaternion& a)
{
	std::cout << "(" << a.GetX() << ", ";
	std::cout << a.GetY() << ", ";
	std::cout << a.GetZ() << ", ";
	std::cout << a.GetScalar() << ")" << std::endl;
}
#endif

typedef Quaternion quat;