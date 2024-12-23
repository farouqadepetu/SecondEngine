#include <cassert>
#include <cmath>
#include "vectors.h"
#include "../Math/math_utility.h"

//VECTOR2 IMPLEMENTATION
Vector2::Vector2() : x(0.0f), y(0.0f)
{}

Vector2::Vector2(float x, float y) : x(x), y(y)
{}

float Vector2::GetX() const
{
	return x;
}

float Vector2::GetY() const
{
	return y;
}

void Vector2::SetX(float x)
{
	this->x = x;
}

void Vector2::SetY(float y)
{
	this->y = y;
}

void Vector2::Set(float x, float y)
{
	this->x = x;
	this->y = y;
}

Vector2& Vector2::operator+=(const Vector2& b)
{
	this->x += b.x;
	this->y += b.y;

	return *this;
}

Vector2& Vector2::operator-=(const Vector2& b)
{
	this->x -= b.x;
	this->y -= b.y;

	return *this;
}

Vector2& Vector2::operator*=(const Vector2& b)
{
	this->x *= b.x;
	this->y *= b.y;

	return *this;
}

Vector2& Vector2::operator*=(float k)
{
	this->x *= k;
	this->y *= k;

	return *this;
}

Vector2& Vector2::operator/=(float k)
{
	this->x /= k;
	this->y /= k;

	return *this;
}

bool Vector2::operator==(const Vector2& vec) const
{
	return CompareFloats(this->x, vec.x) && CompareFloats(this->y, vec.y);
}

bool Vector2::operator!=(const Vector2& vec) const
{
	return !(*this == vec);
}

Vector2 operator+(const Vector2& a, const Vector2& b)
{
	return Vector2(a.x + b.x, a.y + b.y);
}

Vector2 operator-(const Vector2& a, const Vector2& b)
{
	return Vector2(a.x - b.x, a.y - b.y);
}

Vector2 operator-(const Vector2& a)
{
	return Vector2(-a.x, -a.y);
}

Vector2 operator*(const Vector2& a, const Vector2& b)
{
	return Vector2(a.x * b.x, a.y * b.y);
}

Vector2 operator*(const Vector2& a, float k)
{
	return Vector2(a.x * k, a.y * k);
}

Vector2 operator*(float k, const Vector2& a)
{
	return Vector2(k * a.x, a.y * k);
}

Vector2 operator/(const Vector2& a, float k)
{
	return Vector2(a.x / k, a.y / k);
}

float DotProduct(const Vector2& a, const Vector2& b)
{
	return a.x * b.x + a.y * b.y;
}

float Length(const Vector2& a)
{
	return sqrt(a.x * a.x + a.y * a.y);
}

Vector2 Normalize(const Vector2& a)
{
	float mag = 1.0f / Length(a);
	assert(!CompareFloats(mag, 0.0f) && "Magnitude of the quaternion is 0.");
	return Vector2(a.x * mag, a.y * mag);
}

float Distance(const Vector2& a, const Vector2& b)
{
	return Length(b - a);
}


//VECTOR3 IMPLEMENTATION
Vector3::Vector3() : x(0.0f), y(0.0f), z(0.0f)
{}

Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z)
{}

float Vector3::GetX() const
{
	return x;
}

float Vector3::GetY() const
{
	return y;
}

float Vector3::GetZ() const
{
	return z;
}

void Vector3::SetX(float x)
{
	this->x = x;
}

void Vector3::SetY(float y)
{
	this->y = y;
}

void Vector3::SetZ(float z)
{
	this->z = z;
}
void Vector3::Set(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector3& Vector3::operator+=(const Vector3& b)
{
	this->x += b.x;
	this->y += b.y;
	this->z += b.z;

	return *this;
}

Vector3& Vector3::operator-=(const Vector3& b)
{
	this->x -= b.x;
	this->y -= b.y;
	this->z -= b.z;

	return *this;
}

Vector3& Vector3::operator*=(const Vector3& b)
{
	this->x *= b.x;
	this->y *= b.y;
	this->z *= b.z;

	return *this;
}

Vector3& Vector3::operator*=(float k)
{
	this->x *= k;
	this->y *= k;
	this->z *= k;

	return *this;
}

Vector3& Vector3::operator/=(float k)
{
	this->x /= k;
	this->y /= k;
	this->z /= k;

	return *this;
}

bool Vector3::operator==(const Vector3& vec) const
{
	return CompareFloats(this->x, vec.x) && CompareFloats(this->y, vec.y) && CompareFloats(this->z, vec.z);
}

bool Vector3::operator!=(const Vector3& vec) const
{
	return !(*this == vec);
}

Vector3 operator+(const Vector3& a, const Vector3& b)
{
	return Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vector3 operator-(const Vector3& a, const Vector3& b)
{
	return Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vector3 operator-(const Vector3& a)
{
	return Vector3(-a.x, -a.y, -a.z);
}

Vector3 operator*(const Vector3& a, const Vector3& b)
{
	return Vector3(a.x * b.x, a.y * b.y, a.z * b.z);
}

Vector3 operator*(const Vector3& a, float k)
{
	return Vector3(a.x * k, a.y * k, a.z * k);
}

Vector3 operator*(float k, const Vector3& a)
{
	return Vector3(k * a.x, a.y * k, a.z * k);
}

Vector3 operator/(const Vector3& a, float k)
{
	return Vector3(a.x / k, a.y / k, a.z / k);
}

float DotProduct(const Vector3& a, const Vector3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

float Length(const Vector3& a)
{
	return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

Vector3 Normalize(const Vector3& a)
{
	float mag = 1.0f / Length(a);
	assert(!CompareFloats(mag, 0.0f) && "Magnitude of the quaternion is 0.");
	return Vector3(a.x * mag, a.y * mag, a.z * mag);
}

float Distance(const Vector3& a, const Vector3& b)
{
	return Length(b - a);
}

Vector3 CrossProduct(const Vector3& a, const Vector3& b)
{
	return Vector3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

//VECTOR4 IMPLEMENTATION
Vector4::Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f)
{}

Vector4::Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w)
{}

float Vector4::GetX() const
{
	return x;
}

float Vector4::GetY() const
{
	return y;
}

float Vector4::GetZ() const
{
	return z;
}

float Vector4::GetW() const
{
	return w;
}

void Vector4::SetX(float x)
{
	this->x = x;
}

void Vector4::SetY(float y)
{
	this->y = y;
}

void Vector4::SetZ(float z)
{
	this->z = z;
}

void Vector4::SetW(float w)
{
	this->w = w;
}

void Vector4::Set(float x, float y, float z, float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

Vector4& Vector4::operator+=(const Vector4& b)
{
	this->x += b.x;
	this->y += b.y;
	this->z += b.z;
	this->w += b.w;

	return *this;
}

Vector4& Vector4::operator-=(const Vector4& b)
{
	this->x -= b.x;
	this->y -= b.y;
	this->z -= b.z;
	this->w -= b.w;

	return *this;
}

Vector4& Vector4::operator*=(const Vector4& b)
{
	this->x *= b.x;
	this->y *= b.y;
	this->z *= b.z;
	this->w *= b.w;

	return *this;
}

Vector4& Vector4::operator*=(float k)
{
	this->x *= k;
	this->y *= k;
	this->z *= k;
	this->w *= k;

	return *this;
}

Vector4& Vector4::operator/=(float k)
{
	this->x /= k;
	this->y /= k;
	this->z /= k;
	this->w /= k;

	return *this;
}

bool Vector4::operator==(const Vector4& vec) const
{
	return CompareFloats(this->x, vec.x) && CompareFloats(this->y, vec.y) && CompareFloats(this->z, vec.z) && CompareFloats(this->w, vec.w);
}

bool Vector4::operator!=(const Vector4& vec) const
{
	return !(*this == vec);
}

Vector4 operator+(const Vector4& a, const Vector4& b)
{
	return Vector4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

Vector4 operator-(const Vector4& a, const Vector4& b)
{
	return Vector4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

Vector4 operator-(const Vector4& a)
{
	return Vector4(-a.x, -a.y, -a.z, -a.w);
}

Vector4 operator*(const Vector4& a, const Vector4& b)
{
	return Vector4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

Vector4 operator*(const Vector4& a, float k)
{
	return Vector4(a.x * k, a.y * k, a.z * k, a.w * k);
}

Vector4 operator*(float k, const Vector4& a)
{
	return Vector4(k * a.x, a.y * k, a.z * k, a.w * k);
}

Vector4 operator/(const Vector4& a, float k)
{
	return Vector4(a.x / k, a.y / k, a.z / k, a.w / k);
}

float DotProduct(const Vector4& a, const Vector4& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

float Length(const Vector4& a)
{
	return sqrt(a.x * a.x + a.y * a.y + a.z * a.z + a.w * a.w);
}

Vector4 Normalize(const Vector4& a)
{
	float mag =  1.0f / Length(a);
	assert(!CompareFloats(mag, 0.0f) && "Magnitude of the quaternion is 0.");
	return Vector4(a.x * mag, a.y * mag, a.z * mag, a.w * mag);
}

float Distance(const Vector4& a, const Vector4& b)
{
	return Length(b - a);
}
