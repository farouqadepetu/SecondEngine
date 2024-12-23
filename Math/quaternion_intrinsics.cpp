#include <cassert>
#include <cmath>
#include "quaternion_intrinsics.h"
#include "../Math/math_utility.h"

Quaternion_Intrinsics::Quaternion_Intrinsics()
{
	_mm_set_ps(0.0f, 0.0f, 0.0f, 0.0f);
}

Quaternion_Intrinsics::Quaternion_Intrinsics(float x, float y, float z, float scalar)
{
	_mm_set_ps(scalar, z, y, x);
}

Quaternion_Intrinsics::Quaternion_Intrinsics(__m128 quat)
{
	this->quat = quat;
}

float Quaternion_Intrinsics::GetX() const
{
	return quatA[0];
}

float Quaternion_Intrinsics::GetY() const
{ 
	return quatA[1];
}

float Quaternion_Intrinsics::GetZ() const
{
	return quatA[2];
}

float Quaternion_Intrinsics::GetScalar() const
{
	return quatA[3];
}

void Quaternion_Intrinsics::SetX(float x)
{
	quatA[0] = x;
}

void Quaternion_Intrinsics::SetY(float y)
{
	quatA[1] = y;
}

void Quaternion_Intrinsics::SetZ(float z)
{
	quatA[2] = z;
}

void Quaternion_Intrinsics::SetScalar(float scalar)
{
	quatA[3] = scalar;
}

void Quaternion_Intrinsics::Set(float x, float y, float z, float scalar)
{
	quatA[0] = x;
	quatA[1] = y;
	quatA[2] = z;
	quatA[3] = scalar;
}

Quaternion_Intrinsics& Quaternion_Intrinsics::operator+=(const Quaternion_Intrinsics& b)
{
	this->quat = _mm_add_ps(this->quat, b.quat);

	return *this;
}

Quaternion_Intrinsics& Quaternion_Intrinsics::operator-=(const Quaternion_Intrinsics& b)
{
	this->quat = _mm_sub_ps(this->quat, b.quat);

	return *this;
}

Quaternion_Intrinsics& Quaternion_Intrinsics::operator*=(const Quaternion_Intrinsics& b)
{
	/*float x = this->scalar * b.x + this->x * b.scalar + this->y * b.z - this->z * b.y;
	float y = this->scalar * b.y + this->y * b.scalar + this->z * b.x - this->x * b.z;
	float z = this->scalar * b.z + this->z * b.scalar + this->x * b.y - this->y * b.x;
	float scalar = this->scalar * b.scalar - this->x * b.x - this->y * b.y - this->z * b.z;

	this->x = x;
	this->y = y;
	this->z = z;
	this->scalar = scalar;

	return *this;*/

	__m128 x = _mm_xor_ps(_mm_set_ps(0.0f, 0.0f, -0.0f, 0.0f), b.quat);
	x = _mm_shuffle_ps(x, x, _MM_SHUFFLE(0, 1, 2, 3));
	x = _mm_mul_ps(this->quat, x);
	x = _mm_hadd_ps(x, x);
	x = _mm_hadd_ps(x, x);

	__m128 y = _mm_xor_ps(_mm_set_ps(0.0f, -0.0f, 0.0f, 0.0f), b.quat);
	y = _mm_shuffle_ps(y, y, _MM_SHUFFLE(1, 0, 3, 2));
	y = _mm_mul_ps(this->quat, y);
	y = _mm_hadd_ps(y, y);
	y = _mm_hadd_ps(y, y);

	__m128 z = _mm_xor_ps(_mm_set_ps(0.0f, 0.0f, 0.0f, -0.0f), b.quat);
	z = _mm_shuffle_ps(z, z, _MM_SHUFFLE(2, 3, 0, 1));
	z = _mm_mul_ps(this->quat, z);
	z = _mm_hadd_ps(z, z);
	z = _mm_hadd_ps(z, z);

	__m128 scalar = _mm_xor_ps(_mm_set_ps(0.0f, -0.0f, -0.0f, -0.0f), b.quat);
	scalar = _mm_mul_ps(this->quat, scalar);
	scalar = _mm_hadd_ps(scalar, scalar);
	scalar = _mm_hadd_ps(scalar, scalar);

	this->quat = _mm_set_ps(_mm_cvtss_f32(scalar), _mm_cvtss_f32(z), _mm_cvtss_f32(y), _mm_cvtss_f32(x));

	return *this;
}

Quaternion_Intrinsics& Quaternion_Intrinsics::operator*=(float k)
{
	this->quat = _mm_mul_ps(this->quat, _mm_set_ps1(k));

	return *this;
}

Quaternion_Intrinsics& Quaternion_Intrinsics::operator/=(float k)
{
	this->quat = _mm_div_ps(this->quat, _mm_set_ps1(k));

	return *this;
}

bool Quaternion_Intrinsics::operator==(const Quaternion_Intrinsics& b) const
{
	return CompareFloats(this->quatA[0], b.quatA[0]) && CompareFloats(this->quatA[1], b.quatA[1]) 
		&& CompareFloats(this->quatA[2], b.quatA[2]) && CompareFloats(this->quatA[3], b.quatA[3]);
}

bool Quaternion_Intrinsics::operator!=(const Quaternion_Intrinsics& b) const
{
	return !(*this == b);
}

Quaternion_Intrinsics Quaternion_Intrinsics::MakeIdentity()
{
	return Quaternion_Intrinsics(0.0f, 0.0f, 0.0f, 1.0f);
}

Quaternion_Intrinsics Quaternion_Intrinsics::MakeRotation(float angle, float x, float y, float z)
{
	float ang = (angle / 2.0f) * (PI / 180.0f);
	float cosAngle = cos(ang);
	float sinAngle = sin(ang);
	Vector3_Intrinsics axis = Normalize(Vector3_Intrinsics(x, y, z));

	__m128 t0 = _mm_set_ps(cosAngle, sinAngle, sinAngle, sinAngle);
	__m128 t1 = _mm_set_ps(1.0f, axis.GetZ(), axis.GetY(), axis.GetX());

	__m128 t2 = _mm_mul_ps(t0, t1);

	return Quaternion_Intrinsics(t2);
}

Quaternion_Intrinsics Quaternion_Intrinsics::MakeRotation(float angle, const Vector3_Intrinsics& axis)
{
	float ang = (angle / 2.0f) * (PI / 180.0f);
	float cosAngle = cos(ang);
	float sinAngle = sin(ang);
	Vector3_Intrinsics axisN = Normalize(Vector3_Intrinsics(axis.GetX(), axis.GetY(), axis.GetZ()));

	__m128 t0 = _mm_set_ps(cosAngle, sinAngle, sinAngle, sinAngle);
	__m128 t1 = _mm_set_ps(1.0f, axis.GetZ(), axis.GetY(), axis.GetX());

	__m128 t2 = _mm_mul_ps(t0, t1);

	return Quaternion_Intrinsics(t2);
}

Quaternion_Intrinsics operator+(const Quaternion_Intrinsics& a, const Quaternion_Intrinsics& b)
{
	return Quaternion_Intrinsics(_mm_add_ps(a.quat, b.quat));
}

Quaternion_Intrinsics operator-(const Quaternion_Intrinsics& a, const Quaternion_Intrinsics& b)
{
	return Quaternion_Intrinsics(_mm_sub_ps(a.quat, b.quat));
}

Quaternion_Intrinsics operator-(const Quaternion_Intrinsics& a)
{
	return Quaternion_Intrinsics(_mm_sub_ps(_mm_set_ps1(0.0f), a.quat));
}

Quaternion_Intrinsics operator*(const Quaternion_Intrinsics& a, const Quaternion_Intrinsics& b)
{
	__m128 x = _mm_xor_ps(_mm_set_ps(0.0f, 0.0f, -0.0f, 0.0f), b.quat);
	x = _mm_shuffle_ps(x, x, _MM_SHUFFLE(0, 1, 2, 3));
	x = _mm_mul_ps(a.quat, x);
	x = _mm_hadd_ps(x, x);
	x = _mm_hadd_ps(x, x);

	__m128 y = _mm_xor_ps(_mm_set_ps(0.0f, -0.0f, 0.0f, 0.0f), b.quat);
	y = _mm_shuffle_ps(y, y, _MM_SHUFFLE(1, 0, 3, 2));
	y = _mm_mul_ps(a.quat, y);
	y = _mm_hadd_ps(y, y);
	y = _mm_hadd_ps(y, y);

	__m128 z = _mm_xor_ps(_mm_set_ps(0.0f, 0.0f, 0.0f, -0.0f), b.quat);
	z = _mm_shuffle_ps(z, z, _MM_SHUFFLE(2, 3, 0, 1));
	z = _mm_mul_ps(a.quat, z);
	z = _mm_hadd_ps(z, z);
	z = _mm_hadd_ps(z, z);

	__m128 scalar = _mm_xor_ps(_mm_set_ps(0.0f, -0.0f, -0.0f, -0.0f), b.quat);
	scalar = _mm_mul_ps(a.quat, scalar);
	scalar = _mm_hadd_ps(scalar, scalar);
	scalar = _mm_hadd_ps(scalar, scalar);

	return Quaternion_Intrinsics(_mm_set_ps(_mm_cvtss_f32(scalar), _mm_cvtss_f32(z), _mm_cvtss_f32(y), _mm_cvtss_f32(x)));
}

Quaternion_Intrinsics operator*(const Quaternion_Intrinsics& a, float k)
{
	return Quaternion_Intrinsics(_mm_mul_ps(a.quat, _mm_set_ps1(k)));
}

Quaternion_Intrinsics operator*(float k, const Quaternion_Intrinsics& a)
{
	return Quaternion_Intrinsics(_mm_mul_ps(_mm_set_ps1(k), a.quat));
}

Quaternion_Intrinsics operator/(const Quaternion_Intrinsics& a, float k)
{
	return Quaternion_Intrinsics(_mm_div_ps(a.quat, _mm_set_ps1(k)));
}

float DotProduct(const Quaternion_Intrinsics& a, const Quaternion_Intrinsics& b)
{
	return _mm_cvtss_f32(_mm_dp_ps(a.quat, b.quat, 0xff));
}

float Length(const Quaternion_Intrinsics& a)
{
	return _mm_cvtss_f32(_mm_sqrt_ps(_mm_dp_ps(a.quat, a.quat, 0xff)));
}

Quaternion_Intrinsics Normalize(const Quaternion_Intrinsics& a)
{
	float mag = Length(a);
	assert(!CompareFloats(mag, 0.0f) && "Magnitude of the Quaternion_Intrinsics is 0.");

	return a / mag;
}

Quaternion_Intrinsics Conjugate(const Quaternion_Intrinsics& a)
{
	return Quaternion_Intrinsics(_mm_xor_ps(_mm_set_ps(0.0f, -0.0f, -0.0f, -0.0f), a.quat));
}

Quaternion_Intrinsics Inverse(const Quaternion_Intrinsics& a)
{
	return Conjugate(a) / Length(a);
}

bool IsIdentity(const Quaternion_Intrinsics& a)
{
	return CompareFloats(a.quatA[0], 0.0f) && CompareFloats(a.quatA[1], 0.0f) 
		&& CompareFloats(a.quatA[2], 0.0f) && CompareFloats(a.quatA[3], 1.0f);
}

Vector3_Intrinsics Rotate(const Quaternion_Intrinsics& q, const Vector3_Intrinsics& p)
{
	Quaternion_Intrinsics point(p.GetX(), p.GetY(), p.GetZ(), 0.0f);

	Quaternion_Intrinsics result = q * point * Conjugate(q);

	return Vector3_Intrinsics(result.quatA[0], result.quatA[1], result.quatA[2]);
}

Vector4_Intrinsics Rotate(const Quaternion_Intrinsics& q, const Vector4_Intrinsics& p)
{
	Quaternion_Intrinsics point(p.GetX(), p.GetY(), p.GetZ(), 0.0f);

	Quaternion_Intrinsics result = q * point * Conjugate(q);

	return Vector4_Intrinsics(result.quatA[0], result.quatA[1], result.quatA[2], p.GetW());
}

Matrix4x4_Intrinsics QuaternionToMatrix(const Quaternion_Intrinsics& q)
{
	__m128 one = _mm_set_ps1(1.0f);
	__m128 two = _mm_set_ps1(2.0f);

	//2scalar^2, 2z^2, 2y^2, 2x^2
	__m128 squared = _mm_mul_ps(two, _mm_mul_ps(q.quat, q.quat));

	//2xz, 2wy, 2xy, 2wz
	__m128 shuffled = _mm_shuffle_ps(q.quat, q.quat, _MM_SHUFFLE(2, 0, 3, 1));
	__m128 combined1 = _mm_mul_ps(two, _mm_mul_ps(q.quat, shuffled));
	combined1 = _mm_shuffle_ps(combined1, combined1, _MM_SHUFFLE(1, 2, 3, 0));

	//2wx, 2zy, 2xw, 2yz
	shuffled = _mm_shuffle_ps(q.quat, q.quat, _MM_SHUFFLE(0, 1, 2, 3));
	__m128 combined2 = _mm_mul_ps(two, _mm_mul_ps(q.quat, shuffled));
	combined2 = _mm_shuffle_ps(combined2, combined2, _MM_SHUFFLE(3, 2, 0, 1));

	//1 - 2y^2 - 2z^2
	__m128 temp = _mm_shuffle_ps(squared, squared, _MM_SHUFFLE(3, 0, 2, 1));
	temp = _mm_hadd_ps(temp, temp);
	float m00 = _mm_cvtss_f32(_mm_sub_ps(one, temp));

	//2xy + 2wz
	temp = _mm_hadd_ps(combined1, combined1);
	float m01 = _mm_cvtss_f32(temp);

	//2xz - 2wy
	temp = _mm_hsub_ps(combined1, combined1);
	temp = _mm_shuffle_ps(temp, temp, _MM_SHUFFLE(0, 1, 2, 3));
	float m02 = _mm_cvtss_f32(temp);

	float m03 = 0.0f;

	//2xy - 2wz
	temp = _mm_hsub_ps(combined1, combined1);
	float m10 = _mm_cvtss_f32(temp);

	//1 - 2x^2 - 2z^2
	temp = _mm_shuffle_ps(squared, squared, _MM_SHUFFLE(3, 1, 2, 0));
	temp = _mm_hadd_ps(temp, temp);
	float m11 = _mm_cvtss_f32(_mm_sub_ps(one, temp));

	//2yz + 2wx
	temp = _mm_hadd_ps(combined2, combined2);
	float m12 = _mm_cvtss_f32(temp);

	float m13 = 0.0f;

	//2xz + 2wy
	temp = _mm_hadd_ps(combined1, combined1);
	temp = _mm_shuffle_ps(temp, temp, _MM_SHUFFLE(0, 1, 2, 3));
	float m20 = _mm_cvtss_f32(temp);

	//2yz - 2wx
	temp = _mm_hsub_ps(combined2, combined2);
	float m21 = _mm_cvtss_f32(temp);

	//1 - 2x^2 - 2y^2
	temp = _mm_hadd_ps(squared, squared);
	float m22 = _mm_cvtss_f32(_mm_sub_ps(one, temp));

	float m23 = 0.0f;

	float m30 = 0.0f;
	float m31 = 0.0f;
	float m32 = 0.0f;
	float m33 = 1.0f;

	return Matrix4x4_Intrinsics(m00, m01, m02, m03,
								m10, m11, m12, m13,
								m20, m21, m22, m23,
								m30, m31, m32, m33);
}
