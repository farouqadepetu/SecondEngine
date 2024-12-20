#include "vectors_intrinsics.h"

//VECTOR2 IMPLEMENTAION
Vector2_Intrinsics::Vector2_Intrinsics()
{
	vec = _mm_set_ps1(0.0f);
}

Vector2_Intrinsics::Vector2_Intrinsics(float x, float y)
{
	vec = _mm_set_ps(0.0f, 0.0f, y, x);
}

Vector2_Intrinsics::Vector2_Intrinsics(__m128 vec)
{
	this->vec = vec;
}

float Vector2_Intrinsics::GetX() const
{
	return vecA[0];
}

float Vector2_Intrinsics::GetY() const
{
	return vecA[1];
}

void Vector2_Intrinsics::SetX(float x)
{
	vecA[0] = x;
}

void Vector2_Intrinsics::SetY(float y)
{
	vecA[1] = y;
}

void Vector2_Intrinsics::Set(float x, float y)
{
	vec = _mm_set_ps(0.0f, 0.0f, y, x);
}

Vector2_Intrinsics& Vector2_Intrinsics::operator+=(const Vector2_Intrinsics& b)
{
	this->vec = _mm_add_ps(this->vec, b.vec);

	return *this;
}

Vector2_Intrinsics& Vector2_Intrinsics::operator-=(const Vector2_Intrinsics& b)
{
	this->vec = _mm_sub_ps(this->vec, b.vec);

	return *this;
}

Vector2_Intrinsics& Vector2_Intrinsics::operator*=(const Vector2_Intrinsics& b)
{
	this->vec = _mm_mul_ps(this->vec, b.vec);

	return *this;
}

Vector2_Intrinsics& Vector2_Intrinsics::operator/=(float k)
{
	this->vec = _mm_div_ps(this->vec, _mm_set_ps1(k));

	return *this;
}

Vector2_Intrinsics operator+(const Vector2_Intrinsics& a, const Vector2_Intrinsics& b)
{
	return Vector2_Intrinsics(_mm_add_ps(a.vec, b.vec));
}

Vector2_Intrinsics operator-(const Vector2_Intrinsics& a, const Vector2_Intrinsics& b)
{
	return Vector2_Intrinsics(_mm_sub_ps(a.vec, b.vec));
}

Vector2_Intrinsics operator-(const Vector2_Intrinsics& a)
{
	return Vector2_Intrinsics(_mm_sub_ps(_mm_set_ps1(0.0f), a.vec));
}

Vector2_Intrinsics operator*(const Vector2_Intrinsics& a, const Vector2_Intrinsics& b)
{
	return Vector2_Intrinsics(_mm_mul_ps(a.vec, b.vec));
}

Vector2_Intrinsics operator*(const Vector2_Intrinsics& a, float k)
{
	return Vector2_Intrinsics(_mm_mul_ps(a.vec, _mm_set_ps1(k)));
}

Vector2_Intrinsics operator*(float k, const Vector2_Intrinsics& a)
{
	return Vector2_Intrinsics(_mm_mul_ps(_mm_set_ps1(k), a.vec));
}

Vector2_Intrinsics operator/(const Vector2_Intrinsics& a, float k)
{
	return Vector2_Intrinsics(_mm_div_ps(a.vec, _mm_set_ps1(k)));
}

float DotProduct(const Vector2_Intrinsics& a, const Vector2_Intrinsics& b)
{
	return _mm_cvtss_f32(_mm_dp_ps(a.vec, b.vec, 0xff));
}

float Length(const Vector2_Intrinsics& a)
{
	return _mm_cvtss_f32(_mm_sqrt_ps(_mm_dp_ps(a.vec, a.vec, 0xff)));
}

Vector2_Intrinsics Normalize(const Vector2_Intrinsics& a)
{
	float mag = 1.0f / Length(a);
	return Vector2_Intrinsics(_mm_mul_ps(a.vec, _mm_set_ps1(mag)));
}

float Distance(const Vector2_Intrinsics& a, const Vector2_Intrinsics& b)
{
	return Length(b - a);
}

//VECTOR3 IMPLEMENTAION
Vector3_Intrinsics::Vector3_Intrinsics()
{
	vec = _mm_set_ps1(0.0f);
}

Vector3_Intrinsics::Vector3_Intrinsics(float x, float y, float z)
{
	vec = _mm_set_ps(0.0f, z, y, x);
}

Vector3_Intrinsics::Vector3_Intrinsics(__m128 vec)
{
	this->vec = vec;
}

float Vector3_Intrinsics::GetX() const
{
	return vecA[0];
}

float Vector3_Intrinsics::GetY() const
{
	return vecA[1];
}

float Vector3_Intrinsics::GetZ() const
{
	return vecA[2];
}

void Vector3_Intrinsics::SetX(float x)
{
	vecA[0] = x;
}

void Vector3_Intrinsics::SetY(float y)
{
	vecA[1] = y;
}

void Vector3_Intrinsics::SetZ(float z)
{
	vecA[2] = z;
}

void Vector3_Intrinsics::Set(float x, float y, float z)
{
	vec = _mm_set_ps(0.0f, z, y, x);
}

Vector3_Intrinsics& Vector3_Intrinsics::operator+=(const Vector3_Intrinsics& b)
{
	this->vec = _mm_add_ps(this->vec, b.vec);

	return *this;
}

Vector3_Intrinsics& Vector3_Intrinsics::operator-=(const Vector3_Intrinsics& b)
{
	this->vec = _mm_sub_ps(this->vec, b.vec);

	return *this;
}

Vector3_Intrinsics& Vector3_Intrinsics::operator*=(const Vector3_Intrinsics& b)
{
	this->vec = _mm_mul_ps(this->vec, b.vec);

	return *this;
}

Vector3_Intrinsics& Vector3_Intrinsics::operator/=(float k)
{
	this->vec = _mm_div_ps(this->vec, _mm_set_ps1(k));

	return *this;
}

Vector3_Intrinsics operator+(const Vector3_Intrinsics& a, const Vector3_Intrinsics& b)
{
	return Vector3_Intrinsics(_mm_add_ps(a.vec, b.vec));
}

Vector3_Intrinsics operator-(const Vector3_Intrinsics& a, const Vector3_Intrinsics& b)
{
	return Vector3_Intrinsics(_mm_sub_ps(a.vec, b.vec));
}

Vector3_Intrinsics operator-(const Vector3_Intrinsics& a)
{
	return Vector3_Intrinsics(_mm_sub_ps(_mm_set_ps1(0.0f), a.vec));
}

Vector3_Intrinsics operator*(const Vector3_Intrinsics& a, const Vector3_Intrinsics& b)
{
	return Vector3_Intrinsics(_mm_mul_ps(a.vec, b.vec));
}

Vector3_Intrinsics operator*(const Vector3_Intrinsics& a, float k)
{
	return Vector3_Intrinsics(_mm_mul_ps(a.vec, _mm_set_ps1(k)));
}

Vector3_Intrinsics operator*(float k, const Vector3_Intrinsics& a)
{
	return Vector3_Intrinsics(_mm_mul_ps(_mm_set_ps1(k), a.vec));
}

Vector3_Intrinsics operator/(const Vector3_Intrinsics& a, float k)
{
	return Vector3_Intrinsics(_mm_div_ps(a.vec, _mm_set_ps1(k)));
}

float DotProduct(const Vector3_Intrinsics& a, const Vector3_Intrinsics& b)
{
	return _mm_cvtss_f32(_mm_dp_ps(a.vec, b.vec, 0xff));
}

float Length(const Vector3_Intrinsics& a)
{
	return _mm_cvtss_f32(_mm_sqrt_ps(_mm_dp_ps(a.vec, a.vec, 0xff)));
}

Vector3_Intrinsics Normalize(const Vector3_Intrinsics& a)
{
	float mag = 1.0f / Length(a);
	return Vector3_Intrinsics(_mm_mul_ps(a.vec, _mm_set_ps1(mag)));
}

float Distance(const Vector3_Intrinsics& a, const Vector3_Intrinsics& b)
{
	return Length(b - a);
}

Vector3_Intrinsics CrossProduct(const Vector3_Intrinsics& a, const Vector3_Intrinsics& b)
{
	return Vector3_Intrinsics(
		_mm_sub_ps(
		_mm_mul_ps(_mm_shuffle_ps(a.vec, a.vec, _MM_SHUFFLE(3, 0, 2, 1)), _mm_shuffle_ps(b.vec, b.vec, _MM_SHUFFLE(3, 1, 0, 2))),
		_mm_mul_ps(_mm_shuffle_ps(a.vec, a.vec, _MM_SHUFFLE(3, 1, 0, 2)), _mm_shuffle_ps(b.vec, b.vec, _MM_SHUFFLE(3, 0, 2, 1)))
		)
	);
}

//VECTOR4 IMPLEMENTATION
Vector4_Intrinsics::Vector4_Intrinsics()
{
	vec = _mm_set_ps1(0.0f);
}

Vector4_Intrinsics::Vector4_Intrinsics(float x, float y, float z, float w)
{
	vec = _mm_set_ps(w, z, y, x);
}

Vector4_Intrinsics::Vector4_Intrinsics(__m128 vec)
{
	this->vec = vec;
}

float Vector4_Intrinsics::GetX() const
{
	return vecA[0];
}

float Vector4_Intrinsics::GetY() const
{
	return vecA[1];
}

float Vector4_Intrinsics::GetZ() const
{
	return vecA[2];
}

float Vector4_Intrinsics::GetW() const
{
	return vecA[3];
}

void Vector4_Intrinsics::SetX(float x)
{
	vecA[0] = x;
}

void Vector4_Intrinsics::SetY(float y)
{
	vecA[1] = y;
}

void Vector4_Intrinsics::SetZ(float z)
{
	vecA[2] = z;
}

void Vector4_Intrinsics::SetW(float w)
{
	vecA[3] = w;
}

void Vector4_Intrinsics::Set(float x, float y, float z, float w)
{
	vec = _mm_set_ps(w, z, y, x);
}

Vector4_Intrinsics& Vector4_Intrinsics::operator+=(const Vector4_Intrinsics& b)
{
	this->vec = _mm_add_ps(this->vec, b.vec);

	return *this;
}

Vector4_Intrinsics& Vector4_Intrinsics::operator-=(const Vector4_Intrinsics& b)
{
	this->vec = _mm_sub_ps(this->vec, b.vec);

	return *this;
}

Vector4_Intrinsics& Vector4_Intrinsics::operator*=(const Vector4_Intrinsics& b)
{
	this->vec = _mm_mul_ps(this->vec, b.vec);

	return *this;
}

Vector4_Intrinsics& Vector4_Intrinsics::operator/=(float k)
{
	this->vec = _mm_div_ps(this->vec, _mm_set_ps1(k));

	return *this;
}

Vector4_Intrinsics operator+(const Vector4_Intrinsics& a, const Vector4_Intrinsics& b)
{
	return Vector4_Intrinsics(_mm_add_ps(a.vec, b.vec));
}

Vector4_Intrinsics operator-(const Vector4_Intrinsics& a, const Vector4_Intrinsics& b)
{
	return Vector4_Intrinsics(_mm_sub_ps(a.vec, b.vec));
}

Vector4_Intrinsics operator-(const Vector4_Intrinsics& a)
{
	return Vector4_Intrinsics(_mm_xor_ps(a.vec, _mm_set_ps1(-0.0f)));
}

Vector4_Intrinsics operator*(const Vector4_Intrinsics& a, const Vector4_Intrinsics& b)
{
	return Vector4_Intrinsics(_mm_mul_ps(a.vec, b.vec));
}

Vector4_Intrinsics operator*(const Vector4_Intrinsics& a, float k)
{
	return Vector4_Intrinsics(_mm_mul_ps(a.vec, _mm_set_ps1(k)));
}

Vector4_Intrinsics operator*(float k, const Vector4_Intrinsics& a)
{
	return Vector4_Intrinsics(_mm_mul_ps(_mm_set_ps1(k), a.vec));
}

Vector4_Intrinsics operator/(const Vector4_Intrinsics& a, float k)
{
	return Vector4_Intrinsics(_mm_div_ps(a.vec, _mm_set_ps1(k)));
}

float DotProduct(const Vector4_Intrinsics& a, const Vector4_Intrinsics& b)
{
	return _mm_cvtss_f32(_mm_dp_ps(a.vec, b.vec, 0xff));
}

float Length(const Vector4_Intrinsics& a)
{
	return _mm_cvtss_f32(_mm_sqrt_ps(_mm_dp_ps(a.vec, a.vec, 0xff)));
}

Vector4_Intrinsics Normalize(const Vector4_Intrinsics& a)
{
	float mag = 1.0f / Length(a);
	return Vector4_Intrinsics(_mm_mul_ps(a.vec, _mm_set_ps1(mag)));
}

float Distance(const Vector4_Intrinsics& a, const Vector4_Intrinsics& b)
{
	return Length(b - a);
}
