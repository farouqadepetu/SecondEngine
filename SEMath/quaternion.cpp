#include <cassert>
#include <cmath>
#include "quaternion.h"
#include "../Math/math_utility.h"

Quaternion::Quaternion() : x (0.0f), y (0.0f), z(0.0f), scalar(0.0f)
{}

Quaternion::Quaternion(float x, float y, float z, float scalar) : x(x), y(y), z(z), scalar(scalar)
{}

float Quaternion::GetX() const
{
	return x;
}

float Quaternion::GetY() const
{
	return y;
}

float Quaternion::GetZ() const
{
	return z;
}

float Quaternion::GetScalar() const
{
	return scalar;
}

void Quaternion::SetX(float x)
{
	this->x = x;
}

void Quaternion::SetY(float y)
{
	this->y = y;
}

void Quaternion::SetZ(float z)
{
	this->z = z;
}

void Quaternion::SetScalar(float scalar)
{
	this->scalar = scalar;
}

void Quaternion::Set(float x, float y, float z, float scalar)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->scalar = scalar;
}

Quaternion& Quaternion::operator+=(const Quaternion& b)
{
	this->x += b.x;
	this->y += b.y;
	this->z += b.z;
	this->scalar += b.scalar;

	return *this;
}

Quaternion& Quaternion::operator-=(const Quaternion& b)
{
	this->x -= b.x;
	this->y -= b.y;
	this->z -= b.z;
	this->scalar -= b.scalar;

	return *this;
}

Quaternion& Quaternion::operator*=(const Quaternion& b)
{
	float x = this->scalar * b.x + this->x * b.scalar + this->y * b.z - this->z * b.y;
	float y = this->scalar * b.y + this->y * b.scalar + this->z * b.x - this->x * b.z;
	float z = this->scalar * b.z + this->z * b.scalar + this->x * b.y - this->y * b.x;
	float scalar = this->scalar * b.scalar - this->x * b.x - this->y * b.y - this->z * b.z;

	this->x = x;
	this->y = y;
	this->z = z;
	this->scalar = scalar;

	return *this;
}

Quaternion& Quaternion::operator*=(float k)
{
	this->x *= k;
	this->y *= k;
	this->z *= k;
	this->scalar *= k;

	return *this;
}

Quaternion& Quaternion::operator/=(float k)
{
	this->x /= k;
	this->y /= k;
	this->z /= k;
	this->scalar /= k;

	return *this;
}

bool Quaternion::operator==(const Quaternion& b) const
{
	return CompareFloats(this->x, b.x) && CompareFloats(this->y, b.y) && CompareFloats(this->z, b.z) && CompareFloats(this->scalar, b.scalar);
}

bool Quaternion::operator!=(const Quaternion& b) const
{
	return !(*this == b);
}

Quaternion Quaternion::MakeIdentity()
{
	return Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
}

Quaternion Quaternion::MakeRotation(float angle, float x, float y, float z)
{
	float ang = (angle / 2.0f) * (PI / 180.0f);
	float cosAngle = cos(ang);
	float sinAngle = sin(ang);
	Vector3 axis = Normalize(Vector3(x, y, z));
	return Quaternion(sinAngle * axis.GetX(), sinAngle * axis.GetY(), sinAngle * axis.GetZ(), cosAngle);
}

Quaternion Quaternion::MakeRotation(float angle, const Vector3& axis)
{
	float ang = (angle / 2.0f) * (PI / 180.0f);
	float cosAngle = cos(ang);
	float sinAngle = sin(ang);
	Vector3 axisN = Normalize(axis);
	return Quaternion(sinAngle * axisN.GetX(), sinAngle * axisN.GetY(), sinAngle * axisN.GetZ(), cosAngle);
}

Quaternion operator+(const Quaternion& a, const Quaternion& b)
{
	return Quaternion(a.x + b.x, a.y + b.y, a.z + b.z, a.scalar + b.scalar);
}

Quaternion operator-(const Quaternion& a, const Quaternion& b)
{
	return Quaternion(a.x - b.x, a.y - b.y, a.z - b.z, a.scalar - b.scalar);
}

Quaternion operator-(const Quaternion& a)
{
	return Quaternion(-a.x, -a.y, -a.z, -a.scalar);
}

Quaternion operator*(const Quaternion& a, const Quaternion& b)
{
	float x = a.scalar * b.x + a.x * b.scalar + a.y * b.z - a.z * b.y;
	float y = a.scalar * b.y + a.y * b.scalar + a.z * b.x - a.x * b.z;
	float z = a.scalar * b.z + a.z * b.scalar + a.x * b.y - a.y * b.x;
	float scalar = a.scalar * b.scalar - a.x * b.x - a.y * b.y - a.z * b.z;

	return Quaternion(x, y, z, scalar);
}

Quaternion operator*(const Quaternion& a, float k)
{
	return Quaternion(a.x * k, a.y * k, a.z * k, a.scalar * k);
}

Quaternion operator*(float k, const Quaternion& a)
{
	return Quaternion(k * a.x, k * a.y, k * a.z, k * a.scalar);
}

Quaternion operator/(const Quaternion& a, float k)
{
	return Quaternion(a.x / k, a.y / k, a.z / k, a.scalar / k);
}

float DotProduct(const Quaternion& a, const Quaternion& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z * a.scalar * b.scalar;
}

float Length(const Quaternion& a)
{
	return sqrt(a.x * a.x + a.y * a.y + a.z * a.z + a.scalar * a.scalar);
}

Quaternion Normalize(const Quaternion& a)
{
	float mag = Length(a);
	assert(!CompareFloats(mag, 0.0f) && "Magnitude of the quaternion is 0.");

	return a / mag;
}

Quaternion Conjugate(const Quaternion& a)
{
	return Quaternion(-a.x, -a.y, -a.z, a.scalar);
}

Quaternion Inverse(const Quaternion& a)
{
	return Conjugate(a) / Length(a);
}

bool IsIdentity(const Quaternion& a)
{
	return CompareFloats(a.x, 0.0f) && CompareFloats(a.y, 0.0f) && CompareFloats(a.z, 0.0f) && CompareFloats(a.scalar, 1.0f);
}

Vector3 Rotate(const Quaternion& q, const Vector3& p)
{
	Quaternion point(p.GetX(), p.GetY(), p.GetZ(), 0.0f);

	Quaternion result = q * point * Conjugate(q);

	return Vector3(result.x, result.y, result.z);
}

Vector4 Rotate(const Quaternion& q, const Vector4& p)
{
	Quaternion point(p.GetX(), p.GetY(), p.GetZ(), 0.0f);

	Quaternion result = q * point * Conjugate(q);

	return Vector4(result.x, result.y, result.z, p.GetW());
}

Matrix4x4 QuaternionToMatrix(const Quaternion& q)
{
	float m00 = 1.0f - 2.0f * q.y * q.y - 2.0f * q.z * q.z;
	float m01 = 2.0f * q.x * q.y + 2.0f * q.scalar * q.z;
	float m02 = 2.0f * q.x * q.z - 2.0f * q.scalar * q.y;
	float m03 = 0.0f;

	float m10 = 2.0f * q.x * q.y - 2.0f * q.scalar * q.z;
	float m11 = 1.0f - 2.0f * q.x * q.x - 2.0f * q.z * q.z;
	float m12 = 2.0f * q.y * q.z + 2.0f * q.scalar * q.x;
	float m13 = 0.0f;

	float m20 = 2.0f * q.x * q.z + 2.0f * q.scalar * q.y;
	float m21 = 2.0f * q.y * q.z - 2.0f * q.scalar * q.x;
	float m22 = 1.0f - 2.0f * q.x * q.x - 2.0f * q.y * q.y;
	float m23 = 0.0f;

	float m30 = 0.0f;
	float m31 = 0.0f;
	float m32 = 0.0f;
	float m33 = 1.0f;

	return Matrix4x4(m00, m01, m02, m03,
					 m10, m11, m12, m13,
					 m20, m21, m22, m23,
					 m30, m31, m32, m33);
}
