#pragma once

#include <cassert>
#include <cmath>
#include <cfloat>
#include <cstdint>

#include "SEMath_Utility.h"

//VECTOR2
//------------------------------------------------------------------------------------------------------------
class alignas(16) Vector2
{
public:
	Vector2();
	Vector2(float x, float y);

	float GetX() const;
	float GetY() const;

	void SetX(float x);
	void SetY(float y);
	void Set(float x, float y);

	//Returns the result of this + b.
	Vector2& operator+=(const Vector2& b);

	//Returns the result of this - b.
	Vector2& operator-=(const Vector2& b);

	//Returns the result of this * b (component multiplication).
	Vector2& operator*=(const Vector2& b);

	//Returns the result of this * k.
	Vector2& operator*=(float k);

	//Returns the result of this / k.
	Vector2& operator/=(float k);

	//Returns true if this equals to vec.
	bool operator==(const Vector2& vec) const;

	//Returns true if this does not equal to vec.
	bool operator!=(const Vector2& vec) const;

	//Returns the result of a + b.
	friend Vector2 operator+(const Vector2& a, const Vector2& b);

	//Returns the result of a - b.
	friend Vector2 operator-(const Vector2& a, const Vector2& b);

	//Returns the result of the negation of a;
	friend Vector2 operator-(const Vector2& a);

	//Returns the result of the component multiplication between a and b.
	friend Vector2 operator*(const Vector2& a, const Vector2& b);

	//Returns the result of a * k.
	friend Vector2 operator*(const Vector2& a, float k);

	//Returns the result of k * a.
	friend Vector2 operator*(float k, const Vector2& a);

	//Returns the result of a / k.
	friend Vector2 operator/(const Vector2& a, float k);

	//Returns the result of a dot b.
	friend float DotProduct(const Vector2& a, const Vector2& b);

	//Returns the magntidue of a.
	friend float Length(const Vector2& a);

	//Returns a unit vector by normalizing a.
	friend Vector2 Normalize(const Vector2& a);

	//Returns the distance between a and b.
	friend float Distance(const Vector2& a, const Vector2& b);

private:
	float x;
	float y;
};

//VECTOR2 IMPLEMENTATION
inline Vector2::Vector2() : x(0.0f), y(0.0f)
{}

inline Vector2::Vector2(float x, float y) : x(x), y(y)
{}

inline float Vector2::GetX() const
{
	return x;
}

inline float Vector2::GetY() const
{
	return y;
}

inline void Vector2::SetX(float x)
{
	this->x = x;
}

inline void Vector2::SetY(float y)
{
	this->y = y;
}

inline void Vector2::Set(float x, float y)
{
	this->x = x;
	this->y = y;
}

inline Vector2& Vector2::operator+=(const Vector2& b)
{
	this->x += b.x;
	this->y += b.y;

	return *this;
}

inline Vector2& Vector2::operator-=(const Vector2& b)
{
	this->x -= b.x;
	this->y -= b.y;

	return *this;
}

inline Vector2& Vector2::operator*=(const Vector2& b)
{
	this->x *= b.x;
	this->y *= b.y;

	return *this;
}

inline Vector2& Vector2::operator*=(float k)
{
	this->x *= k;
	this->y *= k;

	return *this;
}

inline Vector2& Vector2::operator/=(float k)
{
	this->x /= k;
	this->y /= k;

	return *this;
}

inline bool Vector2::operator==(const Vector2& vec) const
{
	return CompareFloats(this->x, vec.x) && CompareFloats(this->y, vec.y);
}

inline bool Vector2::operator!=(const Vector2& vec) const
{
	return !(*this == vec);
}

inline Vector2 operator+(const Vector2& a, const Vector2& b)
{
	return Vector2(a.x + b.x, a.y + b.y);
}

inline Vector2 operator-(const Vector2& a, const Vector2& b)
{
	return Vector2(a.x - b.x, a.y - b.y);
}

inline Vector2 operator-(const Vector2& a)
{
	return Vector2(-a.x, -a.y);
}

inline Vector2 operator*(const Vector2& a, const Vector2& b)
{
	return Vector2(a.x * b.x, a.y * b.y);
}

inline Vector2 operator*(const Vector2& a, float k)
{
	return Vector2(a.x * k, a.y * k);
}

inline Vector2 operator*(float k, const Vector2& a)
{
	return Vector2(k * a.x, a.y * k);
}

inline Vector2 operator/(const Vector2& a, float k)
{
	return Vector2(a.x / k, a.y / k);
}

inline float DotProduct(const Vector2& a, const Vector2& b)
{
	return a.x * b.x + a.y * b.y;
}

inline float Length(const Vector2& a)
{
	return sqrt(a.x * a.x + a.y * a.y);
}

inline Vector2 Normalize(const Vector2& a)
{
	float mag = 1.0f / Length(a);
	assert(!CompareFloats(mag, 0.0f) && "Magnitude of the vector is 0.");
	return Vector2(a.x * mag, a.y * mag);
}

inline float Distance(const Vector2& a, const Vector2& b)
{
	return Length(b - a);
}
//------------------------------------------------------------------------------------------------------------

//VECTOR3
//------------------------------------------------------------------------------------------------------------
class alignas(16) Vector3
{
public:
	Vector3();
	Vector3(float x, float y, float z);

	float GetX() const;
	float GetY() const;
	float GetZ() const;

	void SetX(float x);
	void SetY(float y);
	void SetZ(float z);
	void Set(float x, float y, float z);

	//Returns the result of this + b.
	Vector3& operator+=(const Vector3& b);

	//Returns the result of this - b.
	Vector3& operator-=(const Vector3& b);

	//Returns the result of this * b (component multiplication).
	Vector3& operator*=(const Vector3& b);

	//Returns the result of this * k.
	Vector3& operator*=(float k);

	//Returns the result of this / k.
	Vector3& operator/=(float k);

	//Returns true if this equals to vec.
	bool operator==(const Vector3& vec) const;

	//Returns true if this does not equal to vec.
	bool operator!=(const Vector3& vec) const;

	//Returns the result of a + b.
	friend Vector3 operator+(const Vector3& a, const Vector3& b);

	//Returns the result of a - b.
	friend Vector3 operator-(const Vector3& a, const Vector3& b);

	//Returns the result of the negation of a;
	friend Vector3 operator-(const Vector3& a);

	//Returns the result of the component multiplication between a and b.
	friend Vector3 operator*(const Vector3& a, const Vector3& b);

	//Returns the result of a * k.
	friend Vector3 operator*(const Vector3& a, float k);

	//Returns the result of k * a.
	friend Vector3 operator*(float k, const Vector3& a);

	//Returns the result of a / k.
	friend Vector3 operator/(const Vector3& a, float k);

	//Returns the result of a dot b.
	friend float DotProduct(const Vector3& a, const Vector3& b);

	//Returns the magntidue of a.
	friend float Length(const Vector3& a);

	//Returns a unit vector by normalizing a.
	friend Vector3 Normalize(const Vector3& a);

	//Returns the distance between a and b.
	friend float Distance(const Vector3& a, const Vector3& b);

	//Returns the cross product between a and b.
	friend Vector3 CrossProduct(const Vector3& a, const Vector3& b);

private:
	float x;
	float y;
	float z;
};

inline Vector3::Vector3() : x(0.0f), y(0.0f), z(0.0f)
{}

inline Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z)
{}

inline float Vector3::GetX() const
{
	return x;
}

inline float Vector3::GetY() const
{
	return y;
}

inline float Vector3::GetZ() const
{
	return z;
}

inline void Vector3::SetX(float x)
{
	this->x = x;
}

inline void Vector3::SetY(float y)
{
	this->y = y;
}

inline void Vector3::SetZ(float z)
{
	this->z = z;
}
inline void Vector3::Set(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

inline Vector3& Vector3::operator+=(const Vector3& b)
{
	this->x += b.x;
	this->y += b.y;
	this->z += b.z;

	return *this;
}

inline Vector3& Vector3::operator-=(const Vector3& b)
{
	this->x -= b.x;
	this->y -= b.y;
	this->z -= b.z;

	return *this;
}

inline Vector3& Vector3::operator*=(const Vector3& b)
{
	this->x *= b.x;
	this->y *= b.y;
	this->z *= b.z;

	return *this;
}

inline Vector3& Vector3::operator*=(float k)
{
	this->x *= k;
	this->y *= k;
	this->z *= k;

	return *this;
}

inline Vector3& Vector3::operator/=(float k)
{
	this->x /= k;
	this->y /= k;
	this->z /= k;

	return *this;
}

inline bool Vector3::operator==(const Vector3& vec) const
{
	return CompareFloats(this->x, vec.x) && CompareFloats(this->y, vec.y) && CompareFloats(this->z, vec.z);
}

inline bool Vector3::operator!=(const Vector3& vec) const
{
	return !(*this == vec);
}

inline Vector3 operator+(const Vector3& a, const Vector3& b)
{
	return Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
}

inline Vector3 operator-(const Vector3& a, const Vector3& b)
{
	return Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
}

inline Vector3 operator-(const Vector3& a)
{
	return Vector3(-a.x, -a.y, -a.z);
}

inline Vector3 operator*(const Vector3& a, const Vector3& b)
{
	return Vector3(a.x * b.x, a.y * b.y, a.z * b.z);
}

inline Vector3 operator*(const Vector3& a, float k)
{
	return Vector3(a.x * k, a.y * k, a.z * k);
}

inline Vector3 operator*(float k, const Vector3& a)
{
	return Vector3(k * a.x, a.y * k, a.z * k);
}

inline Vector3 operator/(const Vector3& a, float k)
{
	return Vector3(a.x / k, a.y / k, a.z / k);
}

inline float DotProduct(const Vector3& a, const Vector3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline float Length(const Vector3& a)
{
	return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

inline Vector3 Normalize(const Vector3& a)
{
	float mag = 1.0f / Length(a);
	assert(!CompareFloats(mag, 0.0f) && "Magnitude of the vector is 0.");
	return Vector3(a.x * mag, a.y * mag, a.z * mag);
}

inline float Distance(const Vector3& a, const Vector3& b)
{
	return Length(b - a);
}

inline Vector3 CrossProduct(const Vector3& a, const Vector3& b)
{
	return Vector3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}
//------------------------------------------------------------------------------------------------------------

//VECTOR4
//------------------------------------------------------------------------------------------------------------

class Vector4
{
public:
	Vector4();
	Vector4(float x, float y, float z, float w);

	float GetX() const;
	float GetY() const;
	float GetZ() const;
	float GetW() const;

	void SetX(float x);
	void SetY(float y);
	void SetZ(float z);
	void SetW(float w);
	void Set(float x, float y, float z, float w);

	//Returns the result of this + b.
	Vector4& operator+=(const Vector4& b);

	//Returns the result of this - b.
	Vector4& operator-=(const Vector4& b);

	//Returns the result of this * b (component multiplication).
	Vector4& operator*=(const Vector4& b);

	//Returns the result of this * k.
	Vector4& operator*=(float k);

	//Returns the result of this / k.
	Vector4& operator/=(float k);

	//Returns true if this equals to vec.
	bool operator==(const Vector4& vec) const;

	//Returns true if this does not equal to vec.
	bool operator!=(const Vector4& vec) const;

	//Returns the result of a + b.
	friend Vector4 operator+(const Vector4& a, const Vector4& b);

	//Returns the result of a - b.
	friend Vector4 operator-(const Vector4& a, const Vector4& b);

	//Returns the result of the negation of a;
	friend Vector4 operator-(const Vector4& a);

	//Returns the result of the component multiplication between a and b.
	friend Vector4 operator*(const Vector4& a, const Vector4& b);

	//Returns the result of a * k.
	friend Vector4 operator*(const Vector4& a, float k);

	//Returns the result of k * a.
	friend Vector4 operator*(float k, const Vector4& a);

	//Returns the result of a / k.
	friend Vector4 operator/(const Vector4& a, float k);

	//Returns the result of a dot b.
	friend float DotProduct(const Vector4& a, const Vector4& b);

	//Returns the magntidue of a.
	friend float Length(const Vector4& a);

	//Returns a unit vector by normalizing a.
	friend Vector4 Normalize(const Vector4& a);

	//Returns the distance between a and b.
	friend float Distance(const Vector4& a, const Vector4& b);

private:
	float x;
	float y;
	float z;
	float w;
};

inline Vector4::Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f)
{}

inline Vector4::Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w)
{}

inline float Vector4::GetX() const
{
	return x;
}

inline float Vector4::GetY() const
{
	return y;
}

inline float Vector4::GetZ() const
{
	return z;
}

inline float Vector4::GetW() const
{
	return w;
}

inline void Vector4::SetX(float x)
{
	this->x = x;
}

inline void Vector4::SetY(float y)
{
	this->y = y;
}

inline void Vector4::SetZ(float z)
{
	this->z = z;
}

inline void Vector4::SetW(float w)
{
	this->w = w;
}

inline void Vector4::Set(float x, float y, float z, float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

inline Vector4& Vector4::operator+=(const Vector4& b)
{
	this->x += b.x;
	this->y += b.y;
	this->z += b.z;
	this->w += b.w;

	return *this;
}

inline Vector4& Vector4::operator-=(const Vector4& b)
{
	this->x -= b.x;
	this->y -= b.y;
	this->z -= b.z;
	this->w -= b.w;

	return *this;
}

inline Vector4& Vector4::operator*=(const Vector4& b)
{
	this->x *= b.x;
	this->y *= b.y;
	this->z *= b.z;
	this->w *= b.w;

	return *this;
}

inline Vector4& Vector4::operator*=(float k)
{
	this->x *= k;
	this->y *= k;
	this->z *= k;
	this->w *= k;

	return *this;
}

inline Vector4& Vector4::operator/=(float k)
{
	this->x /= k;
	this->y /= k;
	this->z /= k;
	this->w /= k;

	return *this;
}

inline bool Vector4::operator==(const Vector4& vec) const
{
	return CompareFloats(this->x, vec.x) && CompareFloats(this->y, vec.y) && CompareFloats(this->z, vec.z) && CompareFloats(this->w, vec.w);
}

inline bool Vector4::operator!=(const Vector4& vec) const
{
	return !(*this == vec);
}

inline Vector4 operator+(const Vector4& a, const Vector4& b)
{
	return Vector4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

inline Vector4 operator-(const Vector4& a, const Vector4& b)
{
	return Vector4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

inline Vector4 operator-(const Vector4& a)
{
	return Vector4(-a.x, -a.y, -a.z, -a.w);
}

inline Vector4 operator*(const Vector4& a, const Vector4& b)
{
	return Vector4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

inline Vector4 operator*(const Vector4& a, float k)
{
	return Vector4(a.x * k, a.y * k, a.z * k, a.w * k);
}

inline Vector4 operator*(float k, const Vector4& a)
{
	return Vector4(k * a.x, a.y * k, a.z * k, a.w * k);
}

inline Vector4 operator/(const Vector4& a, float k)
{
	return Vector4(a.x / k, a.y / k, a.z / k, a.w / k);
}

inline float DotProduct(const Vector4& a, const Vector4& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

inline float Length(const Vector4& a)
{
	return sqrt(a.x * a.x + a.y * a.y + a.z * a.z + a.w * a.w);
}

inline Vector4 Normalize(const Vector4& a)
{
	float mag = 1.0f / Length(a);
	assert(!CompareFloats(mag, 0.0f) && "Magnitude of the vector is 0.");
	return Vector4(a.x * mag, a.y * mag, a.z * mag, a.w * mag);
}

inline float Distance(const Vector4& a, const Vector4& b)
{
	return Length(b - a);
}
//------------------------------------------------------------------------------------------------------------


typedef Vector4 vec4;
typedef Vector3 vec3;
typedef Vector2 vec2;


//MATRIX2X2
//------------------------------------------------------------------------------------------------------------
class Matrix2x2
{
public:

	//Creates an identity matrix
	Matrix2x2();

	//Creates a matrix with the specified rows.
	Matrix2x2(vec2 row0, vec2 row1);

	//Creates a matrix with the specified values.
	Matrix2x2(float m00, float m01,
		float m10, float m11);

	//Returns the element at the specified [row][col]
	float GetElement(uint32_t row, uint32_t col) const;

	//Returns the specified row.
	vec2 GetRow(uint32_t row) const;

	//Returns the specified col.
	vec2 GetCol(uint32_t col) const;

	//Sets the specified [row][col] to the specified value.
	void SetElement(uint32_t row, uint32_t col, float value);

	//Sets the specified row to the specified values.
	void SetRow(uint32_t row, vec2 values);

	//Sets the specified row to the specified values.
	void SetRow(uint32_t row, float val0, float val1);

	//Sets the specified row to the specified values.
	void SetCol(uint32_t col, float val0, float val1);

	//Sets the specified row to the specified values.
	void SetCol(uint32_t col, vec2 values);

	//Returns the identity matrix
	static Matrix2x2 MakeIdentity();

	//Returns a scale matrix with the specified values.
	static Matrix2x2 Scale(float x, float y);

	//Returns a rotation matrix about the z-axis with the specified angle.
	//Angle should be given in degrees.
	static Matrix2x2 RotZ(float angle);

	//Returns the result of this + matB.
	Matrix2x2& operator+=(const Matrix2x2& matB);

	//Returns the result of this - matB.
	Matrix2x2& operator-=(const Matrix2x2& matB);

	//Returns the result of this * matB (matrix-matrix multiplication).
	Matrix2x2& operator*=(const Matrix2x2& matB);

	//Returns the result of this * k.
	Matrix2x2& operator*=(float k);

	//Returns the result of this / k.
	Matrix2x2& operator/=(float k);

	//Returns true if the matrix is an identity matrix, false otherwise.
	friend bool IsIdentity(const Matrix2x2& mat);

	//Returns the transposition of the matrix.
	friend Matrix2x2 Transpose(const Matrix2x2& mat);

	//Returns the determinant of the matrix.
	friend float Determinant(const Matrix2x2& mat);

	//Returns the inverse of the matrix.
	//Returns the identity matrix if mat is noninvertible (singular)
	friend Matrix2x2 Inverse(const Matrix2x2& mat);

	//Returns a rotation matrix about the z-axis with the specified angle.
	//Returns the result of matA + matB.
	friend Matrix2x2 operator+(const Matrix2x2& matA, const Matrix2x2& matB);

	//Returns the result of matA + matB.
	friend Matrix2x2 operator-(const Matrix2x2& matA, const Matrix2x2& matB);

	//Returns the result of matA * matB (matrix-matrix multiplication).
	friend Matrix2x2 operator*(const Matrix2x2& matA, const Matrix2x2& matB);

	//Returns the result of mat * vec (matrix-vector multiplication).
	friend vec2 operator*(const Matrix2x2& mat, const vec2& vec);

	//Returns the result of mat * vec (vector-matrix multiplication).
	friend vec2 operator*(const vec2& vec, const Matrix2x2& mat);

	//Returns the result of mat * k
	friend Matrix2x2 operator*(const Matrix2x2& mat, float k);

	//Returns the result of k * mat
	friend Matrix2x2 operator*(float k, const Matrix2x2& mat);

	//Returns the result of mat / k
	friend Matrix2x2 operator/(const Matrix2x2& mat, float k);


private:
	float mat[2][2];
};

inline Matrix2x2::Matrix2x2()
{
	mat[0][0] = 0.0f;
	mat[0][1] = 0.0f;

	mat[1][0] = 0.0f;
	mat[1][1] = 0.0f;
}

inline Matrix2x2::Matrix2x2(vec2 row0, vec2 row1)
{
	SetRow(0, row0);
	SetRow(1, row1);
}

inline Matrix2x2::Matrix2x2(float m00, float m01,
	float m10, float m11)
{
	mat[0][0] = m00;
	mat[0][1] = m01;

	mat[1][0] = m10;
	mat[1][1] = m11;
}

inline float Matrix2x2::GetElement(uint32_t row, uint32_t col) const
{
	assert((row < 2 && col < 2) && "Row or Col is greater than 1");

	return mat[row][col];
}

inline vec2 Matrix2x2::GetRow(uint32_t row) const
{
	assert(row < 2 && "Row is greater than 1");
	vec2 result(mat[row][0], mat[row][1]);
	return result;
}

inline vec2 Matrix2x2::GetCol(uint32_t col) const
{
	assert(col < 2 && "Col is greater than 1");
	vec2 result(mat[0][col], mat[1][col]);
	return result;
}

inline void Matrix2x2::SetElement(uint32_t row, uint32_t col, float value)
{
	assert((row < 2 && col < 2) && "Row or Col is greater than 1");
	mat[row][col] = value;
}

inline void Matrix2x2::SetRow(uint32_t row, vec2 values)
{
	assert(row < 2 && "Row is greater than 1");

	mat[row][0] = values.GetX();
	mat[row][1] = values.GetY();
}

inline void Matrix2x2::SetRow(uint32_t row, float val0, float val1)
{
	assert(row < 2 && "Row is greater than 1");

	mat[row][0] = val0;
	mat[row][1] = val1;
}

inline void Matrix2x2::SetCol(uint32_t col, float val0, float val1)
{
	assert(col < 2 && "Col is greater than 1");

	mat[0][col] = val0;
	mat[1][col] = val1;
}

inline void Matrix2x2::SetCol(uint32_t col, vec2 values)
{
	assert(col < 2 && "Col is greater than 1");
	return;

	mat[0][col] = values.GetX();
	mat[1][col] = values.GetY();
}

inline Matrix2x2 Matrix2x2::MakeIdentity()
{
	return Matrix2x2(1.0f, 0.0f,
		0.0f, 1.0f);
}

inline Matrix2x2 Matrix2x2::Scale(float x, float y)
{
	return Matrix2x2(x, 0.0f,
		0.0f, y);
}

inline Matrix2x2 Matrix2x2::RotZ(float angle)
{
	float ang = angle * (PI / 180.0f);
	float cosAngle = cos(ang);
	float sinAngle = sin(ang);

	return Matrix2x2(cosAngle, sinAngle,
		-sinAngle, cosAngle);
}

inline Matrix2x2& Matrix2x2::operator+=(const Matrix2x2& matB)
{
	this->mat[0][0] += matB.mat[0][0];
	this->mat[0][1] += matB.mat[0][1];

	this->mat[1][0] += matB.mat[1][0];
	this->mat[1][1] += matB.mat[1][1];

	return *this;
}

inline Matrix2x2& Matrix2x2::operator-=(const Matrix2x2& matB)
{
	this->mat[0][0] -= matB.mat[0][0];
	this->mat[0][1] -= matB.mat[0][1];

	this->mat[1][0] -= matB.mat[1][0];
	this->mat[1][1] -= matB.mat[1][1];

	return *this;
}

inline Matrix2x2& Matrix2x2::operator*=(const Matrix2x2& matB)
{
	Matrix2x2 result;

	result.mat[0][0] = this->mat[0][0] * matB.mat[0][0] + this->mat[0][1] * matB.mat[1][0];
	result.mat[0][1] = this->mat[0][0] * matB.mat[0][1] + this->mat[0][1] * matB.mat[1][1];

	result.mat[1][0] = this->mat[1][0] * matB.mat[0][0] + this->mat[1][1] * matB.mat[1][0];
	result.mat[1][1] = this->mat[1][0] * matB.mat[0][1] + this->mat[1][1] * matB.mat[1][1];

	*this = result;

	return *this;
}

inline Matrix2x2& Matrix2x2::operator*=(float k)
{
	this->mat[0][0] *= k;
	this->mat[0][1] *= k;

	this->mat[1][0] *= k;
	this->mat[1][1] *= k;


	return *this;
}

inline Matrix2x2& Matrix2x2::operator/=(float k)
{
	this->mat[0][0] /= k;
	this->mat[0][1] /= k;

	this->mat[1][0] /= k;
	this->mat[1][1] /= k;

	return *this;
}

inline bool IsIdentity(const Matrix2x2& mat)
{
	return
		CompareFloats(mat.mat[0][0], 1.0f) &&
		CompareFloats(mat.mat[0][1], 0.0f) &&

		CompareFloats(mat.mat[1][0], 0.0f) &&
		CompareFloats(mat.mat[1][1], 1.0f);
}

inline Matrix2x2 Transpose(const Matrix2x2& mat)
{
	Matrix2x2 result;

	result.mat[0][0] = mat.mat[0][0];
	result.mat[0][1] = mat.mat[1][0];

	result.mat[1][0] = mat.mat[0][1];
	result.mat[1][1] = mat.mat[1][1];

	return result;
}

inline float Determinant(const Matrix2x2& mat)
{
	return mat.mat[0][0] * mat.mat[1][1] - mat.mat[0][1] * mat.mat[1][0];
}


inline Matrix2x2 Inverse(const Matrix2x2& mat)
{
	float det = Determinant(mat);
	if (CompareFloats(det, 0.0f))
		return Matrix2x2();

	Matrix2x2 cofactors;

	cofactors.mat[0][0] = mat.mat[1][1];

	cofactors.mat[0][1] = -mat.mat[1][0];

	cofactors.mat[1][0] = -mat.mat[0][1];

	cofactors.mat[1][1] = mat.mat[0][0];

	return (1.0f / det) * Transpose(cofactors);
}

inline Matrix2x2 operator+(const Matrix2x2& matA, const Matrix2x2& matB)
{
	Matrix2x2 result;

	result.mat[0][0] = matA.mat[0][0] + matB.mat[0][0];
	result.mat[0][1] = matA.mat[0][1] + matB.mat[0][1];

	result.mat[1][0] = matA.mat[1][0] + matB.mat[1][0];
	result.mat[1][1] = matA.mat[1][1] + matB.mat[1][1];

	return result;
}

inline Matrix2x2 operator-(const Matrix2x2& matA, const Matrix2x2& matB)
{
	Matrix2x2 result;

	result.mat[0][0] = matA.mat[0][0] - matB.mat[0][0];
	result.mat[0][1] = matA.mat[0][1] - matB.mat[0][1];

	result.mat[1][0] = matA.mat[1][0] - matB.mat[1][0];
	result.mat[1][1] = matA.mat[1][1] - matB.mat[1][1];

	return result;
}

inline Matrix2x2 operator*(const Matrix2x2& matA, const Matrix2x2& matB)
{
	Matrix2x2 result;

	result.mat[0][0] = matA.mat[0][0] * matB.mat[0][0] + matA.mat[0][1] * matB.mat[1][0];
	result.mat[0][1] = matA.mat[0][0] * matB.mat[0][1] + matA.mat[0][1] * matB.mat[1][1];

	result.mat[1][0] = matA.mat[1][0] * matB.mat[0][0] + matA.mat[1][1] * matB.mat[1][0];
	result.mat[1][1] = matA.mat[1][0] * matB.mat[0][1] + matA.mat[1][1] * matB.mat[1][1];

	return result;
}

inline vec2 operator*(const Matrix2x2& mat, const vec2& vec)
{
	vec2 result;

	result.SetX(mat.mat[0][0] * vec.GetX() + mat.mat[0][1] * vec.GetY());
	result.SetY(mat.mat[1][0] * vec.GetX() + mat.mat[1][1] * vec.GetY());

	return result;
}

inline vec2 operator*(const vec2& vec, const Matrix2x2& mat)
{
	vec2 result;

	result.SetX(vec.GetX() * mat.mat[0][0] + vec.GetY() * mat.mat[1][0]);
	result.SetY(vec.GetX() * mat.mat[0][1] + vec.GetY() * mat.mat[1][1]);

	return result;
}

inline Matrix2x2 operator*(const Matrix2x2& mat, float k)
{
	Matrix2x2 result;

	result.mat[0][0] = mat.mat[0][0] * k;
	result.mat[0][1] = mat.mat[0][1] * k;

	result.mat[1][0] = mat.mat[1][0] * k;
	result.mat[1][1] = mat.mat[1][1] * k;

	return result;
}

inline Matrix2x2 operator*(float k, const Matrix2x2& mat)
{
	Matrix2x2 result;

	result.mat[0][0] = k * mat.mat[0][0];
	result.mat[0][1] = k * mat.mat[0][1];

	result.mat[1][0] = k * mat.mat[1][0];
	result.mat[1][1] = k * mat.mat[1][1];

	return result;
}

inline Matrix2x2 operator/(const Matrix2x2& mat, float k)
{
	Matrix2x2 result;

	result.mat[0][0] = mat.mat[0][0] / k;
	result.mat[0][1] = mat.mat[0][1] / k;

	result.mat[1][0] = mat.mat[1][0] / k;
	result.mat[1][1] = mat.mat[1][1] / k;

	return result;
}
//------------------------------------------------------------------------------------------------------------

//MATRIX3X3
//------------------------------------------------------------------------------------------------------------

class Matrix3x3
{
public:

	//Creates an identity matrix
	Matrix3x3();

	//Creates a matrix with the specified rows.
	Matrix3x3(vec3 row0, vec3 row1, vec3 row2);

	//Creates a matrix with the specified values.
	Matrix3x3(float m00, float m01, float m02,
		float m10, float m11, float m12,
		float m20, float m21, float m22);

	//Returns the element at the specified [row][col]
	//If row or col is > 3 the first element of the matrix is returned.
	float GetElement(uint32_t row, uint32_t col) const;

	//Returns the specified row.
	vec3 GetRow(uint32_t row) const;

	//Returns the specified col.
	vec3 GetCol(uint32_t col) const;

	//Sets the specified [row][col] to the specified value.
	void SetElement(uint32_t row, uint32_t col, float value);

	//Sets the specified row to the specified values.
	void SetRow(uint32_t row, vec3 values);

	//Sets the specified row to the specified values.
	void SetRow(uint32_t row, float val0, float val1, float val2);

	//Sets the specified row to the specified values.
	void SetCol(uint32_t col, float val0, float val1, float val2);

	//Sets the specified row to the specified values.
	void SetCol(uint32_t col, vec3 values);

	//Returns the identity matrix
	static Matrix3x3 MakeIdentity();

	//Returns a scale matrix with the specified values.
	static Matrix3x3 Scale(float x, float y, float z);

	//Returns a translation matrix with the specified.
	static Matrix3x3 Translate(float x, float y);

	//Returns a rotation matrix about the x-axis with the specified angle.
	//Angle should be given in degrees.
	static Matrix3x3 RotX(float angle);

	//Returns a rotation matrix about the y-axis with the specified angle.
	//Angle should be given in degrees.
	static Matrix3x3 RotY(float angle);

	//Returns a rotation matrix about the z-axis with the specified angle.
	//Angle should be given in degrees.
	static Matrix3x3 RotZ(float angle);

	//Returns the result of this + matB.
	Matrix3x3& operator+=(const Matrix3x3& matB);

	//Returns the result of this - matB.
	Matrix3x3& operator-=(const Matrix3x3& matB);

	//Returns the result of this * matB (matrix-matrix multiplication).
	Matrix3x3& operator*=(const Matrix3x3& matB);

	//Returns the result of this * k.
	Matrix3x3& operator*=(float k);

	//Returns the result of this / k.
	Matrix3x3& operator/=(float k);

	//Returns true if the matrix is an identity matrix, false otherwise.
	friend bool IsIdentity(const Matrix3x3& mat);

	//Returns the transposition of the matrix.
	friend Matrix3x3 Transpose(const Matrix3x3& mat);

	//Returns the determinant of the matrix.
	friend float Determinant(const Matrix3x3& mat);

	//Returns the inverse of the matrix.
	//Returns the identity matrix if mat is noninvertible (singular)
	friend Matrix3x3 Inverse(const Matrix3x3& mat);

	//Returns a rotation matrix about the z-axis with the specified angle.
	//Returns the result of matA + matB.
	friend Matrix3x3 operator+(const Matrix3x3& matA, const Matrix3x3& matB);

	//Returns the result of matA + matB.
	friend Matrix3x3 operator-(const Matrix3x3& matA, const Matrix3x3& matB);

	//Returns the result of matA * matB (matrix-matrix multiplication).
	friend Matrix3x3 operator*(const Matrix3x3& matA, const Matrix3x3& matB);

	//Returns the result of mat * vec (matrix-vector multiplication).
	friend vec3 operator*(const Matrix3x3& mat, const vec3& vec);

	//Returns the result of mat * vec (vector-matrix multiplication).
	friend vec3 operator*(const vec3& vec, const Matrix3x3& mat);

	//Returns the result of mat * k
	friend Matrix3x3 operator*(const Matrix3x3& mat, float k);

	//Returns the result of k * mat
	friend Matrix3x3 operator*(float k, const Matrix3x3& mat);

	//Returns the result of mat / k
	friend Matrix3x3 operator/(const Matrix3x3& mat, float k);


private:
	float mat[3][3];
};

inline Matrix3x3::Matrix3x3()
{
	mat[0][0] = 0.0f;
	mat[0][1] = 0.0f;
	mat[0][2] = 0.0f;

	mat[1][0] = 0.0f;
	mat[1][1] = 0.0f;
	mat[1][2] = 0.0f;

	mat[2][0] = 0.0f;
	mat[2][1] = 0.0f;
	mat[2][2] = 0.0f;
}

inline Matrix3x3::Matrix3x3(vec3 row0, vec3 row1, vec3 row2)
{
	SetRow(0, row0);
	SetRow(1, row1);
	SetRow(2, row2);
}

inline Matrix3x3::Matrix3x3(float m00, float m01, float m02,
	float m10, float m11, float m12,
	float m20, float m21, float m22)
{
	mat[0][0] = m00;
	mat[0][1] = m01;
	mat[0][2] = m02;

	mat[1][0] = m10;
	mat[1][1] = m11;
	mat[1][2] = m12;

	mat[2][0] = m20;
	mat[2][1] = m21;
	mat[2][2] = m22;
}

inline float Matrix3x3::GetElement(uint32_t row, uint32_t col) const
{
	assert((row < 3 && col < 3) && "Row or Col is greater than 2");

	return mat[row][col];
}

inline vec3 Matrix3x3::GetRow(uint32_t row) const
{
	assert(row < 3 && "Row is greater than 2");
	vec3 result(mat[row][0], mat[row][1], mat[row][2]);
	return result;
}

inline vec3 Matrix3x3::GetCol(uint32_t col) const
{
	assert(col < 3 && "Col is greater than 2");
	vec3 result(mat[0][col], mat[1][col], mat[2][col]);
	return result;
}

inline void Matrix3x3::SetElement(uint32_t row, uint32_t col, float value)
{
	assert((row < 3 && col < 3) && "Row or Col is greater than 2");
	mat[row][col] = value;
}

inline void Matrix3x3::SetRow(uint32_t row, vec3 values)
{
	assert(row < 3 && "Row is greater than 2");

	mat[row][0] = values.GetX();
	mat[row][1] = values.GetY();
	mat[row][2] = values.GetZ();
}

inline void Matrix3x3::SetRow(uint32_t row, float val0, float val1, float val2)
{
	assert(row < 3 && "Row is greater than 2");

	mat[row][0] = val0;
	mat[row][1] = val1;
	mat[row][2] = val2;
}

inline void Matrix3x3::SetCol(uint32_t col, float val0, float val1, float val2)
{
	assert(col < 3 && "Col is greater than 2");
	mat[0][col] = val0;
	mat[1][col] = val1;
	mat[2][col] = val2;
}

inline void Matrix3x3::SetCol(uint32_t col, vec3 values)
{
	assert(col < 3 && "Col is greater than 2");

	mat[0][col] = values.GetX();
	mat[1][col] = values.GetY();
	mat[2][col] = values.GetZ();
}

inline Matrix3x3 Matrix3x3::MakeIdentity()
{
	return Matrix3x3(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f);
}

inline Matrix3x3 Matrix3x3::Scale(float x, float y, float z)
{
	return Matrix3x3(x, 0.0f, 0.0f,
		0.0f, y, 0.0f,
		0.0f, 0.0f, z);
}

inline Matrix3x3 Matrix3x3::Translate(float x, float y)
{
	return Matrix3x3(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		x, y, 1.0f);
}

inline Matrix3x3 Matrix3x3::RotX(float angle)
{
	float ang = angle * (PI / 180.0f);
	float cosAngle = cos(ang);
	float sinAngle = sin(ang);

	return Matrix3x3(1.0f, 0.0f, 0.0f,
		0.0f, cosAngle, sinAngle,
		0.0f, -sinAngle, cosAngle);
}

inline Matrix3x3 Matrix3x3::RotY(float angle)
{
	float ang = angle * (PI / 180.0f);
	float cosAngle = cos(ang);
	float sinAngle = sin(ang);

	return Matrix3x3(cosAngle, 0.0f, -sinAngle,
		0.0f, 1.0f, 0.0f,
		sinAngle, 0.0f, cosAngle);
}

inline Matrix3x3 Matrix3x3::RotZ(float angle)
{
	float ang = angle * (PI / 180.0f);
	float cosAngle = cos(ang);
	float sinAngle = sin(ang);

	return Matrix3x3(cosAngle, sinAngle, 0.0f,
		-sinAngle, cosAngle, 0.0f,
		0.0f, 0.0f, 1.0f);
}

inline Matrix3x3& Matrix3x3::operator+=(const Matrix3x3& matB)
{
	this->mat[0][0] += matB.mat[0][0];
	this->mat[0][1] += matB.mat[0][1];
	this->mat[0][2] += matB.mat[0][2];

	this->mat[1][0] += matB.mat[1][0];
	this->mat[1][1] += matB.mat[1][1];
	this->mat[1][2] += matB.mat[1][2];

	this->mat[2][0] += matB.mat[2][0];
	this->mat[2][1] += matB.mat[2][1];
	this->mat[2][2] += matB.mat[2][2];

	return *this;
}

inline Matrix3x3& Matrix3x3::operator-=(const Matrix3x3& matB)
{
	this->mat[0][0] -= matB.mat[0][0];
	this->mat[0][1] -= matB.mat[0][1];
	this->mat[0][2] -= matB.mat[0][2];

	this->mat[1][0] -= matB.mat[1][0];
	this->mat[1][1] -= matB.mat[1][1];
	this->mat[1][2] -= matB.mat[1][2];

	this->mat[2][0] -= matB.mat[2][0];
	this->mat[2][1] -= matB.mat[2][1];
	this->mat[2][2] -= matB.mat[2][2];

	return *this;
}

inline Matrix3x3& Matrix3x3::operator*=(const Matrix3x3& matB)
{
	Matrix3x3 result;

	result.mat[0][0] = this->mat[0][0] * matB.mat[0][0] + this->mat[0][1] * matB.mat[1][0] + this->mat[0][2] * matB.mat[2][0];
	result.mat[0][1] = this->mat[0][0] * matB.mat[0][1] + this->mat[0][1] * matB.mat[1][1] + this->mat[0][2] * matB.mat[2][1];
	result.mat[0][2] = this->mat[0][0] * matB.mat[0][2] + this->mat[0][1] * matB.mat[1][2] + this->mat[0][2] * matB.mat[2][2];

	result.mat[1][0] = this->mat[1][0] * matB.mat[0][0] + this->mat[1][1] * matB.mat[1][0] + this->mat[1][2] * matB.mat[2][0];
	result.mat[1][1] = this->mat[1][0] * matB.mat[0][1] + this->mat[1][1] * matB.mat[1][1] + this->mat[1][2] * matB.mat[2][1];
	result.mat[1][2] = this->mat[1][0] * matB.mat[0][2] + this->mat[1][1] * matB.mat[1][2] + this->mat[1][2] * matB.mat[2][2];

	result.mat[2][0] = this->mat[2][0] * matB.mat[0][0] + this->mat[2][1] * matB.mat[1][0] + this->mat[2][2] * matB.mat[2][0];
	result.mat[2][1] = this->mat[2][0] * matB.mat[0][1] + this->mat[2][1] * matB.mat[1][1] + this->mat[2][2] * matB.mat[2][1];
	result.mat[2][2] = this->mat[2][0] * matB.mat[0][2] + this->mat[2][1] * matB.mat[1][2] + this->mat[2][2] * matB.mat[2][2];

	*this = result;

	return *this;
}

inline Matrix3x3& Matrix3x3::operator*=(float k)
{
	this->mat[0][0] *= k;
	this->mat[0][1] *= k;
	this->mat[0][2] *= k;

	this->mat[1][0] *= k;
	this->mat[1][1] *= k;
	this->mat[1][2] *= k;

	this->mat[2][0] *= k;
	this->mat[2][1] *= k;
	this->mat[2][2] *= k;

	return *this;
}

inline Matrix3x3& Matrix3x3::operator/=(float k)
{
	this->mat[0][0] /= k;
	this->mat[0][1] /= k;
	this->mat[0][2] /= k;

	this->mat[1][0] /= k;
	this->mat[1][1] /= k;
	this->mat[1][2] /= k;

	this->mat[2][0] /= k;
	this->mat[2][1] /= k;
	this->mat[2][2] /= k;

	return *this;
}

inline bool IsIdentity(const Matrix3x3& mat)
{
	return
		CompareFloats(mat.mat[0][0], 1.0f) &&
		CompareFloats(mat.mat[0][1], 0.0f) &&
		CompareFloats(mat.mat[0][2], 0.0f) &&

		CompareFloats(mat.mat[1][0], 0.0f) &&
		CompareFloats(mat.mat[1][1], 1.0f) &&
		CompareFloats(mat.mat[1][2], 0.0f) &&

		CompareFloats(mat.mat[2][0], 0.0f) &&
		CompareFloats(mat.mat[2][1], 0.0f) &&
		CompareFloats(mat.mat[2][2], 1.0f);
}

inline Matrix3x3 Transpose(const Matrix3x3& mat)
{
	Matrix3x3 result;

	result.mat[0][0] = mat.mat[0][0];
	result.mat[0][1] = mat.mat[1][0];
	result.mat[0][2] = mat.mat[2][0];

	result.mat[1][0] = mat.mat[0][1];
	result.mat[1][1] = mat.mat[1][1];
	result.mat[1][2] = mat.mat[2][1];

	result.mat[2][0] = mat.mat[0][2];
	result.mat[2][1] = mat.mat[1][2];
	result.mat[2][2] = mat.mat[2][2];

	return result;
}

inline float Determinant(const Matrix3x3& mat)
{
	float t0 = mat.mat[0][0] * (mat.mat[1][1] * mat.mat[2][2] - mat.mat[1][2] * mat.mat[2][1]);

	float t1 = mat.mat[0][1] * (mat.mat[1][2] * mat.mat[2][0] - mat.mat[1][0] * mat.mat[2][2]);

	float t2 = mat.mat[0][2] * (mat.mat[1][0] * mat.mat[2][1] - mat.mat[1][1] * mat.mat[2][0]);

	return t0 + t1 + t2;
}


inline Matrix3x3 Inverse(const Matrix3x3& mat)
{
	float det = Determinant(mat);
	if (CompareFloats(det, 0.0f))
		return Matrix3x3();

	Matrix3x3 cofactors;

	cofactors.mat[0][0] = Determinant(Matrix2x2(mat.mat[1][1], mat.mat[1][2], mat.mat[2][1], mat.mat[2][2]));
	cofactors.mat[0][1] = -Determinant(Matrix2x2(mat.mat[1][0], mat.mat[1][2], mat.mat[2][0], mat.mat[2][2]));
	cofactors.mat[0][2] = Determinant(Matrix2x2(mat.mat[1][0], mat.mat[1][1], mat.mat[2][0], mat.mat[2][1]));

	cofactors.mat[1][0] = -Determinant(Matrix2x2(mat.mat[0][1], mat.mat[0][2], mat.mat[2][1], mat.mat[2][2]));
	cofactors.mat[1][1] = Determinant(Matrix2x2(mat.mat[0][0], mat.mat[0][2], mat.mat[2][0], mat.mat[2][2]));
	cofactors.mat[1][2] = -Determinant(Matrix2x2(mat.mat[0][0], mat.mat[0][1], mat.mat[2][0], mat.mat[2][1]));

	cofactors.mat[2][0] = Determinant(Matrix2x2(mat.mat[0][1], mat.mat[0][2], mat.mat[1][1], mat.mat[1][2]));
	cofactors.mat[2][1] = -Determinant(Matrix2x2(mat.mat[0][0], mat.mat[0][2], mat.mat[1][0], mat.mat[1][2]));
	cofactors.mat[2][2] = Determinant(Matrix2x2(mat.mat[0][0], mat.mat[0][1], mat.mat[1][0], mat.mat[1][1]));

	return (1.0f / det) * Transpose(cofactors);
}

inline Matrix3x3 operator+(const Matrix3x3& matA, const Matrix3x3& matB)
{
	Matrix3x3 result;

	result.mat[0][0] = matA.mat[0][0] + matB.mat[0][0];
	result.mat[0][1] = matA.mat[0][1] + matB.mat[0][1];
	result.mat[0][2] = matA.mat[0][2] + matB.mat[0][2];

	result.mat[1][0] = matA.mat[1][0] + matB.mat[1][0];
	result.mat[1][1] = matA.mat[1][1] + matB.mat[1][1];
	result.mat[1][2] = matA.mat[1][2] + matB.mat[1][2];

	result.mat[2][0] = matA.mat[2][0] + matB.mat[2][0];
	result.mat[2][1] = matA.mat[2][1] + matB.mat[2][1];
	result.mat[2][2] = matA.mat[2][2] + matB.mat[2][2];

	return result;
}

inline Matrix3x3 operator-(const Matrix3x3& matA, const Matrix3x3& matB)
{
	Matrix3x3 result;

	result.mat[0][0] = matA.mat[0][0] - matB.mat[0][0];
	result.mat[0][1] = matA.mat[0][1] - matB.mat[0][1];
	result.mat[0][2] = matA.mat[0][2] - matB.mat[0][2];

	result.mat[1][0] = matA.mat[1][0] - matB.mat[1][0];
	result.mat[1][1] = matA.mat[1][1] - matB.mat[1][1];
	result.mat[1][2] = matA.mat[1][2] - matB.mat[1][2];

	result.mat[2][0] = matA.mat[2][0] - matB.mat[2][0];
	result.mat[2][1] = matA.mat[2][1] - matB.mat[2][1];
	result.mat[2][2] = matA.mat[2][2] - matB.mat[2][2];

	return result;
}

inline Matrix3x3 operator*(const Matrix3x3& matA, const Matrix3x3& matB)
{
	Matrix3x3 result;

	result.mat[0][0] = matA.mat[0][0] * matB.mat[0][0] + matA.mat[0][1] * matB.mat[1][0] + matA.mat[0][2] * matB.mat[2][0];
	result.mat[0][1] = matA.mat[0][0] * matB.mat[0][1] + matA.mat[0][1] * matB.mat[1][1] + matA.mat[0][2] * matB.mat[2][1];
	result.mat[0][2] = matA.mat[0][0] * matB.mat[0][2] + matA.mat[0][1] * matB.mat[1][2] + matA.mat[0][2] * matB.mat[2][2];

	result.mat[1][0] = matA.mat[1][0] * matB.mat[0][0] + matA.mat[1][1] * matB.mat[1][0] + matA.mat[1][2] * matB.mat[2][0];
	result.mat[1][1] = matA.mat[1][0] * matB.mat[0][1] + matA.mat[1][1] * matB.mat[1][1] + matA.mat[1][2] * matB.mat[2][1];
	result.mat[1][2] = matA.mat[1][0] * matB.mat[0][2] + matA.mat[1][1] * matB.mat[1][2] + matA.mat[1][2] * matB.mat[2][2];

	result.mat[2][0] = matA.mat[2][0] * matB.mat[0][0] + matA.mat[2][1] * matB.mat[1][0] + matA.mat[2][2] * matB.mat[2][0];
	result.mat[2][1] = matA.mat[2][0] * matB.mat[0][1] + matA.mat[2][1] * matB.mat[1][1] + matA.mat[2][2] * matB.mat[2][1];
	result.mat[2][2] = matA.mat[2][0] * matB.mat[0][2] + matA.mat[2][1] * matB.mat[1][2] + matA.mat[2][2] * matB.mat[2][2];

	return result;
}

inline vec3 operator*(const Matrix3x3& mat, const vec3& vec)
{
	vec3 result;

	result.SetX(mat.mat[0][0] * vec.GetX() + mat.mat[0][1] * vec.GetY() + mat.mat[0][2] * vec.GetZ());
	result.SetY(mat.mat[1][0] * vec.GetX() + mat.mat[1][1] * vec.GetY() + mat.mat[1][2] * vec.GetZ());
	result.SetZ(mat.mat[2][0] * vec.GetX() + mat.mat[2][1] * vec.GetY() + mat.mat[2][2] * vec.GetZ());

	return result;
}

inline vec3 operator*(const vec3& vec, const Matrix3x3& mat)
{
	vec3 result;

	result.SetX(vec.GetX() * mat.mat[0][0] + vec.GetY() * mat.mat[1][0] + vec.GetZ() * mat.mat[2][0]);
	result.SetY(vec.GetX() * mat.mat[0][1] + vec.GetY() * mat.mat[1][1] + vec.GetZ() * mat.mat[2][1]);
	result.SetZ(vec.GetX() * mat.mat[0][2] + vec.GetY() * mat.mat[1][2] + vec.GetZ() * mat.mat[2][2]);

	return result;
}

inline Matrix3x3 operator*(const Matrix3x3& mat, float k)
{
	Matrix3x3 result;

	result.mat[0][0] = mat.mat[0][0] * k;
	result.mat[0][1] = mat.mat[0][1] * k;
	result.mat[0][2] = mat.mat[0][2] * k;

	result.mat[1][0] = mat.mat[1][0] * k;
	result.mat[1][1] = mat.mat[1][1] * k;
	result.mat[1][2] = mat.mat[1][2] * k;

	result.mat[2][0] = mat.mat[2][0] * k;
	result.mat[2][1] = mat.mat[2][1] * k;
	result.mat[2][2] = mat.mat[2][2] * k;

	return result;
}

inline Matrix3x3 operator*(float k, const Matrix3x3& mat)
{
	Matrix3x3 result;

	result.mat[0][0] = k * mat.mat[0][0];
	result.mat[0][1] = k * mat.mat[0][1];
	result.mat[0][2] = k * mat.mat[0][2];

	result.mat[1][0] = k * mat.mat[1][0];
	result.mat[1][1] = k * mat.mat[1][1];
	result.mat[1][2] = k * mat.mat[1][2];

	result.mat[2][0] = k * mat.mat[2][0];
	result.mat[2][1] = k * mat.mat[2][1];
	result.mat[2][2] = k * mat.mat[2][2];

	return result;
}

inline Matrix3x3 operator/(const Matrix3x3& mat, float k)
{
	Matrix3x3 result;

	result.mat[0][0] = mat.mat[0][0] / k;
	result.mat[0][1] = mat.mat[0][1] / k;
	result.mat[0][2] = mat.mat[0][2] / k;

	result.mat[1][0] = mat.mat[1][0] / k;
	result.mat[1][1] = mat.mat[1][1] / k;
	result.mat[1][2] = mat.mat[1][2] / k;

	result.mat[2][0] = mat.mat[2][0] / k;
	result.mat[2][1] = mat.mat[2][1] / k;
	result.mat[2][2] = mat.mat[2][2] / k;

	return result;
}
//------------------------------------------------------------------------------------------------------------

//MATRIX4X4
//------------------------------------------------------------------------------------------------------------
class Matrix4x4
{
public:

	//Creates an identity matrix
	Matrix4x4();

	//Creates a matrix with the specified rows.
	Matrix4x4(vec4 row0, vec4 row1, vec4 row2, vec4 row3);

	//Creates a matrix with the specified values.
	Matrix4x4(float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33);

	//Returns the element at the specified [row][col]
	float GetElement(uint32_t row, uint32_t col) const;

	//Returns the specified row.
	vec4 GetRow(uint32_t row) const;

	//Returns the specified col.
	vec4 GetCol(uint32_t col) const;

	//Sets the specified [row][col] to the specified value.
	void SetElement(uint32_t row, uint32_t col, float value);

	//Sets the specified row to the specified values.
	void SetRow(uint32_t row, vec4 values);

	//Sets the specified row to the specified values.
	void SetRow(uint32_t row, float val0, float val1, float val2, float val3);

	//Sets the specified row to the specified values.
	void SetCol(uint32_t col, float val0, float val1, float val2, float val3);

	//Sets the specified row to the specified values.
	void SetCol(uint32_t col, vec4 values);

	//Returns the identity matrix
	static Matrix4x4 MakeIdentity();

	//Returns a scale matrix with the specified values.
	static Matrix4x4 Scale(float x, float y, float z);

	//Returns a translation matrix with the specified.
	static Matrix4x4 Translate(float x, float y, float z);

	//Returns a rotation matrix about the x-axis with the specified angle.
	//Angle should be given in degrees.
	static Matrix4x4 RotX(float angle);

	//Returns a rotation matrix about the y-axis with the specified angle.
	//Angle should be given in degrees.
	static Matrix4x4 RotY(float angle);

	//Returns a rotation matrix about the z-axis with the specified angle.
	//Angle should be given in degrees.
	static Matrix4x4 RotZ(float angle);

	//Returns the result of this + matB.
	Matrix4x4& operator+=(const Matrix4x4& matB);

	//Returns the result of this - matB.
	Matrix4x4& operator-=(const Matrix4x4& matB);

	//Returns the result of this * matB (matrix-matrix multiplication).
	Matrix4x4& operator*=(const Matrix4x4& matB);

	//Returns the result of this * k.
	Matrix4x4& operator*=(float k);

	//Returns the result of this / k.
	Matrix4x4& operator/=(float k);

	//Returns true if the matrix is an identity matrix, false otherwise.
	friend bool IsIdentity(const Matrix4x4& mat);

	//Returns the transposition of the matrix.
	friend Matrix4x4 Transpose(const Matrix4x4& mat);

	//Returns the determinant of the matrix.
	friend float Determinant(const Matrix4x4& mat);

	//Returns the inverse of the matrix.
	//Returns the identity matrix if mat is noninvertible (singular)
	friend Matrix4x4 Inverse(const Matrix4x4& mat);

	//Returns a rotation matrix about the z-axis with the specified angle.
	//Returns the result of matA + matB.
	friend Matrix4x4 operator+(const Matrix4x4& matA, const Matrix4x4& matB);

	//Returns the result of matA + matB.
	friend Matrix4x4 operator-(const Matrix4x4& matA, const Matrix4x4& matB);

	//Returns the result of matA * matB (matrix-matrix multiplication).
	friend Matrix4x4 operator*(const Matrix4x4& matA, const Matrix4x4& matB);

	//Returns the result of mat * vec (matrix-vector multiplication).
	friend vec4 operator*(const Matrix4x4& mat, const vec4& vec);

	//Returns the result of mat * vec (vector-matrix multiplication).
	friend vec4 operator*(const vec4& vec, const Matrix4x4& mat);

	//Returns the result of mat * k
	friend Matrix4x4 operator*(const Matrix4x4& mat, float k);

	//Returns the result of k * mat
	friend Matrix4x4 operator*(float k, const Matrix4x4& mat);

	//Returns the result of mat / k
	friend Matrix4x4 operator/(const Matrix4x4& mat, float k);


private:
	float mat[4][4];
};

inline Matrix4x4::Matrix4x4()
{
	mat[0][0] = 0.0f;
	mat[0][1] = 0.0f;
	mat[0][2] = 0.0f;
	mat[0][3] = 0.0f;

	mat[1][0] = 0.0f;
	mat[1][1] = 0.0f;
	mat[1][2] = 0.0f;
	mat[1][3] = 0.0f;

	mat[2][0] = 0.0f;
	mat[2][1] = 0.0f;
	mat[2][2] = 0.0f;
	mat[2][3] = 0.0f;

	mat[3][0] = 0.0f;
	mat[3][1] = 0.0f;
	mat[3][2] = 0.0f;
	mat[3][3] = 0.0f;
}

inline Matrix4x4::Matrix4x4(vec4 row0, vec4 row1, vec4 row2, vec4 row3)
{
	SetRow(0, row0);
	SetRow(1, row1);
	SetRow(2, row2);
	SetRow(3, row3);
}

inline Matrix4x4::Matrix4x4(float m00, float m01, float m02, float m03,
	float m10, float m11, float m12, float m13,
	float m20, float m21, float m22, float m23,
	float m30, float m31, float m32, float m33)
{
	mat[0][0] = m00;
	mat[0][1] = m01;
	mat[0][2] = m02;
	mat[0][3] = m03;

	mat[1][0] = m10;
	mat[1][1] = m11;
	mat[1][2] = m12;
	mat[1][3] = m13;

	mat[2][0] = m20;
	mat[2][1] = m21;
	mat[2][2] = m22;
	mat[2][3] = m23;

	mat[3][0] = m30;
	mat[3][1] = m31;
	mat[3][2] = m32;
	mat[3][3] = m33;
}

inline float Matrix4x4::GetElement(uint32_t row, uint32_t col) const
{
	assert((row < 4 && col < 4) && "Row or Col is greater than 3");

	return mat[row][col];
}

inline vec4 Matrix4x4::GetRow(uint32_t row) const
{
	assert(row < 4 && "Row is greater than 3");
	vec4 result(mat[row][0], mat[row][1], mat[row][2], mat[row][3]);
	return result;
}

inline vec4 Matrix4x4::GetCol(uint32_t col) const
{
	assert(col < 4 && "Col is greater than 3");
	vec4 result(mat[0][col], mat[1][col], mat[2][col], mat[3][col]);
	return result;
}

inline void Matrix4x4::SetElement(uint32_t row, uint32_t col, float value)
{
	assert((row < 4 && col < 4) && "Row or Col is greater than 3");
	mat[row][col] = value;
}

inline void Matrix4x4::SetRow(uint32_t row, vec4 values)
{
	assert(row < 4 && "Row is greater than 3");

	mat[row][0] = values.GetX();
	mat[row][1] = values.GetY();
	mat[row][2] = values.GetZ();
	mat[row][3] = values.GetW();
}

inline void Matrix4x4::SetRow(uint32_t row, float val0, float val1, float val2, float val3)
{
	assert(row < 4 && "Row is greater than 3");

	mat[row][0] = val0;
	mat[row][1] = val1;
	mat[row][2] = val2;
	mat[row][3] = val3;
}

inline void Matrix4x4::SetCol(uint32_t col, float val0, float val1, float val2, float val3)
{
	assert(col < 4 && "Col is greater than 3");

	mat[0][col] = val0;
	mat[1][col] = val1;
	mat[2][col] = val2;
	mat[3][col] = val3;
}

inline void Matrix4x4::SetCol(uint32_t col, vec4 values)
{
	assert(col < 4 && "Col is greater than 3");

	mat[0][col] = values.GetX();
	mat[1][col] = values.GetY();
	mat[2][col] = values.GetZ();
	mat[3][col] = values.GetW();
}

inline Matrix4x4 Matrix4x4::MakeIdentity()
{
	return Matrix4x4(1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
}

inline Matrix4x4 Matrix4x4::Scale(float x, float y, float z)
{
	return Matrix4x4(x, 0.0f, 0.0f, 0.0f,
		0.0f, y, 0.0f, 0.0f,
		0.0f, 0.0f, z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
}

inline Matrix4x4 Matrix4x4::Translate(float x, float y, float z)
{
	return Matrix4x4(1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		x, y, z, 1.0f);
}

inline Matrix4x4 Matrix4x4::RotX(float angle)
{
	float ang = angle * (PI / 180.0f);
	float cosAngle = cos(ang);
	float sinAngle = sin(ang);

	return Matrix4x4(1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cosAngle, sinAngle, 0.0f,
		0.0f, -sinAngle, cosAngle, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
}

inline Matrix4x4 Matrix4x4::RotY(float angle)
{
	float ang = angle * (PI / 180.0f);
	float cosAngle = cos(ang);
	float sinAngle = sin(ang);

	return Matrix4x4(cosAngle, 0.0f, -sinAngle, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		sinAngle, 0.0f, cosAngle, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
}

inline Matrix4x4 Matrix4x4::RotZ(float angle)
{
	float ang = angle * (PI / 180.0f);
	float cosAngle = cos(ang);
	float sinAngle = sin(ang);

	return Matrix4x4(cosAngle, sinAngle, 0.0f, 0.0f,
		-sinAngle, cosAngle, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
}

inline Matrix4x4& Matrix4x4::operator+=(const Matrix4x4& matB)
{
	this->mat[0][0] += matB.mat[0][0];
	this->mat[0][1] += matB.mat[0][1];
	this->mat[0][2] += matB.mat[0][2];
	this->mat[0][3] += matB.mat[0][3];

	this->mat[1][0] += matB.mat[1][0];
	this->mat[1][1] += matB.mat[1][1];
	this->mat[1][2] += matB.mat[1][2];
	this->mat[1][3] += matB.mat[1][3];

	this->mat[2][0] += matB.mat[2][0];
	this->mat[2][1] += matB.mat[2][1];
	this->mat[2][2] += matB.mat[2][2];
	this->mat[2][3] += matB.mat[2][3];

	this->mat[3][0] += matB.mat[3][0];
	this->mat[3][1] += matB.mat[3][1];
	this->mat[3][2] += matB.mat[3][2];
	this->mat[3][3] += matB.mat[3][3];

	return *this;
}

inline Matrix4x4& Matrix4x4::operator-=(const Matrix4x4& matB)
{
	this->mat[0][0] -= matB.mat[0][0];
	this->mat[0][1] -= matB.mat[0][1];
	this->mat[0][2] -= matB.mat[0][2];
	this->mat[0][3] -= matB.mat[0][3];

	this->mat[1][0] -= matB.mat[1][0];
	this->mat[1][1] -= matB.mat[1][1];
	this->mat[1][2] -= matB.mat[1][2];
	this->mat[1][3] -= matB.mat[1][3];

	this->mat[2][0] -= matB.mat[2][0];
	this->mat[2][1] -= matB.mat[2][1];
	this->mat[2][2] -= matB.mat[2][2];
	this->mat[2][3] -= matB.mat[2][3];

	this->mat[3][0] -= matB.mat[3][0];
	this->mat[3][1] -= matB.mat[3][1];
	this->mat[3][2] -= matB.mat[3][2];
	this->mat[3][3] -= matB.mat[3][3];

	return *this;
}

inline Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& matB)
{
	Matrix4x4 result;

	result.mat[0][0] = this->mat[0][0] * matB.mat[0][0] + this->mat[0][1] * matB.mat[1][0] + this->mat[0][2] * matB.mat[2][0] + this->mat[0][3] * matB.mat[3][0];
	result.mat[0][1] = this->mat[0][0] * matB.mat[0][1] + this->mat[0][1] * matB.mat[1][1] + this->mat[0][2] * matB.mat[2][1] + this->mat[0][3] * matB.mat[3][1];
	result.mat[0][2] = this->mat[0][0] * matB.mat[0][2] + this->mat[0][1] * matB.mat[1][2] + this->mat[0][2] * matB.mat[2][2] + this->mat[0][3] * matB.mat[3][2];
	result.mat[0][3] = this->mat[0][0] * matB.mat[0][3] + this->mat[0][1] * matB.mat[1][3] + this->mat[0][2] * matB.mat[2][3] + this->mat[0][3] * matB.mat[3][3];

	result.mat[1][0] = this->mat[1][0] * matB.mat[0][0] + this->mat[1][1] * matB.mat[1][0] + this->mat[1][2] * matB.mat[2][0] + this->mat[1][3] * matB.mat[3][0];
	result.mat[1][1] = this->mat[1][0] * matB.mat[0][1] + this->mat[1][1] * matB.mat[1][1] + this->mat[1][2] * matB.mat[2][1] + this->mat[1][3] * matB.mat[3][1];
	result.mat[1][2] = this->mat[1][0] * matB.mat[0][2] + this->mat[1][1] * matB.mat[1][2] + this->mat[1][2] * matB.mat[2][2] + this->mat[1][3] * matB.mat[3][2];
	result.mat[1][3] = this->mat[1][0] * matB.mat[0][3] + this->mat[1][1] * matB.mat[1][3] + this->mat[1][2] * matB.mat[2][3] + this->mat[1][3] * matB.mat[3][3];

	result.mat[2][0] = this->mat[2][0] * matB.mat[0][0] + this->mat[2][1] * matB.mat[1][0] + this->mat[2][2] * matB.mat[2][0] + this->mat[2][3] * matB.mat[3][0];
	result.mat[2][1] = this->mat[2][0] * matB.mat[0][1] + this->mat[2][1] * matB.mat[1][1] + this->mat[2][2] * matB.mat[2][1] + this->mat[2][3] * matB.mat[3][1];
	result.mat[2][2] = this->mat[2][0] * matB.mat[0][2] + this->mat[2][1] * matB.mat[1][2] + this->mat[2][2] * matB.mat[2][2] + this->mat[2][3] * matB.mat[3][2];
	result.mat[2][3] = this->mat[2][0] * matB.mat[0][3] + this->mat[2][1] * matB.mat[1][3] + this->mat[2][2] * matB.mat[2][3] + this->mat[2][3] * matB.mat[3][3];

	result.mat[3][0] = this->mat[3][0] * matB.mat[0][0] + this->mat[3][1] * matB.mat[1][0] + this->mat[3][2] * matB.mat[2][0] + this->mat[3][3] * matB.mat[3][0];
	result.mat[3][1] = this->mat[3][0] * matB.mat[0][1] + this->mat[3][1] * matB.mat[1][1] + this->mat[3][2] * matB.mat[2][1] + this->mat[3][3] * matB.mat[3][1];
	result.mat[3][2] = this->mat[3][0] * matB.mat[0][2] + this->mat[3][1] * matB.mat[1][2] + this->mat[3][2] * matB.mat[2][2] + this->mat[3][3] * matB.mat[3][2];
	result.mat[3][3] = this->mat[3][0] * matB.mat[0][3] + this->mat[3][1] * matB.mat[1][3] + this->mat[3][2] * matB.mat[2][3] + this->mat[3][3] * matB.mat[3][3];

	*this = result;

	return *this;
}

inline Matrix4x4& Matrix4x4::operator*=(float k)
{
	this->mat[0][0] *= k;
	this->mat[0][1] *= k;
	this->mat[0][2] *= k;
	this->mat[0][3] *= k;

	this->mat[1][0] *= k;
	this->mat[1][1] *= k;
	this->mat[1][2] *= k;
	this->mat[1][3] *= k;

	this->mat[2][0] *= k;
	this->mat[2][1] *= k;
	this->mat[2][2] *= k;
	this->mat[2][3] *= k;

	this->mat[3][0] *= k;
	this->mat[3][1] *= k;
	this->mat[3][2] *= k;
	this->mat[3][3] *= k;

	return *this;
}

inline Matrix4x4& Matrix4x4::operator/=(float k)
{
	this->mat[0][0] /= k;
	this->mat[0][1] /= k;
	this->mat[0][2] /= k;
	this->mat[0][3] /= k;

	this->mat[1][0] /= k;
	this->mat[1][1] /= k;
	this->mat[1][2] /= k;
	this->mat[1][3] /= k;

	this->mat[2][0] /= k;
	this->mat[2][1] /= k;
	this->mat[2][2] /= k;
	this->mat[2][3] /= k;

	this->mat[3][0] /= k;
	this->mat[3][1] /= k;
	this->mat[3][2] /= k;
	this->mat[3][3] /= k;

	return *this;
}

inline bool IsIdentity(const Matrix4x4& mat)
{
	return
		CompareFloats(mat.mat[0][0], 1.0f) &&
		CompareFloats(mat.mat[0][1], 0.0f) &&
		CompareFloats(mat.mat[0][2], 0.0f) &&
		CompareFloats(mat.mat[0][3], 0.0f) &&

		CompareFloats(mat.mat[1][0], 0.0f) &&
		CompareFloats(mat.mat[1][1], 1.0f) &&
		CompareFloats(mat.mat[1][2], 0.0f) &&
		CompareFloats(mat.mat[1][3], 0.0f) &&

		CompareFloats(mat.mat[2][0], 0.0f) &&
		CompareFloats(mat.mat[2][1], 0.0f) &&
		CompareFloats(mat.mat[2][2], 1.0f) &&
		CompareFloats(mat.mat[2][3], 0.0f) &&

		CompareFloats(mat.mat[3][0], 0.0f) &&
		CompareFloats(mat.mat[3][1], 0.0f) &&
		CompareFloats(mat.mat[3][2], 0.0f) &&
		CompareFloats(mat.mat[3][3], 1.0f);
}

inline Matrix4x4 Transpose(const Matrix4x4& mat)
{
	Matrix4x4 result;

	result.mat[0][0] = mat.mat[0][0];
	result.mat[0][1] = mat.mat[1][0];
	result.mat[0][2] = mat.mat[2][0];
	result.mat[0][3] = mat.mat[3][0];

	result.mat[1][0] = mat.mat[0][1];
	result.mat[1][1] = mat.mat[1][1];
	result.mat[1][2] = mat.mat[2][1];
	result.mat[1][3] = mat.mat[3][1];

	result.mat[2][0] = mat.mat[0][2];
	result.mat[2][1] = mat.mat[1][2];
	result.mat[2][2] = mat.mat[2][2];
	result.mat[2][3] = mat.mat[3][2];

	result.mat[3][0] = mat.mat[0][3];
	result.mat[3][1] = mat.mat[1][3];
	result.mat[3][2] = mat.mat[2][3];
	result.mat[3][3] = mat.mat[3][3];

	return result;
}

inline float Determinant(const Matrix4x4& mat)
{
	float c00 = mat.mat[0][0] * (mat.mat[1][1] * (mat.mat[2][2] * mat.mat[3][3] - mat.mat[2][3] * mat.mat[3][2])
		+ mat.mat[1][2] * (mat.mat[2][3] * mat.mat[3][1] - mat.mat[2][1] * mat.mat[3][3])
		+ mat.mat[1][3] * (mat.mat[2][1] * mat.mat[3][2] - mat.mat[2][2] * mat.mat[3][1]));

	float c01 = mat.mat[0][1] * (mat.mat[1][0] * (mat.mat[2][2] * mat.mat[3][3] - mat.mat[2][3] * mat.mat[3][2])
		+ mat.mat[1][2] * (mat.mat[2][3] * mat.mat[3][0] - mat.mat[2][0] * mat.mat[3][3])
		+ mat.mat[1][3] * (mat.mat[2][0] * mat.mat[3][2] - mat.mat[2][2] * mat.mat[3][0]));

	float c02 = mat.mat[0][2] * (mat.mat[1][0] * (mat.mat[2][1] * mat.mat[3][3] - mat.mat[2][3] * mat.mat[3][1])
		+ mat.mat[1][1] * (mat.mat[2][3] * mat.mat[3][0] - mat.mat[2][0] * mat.mat[3][3])
		+ mat.mat[1][3] * (mat.mat[2][0] * mat.mat[3][1] - mat.mat[2][1] * mat.mat[3][0]));

	float c03 = mat.mat[0][3] * (mat.mat[1][0] * (mat.mat[2][1] * mat.mat[3][2] - mat.mat[2][2] * mat.mat[3][1])
		+ mat.mat[1][1] * (mat.mat[2][2] * mat.mat[3][0] - mat.mat[3][0] * mat.mat[3][2])
		+ mat.mat[1][2] * (mat.mat[2][0] * mat.mat[3][1] - mat.mat[2][1] * mat.mat[3][0]));

	return c00 - c01 + c02 - c03;
}


inline Matrix4x4 Inverse(const Matrix4x4& mat)
{
	float det = Determinant(mat);
	if (CompareFloats(det, 0.0f))
		return Matrix4x4();

	Matrix4x4 cofactors;

	//0th row
	cofactors.mat[0][0] = Determinant(Matrix3x3(mat.mat[1][1], mat.mat[1][2], mat.mat[1][3],
		mat.mat[2][1], mat.mat[2][2], mat.mat[2][3],
		mat.mat[3][1], mat.mat[3][2], mat.mat[3][3]));

	cofactors.mat[0][1] = -Determinant(Matrix3x3(mat.mat[1][0], mat.mat[1][2], mat.mat[1][3],
		mat.mat[2][0], mat.mat[2][2], mat.mat[2][3],
		mat.mat[3][0], mat.mat[3][2], mat.mat[3][3]));

	cofactors.mat[0][2] = Determinant(Matrix3x3(mat.mat[1][0], mat.mat[1][1], mat.mat[1][3],
		mat.mat[2][0], mat.mat[2][1], mat.mat[2][3],
		mat.mat[3][0], mat.mat[3][1], mat.mat[3][3]));

	cofactors.mat[0][3] = -Determinant(Matrix3x3(mat.mat[1][0], mat.mat[1][1], mat.mat[1][2],
		mat.mat[2][0], mat.mat[2][1], mat.mat[2][2],
		mat.mat[3][0], mat.mat[3][1], mat.mat[3][2]));

	//1st row
	cofactors.mat[1][0] = -Determinant(Matrix3x3(mat.mat[0][1], mat.mat[0][2], mat.mat[0][3],
		mat.mat[2][1], mat.mat[2][2], mat.mat[2][3],
		mat.mat[3][1], mat.mat[3][2], mat.mat[3][3]));

	cofactors.mat[1][1] = Determinant(Matrix3x3(mat.mat[0][0], mat.mat[0][2], mat.mat[0][3],
		mat.mat[2][0], mat.mat[2][2], mat.mat[2][3],
		mat.mat[3][0], mat.mat[3][2], mat.mat[3][3]));

	cofactors.mat[1][2] = -Determinant(Matrix3x3(mat.mat[0][0], mat.mat[0][1], mat.mat[0][3],
		mat.mat[2][0], mat.mat[2][1], mat.mat[2][3],
		mat.mat[3][0], mat.mat[3][1], mat.mat[3][3]));

	cofactors.mat[1][3] = Determinant(Matrix3x3(mat.mat[0][0], mat.mat[0][1], mat.mat[0][2],
		mat.mat[2][0], mat.mat[2][1], mat.mat[2][2],
		mat.mat[3][0], mat.mat[3][1], mat.mat[3][2]));

	//2nd row
	cofactors.mat[2][0] = Determinant(Matrix3x3(mat.mat[0][1], mat.mat[0][2], mat.mat[0][3],
		mat.mat[1][1], mat.mat[1][2], mat.mat[1][3],
		mat.mat[3][1], mat.mat[3][2], mat.mat[3][3]));

	cofactors.mat[2][1] = -Determinant(Matrix3x3(mat.mat[0][0], mat.mat[0][2], mat.mat[0][3],
		mat.mat[1][0], mat.mat[1][2], mat.mat[1][3],
		mat.mat[3][0], mat.mat[3][2], mat.mat[3][3]));

	cofactors.mat[2][2] = Determinant(Matrix3x3(mat.mat[0][0], mat.mat[0][1], mat.mat[0][3],
		mat.mat[1][0], mat.mat[1][1], mat.mat[1][3],
		mat.mat[3][0], mat.mat[3][1], mat.mat[3][3]));

	cofactors.mat[2][3] = -Determinant(Matrix3x3(mat.mat[0][0], mat.mat[0][1], mat.mat[0][2],
		mat.mat[1][0], mat.mat[1][1], mat.mat[1][2],
		mat.mat[3][0], mat.mat[3][1], mat.mat[3][2]));

	//3rd row
	cofactors.mat[3][0] = -Determinant(Matrix3x3(mat.mat[0][1], mat.mat[0][2], mat.mat[0][3],
		mat.mat[1][1], mat.mat[1][2], mat.mat[1][3],
		mat.mat[2][1], mat.mat[2][2], mat.mat[2][3]));

	cofactors.mat[3][1] = Determinant(Matrix3x3(mat.mat[0][0], mat.mat[0][2], mat.mat[0][3],
		mat.mat[1][0], mat.mat[1][2], mat.mat[1][3],
		mat.mat[2][0], mat.mat[2][2], mat.mat[2][3]));

	cofactors.mat[3][2] = -Determinant(Matrix3x3(mat.mat[0][0], mat.mat[0][1], mat.mat[0][3],
		mat.mat[1][0], mat.mat[1][1], mat.mat[1][3],
		mat.mat[2][0], mat.mat[2][1], mat.mat[2][3]));

	cofactors.mat[3][3] = Determinant(Matrix3x3(mat.mat[0][0], mat.mat[0][1], mat.mat[0][2],
		mat.mat[1][0], mat.mat[1][1], mat.mat[1][2],
		mat.mat[2][0], mat.mat[2][1], mat.mat[2][2]));

	return (1.0f / det) * Transpose(cofactors);
}

inline Matrix4x4 operator+(const Matrix4x4& matA, const Matrix4x4& matB)
{
	Matrix4x4 result;

	result.mat[0][0] = matA.mat[0][0] + matB.mat[0][0];
	result.mat[0][1] = matA.mat[0][1] + matB.mat[0][1];
	result.mat[0][2] = matA.mat[0][2] + matB.mat[0][2];
	result.mat[0][3] = matA.mat[0][3] + matB.mat[0][3];

	result.mat[1][0] = matA.mat[1][0] + matB.mat[1][0];
	result.mat[1][1] = matA.mat[1][1] + matB.mat[1][1];
	result.mat[1][2] = matA.mat[1][2] + matB.mat[1][2];
	result.mat[1][3] = matA.mat[1][3] + matB.mat[1][3];

	result.mat[2][0] = matA.mat[2][0] + matB.mat[2][0];
	result.mat[2][1] = matA.mat[2][1] + matB.mat[2][1];
	result.mat[2][2] = matA.mat[2][2] + matB.mat[2][2];
	result.mat[2][3] = matA.mat[2][3] + matB.mat[2][3];

	result.mat[3][0] = matA.mat[3][0] + matB.mat[3][0];
	result.mat[3][1] = matA.mat[3][1] + matB.mat[3][1];
	result.mat[3][2] = matA.mat[3][2] + matB.mat[3][2];
	result.mat[3][3] = matA.mat[3][3] + matB.mat[3][3];

	return result;
}

inline Matrix4x4 operator-(const Matrix4x4& matA, const Matrix4x4& matB)
{
	Matrix4x4 result;

	result.mat[0][0] = matA.mat[0][0] - matB.mat[0][0];
	result.mat[0][1] = matA.mat[0][1] - matB.mat[0][1];
	result.mat[0][2] = matA.mat[0][2] - matB.mat[0][2];
	result.mat[0][3] = matA.mat[0][3] - matB.mat[0][3];

	result.mat[1][0] = matA.mat[1][0] - matB.mat[1][0];
	result.mat[1][1] = matA.mat[1][1] - matB.mat[1][1];
	result.mat[1][2] = matA.mat[1][2] - matB.mat[1][2];
	result.mat[1][3] = matA.mat[1][3] - matB.mat[1][3];

	result.mat[2][0] = matA.mat[2][0] - matB.mat[2][0];
	result.mat[2][1] = matA.mat[2][1] - matB.mat[2][1];
	result.mat[2][2] = matA.mat[2][2] - matB.mat[2][2];
	result.mat[2][3] = matA.mat[2][3] - matB.mat[2][3];

	result.mat[3][0] = matA.mat[3][0] - matB.mat[3][0];
	result.mat[3][1] = matA.mat[3][1] - matB.mat[3][1];
	result.mat[3][2] = matA.mat[3][2] - matB.mat[3][2];
	result.mat[3][3] = matA.mat[3][3] - matB.mat[3][3];

	return result;
}

inline Matrix4x4 operator*(const Matrix4x4& matA, const Matrix4x4& matB)
{
	Matrix4x4 result;

	result.mat[0][0] = matA.mat[0][0] * matB.mat[0][0] + matA.mat[0][1] * matB.mat[1][0] + matA.mat[0][2] * matB.mat[2][0] + matA.mat[0][3] * matB.mat[3][0];
	result.mat[0][1] = matA.mat[0][0] * matB.mat[0][1] + matA.mat[0][1] * matB.mat[1][1] + matA.mat[0][2] * matB.mat[2][1] + matA.mat[0][3] * matB.mat[3][1];
	result.mat[0][2] = matA.mat[0][0] * matB.mat[0][2] + matA.mat[0][1] * matB.mat[1][2] + matA.mat[0][2] * matB.mat[2][2] + matA.mat[0][3] * matB.mat[3][2];
	result.mat[0][3] = matA.mat[0][0] * matB.mat[0][3] + matA.mat[0][1] * matB.mat[1][3] + matA.mat[0][2] * matB.mat[2][3] + matA.mat[0][3] * matB.mat[3][3];

	result.mat[1][0] = matA.mat[1][0] * matB.mat[0][0] + matA.mat[1][1] * matB.mat[1][0] + matA.mat[1][2] * matB.mat[2][0] + matA.mat[1][3] * matB.mat[3][0];
	result.mat[1][1] = matA.mat[1][0] * matB.mat[0][1] + matA.mat[1][1] * matB.mat[1][1] + matA.mat[1][2] * matB.mat[2][1] + matA.mat[1][3] * matB.mat[3][1];
	result.mat[1][2] = matA.mat[1][0] * matB.mat[0][2] + matA.mat[1][1] * matB.mat[1][2] + matA.mat[1][2] * matB.mat[2][2] + matA.mat[1][3] * matB.mat[3][2];
	result.mat[1][3] = matA.mat[1][0] * matB.mat[0][3] + matA.mat[1][1] * matB.mat[1][3] + matA.mat[1][2] * matB.mat[2][3] + matA.mat[1][3] * matB.mat[3][3];

	result.mat[2][0] = matA.mat[2][0] * matB.mat[0][0] + matA.mat[2][1] * matB.mat[1][0] + matA.mat[2][2] * matB.mat[2][0] + matA.mat[2][3] * matB.mat[3][0];
	result.mat[2][1] = matA.mat[2][0] * matB.mat[0][1] + matA.mat[2][1] * matB.mat[1][1] + matA.mat[2][2] * matB.mat[2][1] + matA.mat[2][3] * matB.mat[3][1];
	result.mat[2][2] = matA.mat[2][0] * matB.mat[0][2] + matA.mat[2][1] * matB.mat[1][2] + matA.mat[2][2] * matB.mat[2][2] + matA.mat[2][3] * matB.mat[3][2];
	result.mat[2][3] = matA.mat[2][0] * matB.mat[0][3] + matA.mat[2][1] * matB.mat[1][3] + matA.mat[2][2] * matB.mat[2][3] + matA.mat[2][3] * matB.mat[3][3];

	result.mat[3][0] = matA.mat[3][0] * matB.mat[0][0] + matA.mat[3][1] * matB.mat[1][0] + matA.mat[3][2] * matB.mat[2][0] + matA.mat[3][3] * matB.mat[3][0];
	result.mat[3][1] = matA.mat[3][0] * matB.mat[0][1] + matA.mat[3][1] * matB.mat[1][1] + matA.mat[3][2] * matB.mat[2][1] + matA.mat[3][3] * matB.mat[3][1];
	result.mat[3][2] = matA.mat[3][0] * matB.mat[0][2] + matA.mat[3][1] * matB.mat[1][2] + matA.mat[3][2] * matB.mat[2][2] + matA.mat[3][3] * matB.mat[3][2];
	result.mat[3][3] = matA.mat[3][0] * matB.mat[0][3] + matA.mat[3][1] * matB.mat[1][3] + matA.mat[3][2] * matB.mat[2][3] + matA.mat[3][3] * matB.mat[3][3];

	return result;
}

inline vec4 operator*(const Matrix4x4& mat, const vec4& vec)
{
	vec4 result;

	result.SetX(mat.mat[0][0] * vec.GetX() + mat.mat[0][1] * vec.GetY() + mat.mat[0][2] * vec.GetZ() + mat.mat[0][3] * vec.GetW());
	result.SetY(mat.mat[1][0] * vec.GetX() + mat.mat[1][1] * vec.GetY() + mat.mat[1][2] * vec.GetZ() + mat.mat[1][3] * vec.GetW());
	result.SetZ(mat.mat[2][0] * vec.GetX() + mat.mat[2][1] * vec.GetY() + mat.mat[2][2] * vec.GetZ() + mat.mat[2][3] * vec.GetW());
	result.SetW(mat.mat[3][0] * vec.GetX() + mat.mat[3][1] * vec.GetY() + mat.mat[3][2] * vec.GetZ() + mat.mat[3][3] * vec.GetW());

	return result;
}

inline vec4 operator*(const vec4& vec, const Matrix4x4& mat)
{
	vec4 result;

	result.SetX(vec.GetX() * mat.mat[0][0] + vec.GetY() * mat.mat[1][0] + vec.GetZ() * mat.mat[2][0] + vec.GetW() * mat.mat[3][0]);
	result.SetY(vec.GetX() * mat.mat[0][1] + vec.GetY() * mat.mat[1][1] + vec.GetZ() * mat.mat[2][1] + vec.GetW() * mat.mat[3][1]);
	result.SetZ(vec.GetX() * mat.mat[0][2] + vec.GetY() * mat.mat[1][2] + vec.GetZ() * mat.mat[2][2] + vec.GetW() * mat.mat[3][2]);
	result.SetW(vec.GetX() * mat.mat[0][3] + vec.GetY() * mat.mat[1][3] + vec.GetZ() * mat.mat[2][3] + vec.GetW() * mat.mat[3][3]);

	return result;
}

inline Matrix4x4 operator*(const Matrix4x4& mat, float k)
{
	Matrix4x4 result;

	result.mat[0][0] = mat.mat[0][0] * k;
	result.mat[0][1] = mat.mat[0][1] * k;
	result.mat[0][2] = mat.mat[0][2] * k;
	result.mat[0][3] = mat.mat[0][3] * k;

	result.mat[1][0] = mat.mat[1][0] * k;
	result.mat[1][1] = mat.mat[1][1] * k;
	result.mat[1][2] = mat.mat[1][2] * k;
	result.mat[1][3] = mat.mat[1][3] * k;

	result.mat[2][0] = mat.mat[2][0] * k;
	result.mat[2][1] = mat.mat[2][1] * k;
	result.mat[2][2] = mat.mat[2][2] * k;
	result.mat[2][3] = mat.mat[2][3] * k;

	result.mat[3][0] = mat.mat[3][0] * k;
	result.mat[3][1] = mat.mat[3][1] * k;
	result.mat[3][2] = mat.mat[3][2] * k;
	result.mat[3][3] = mat.mat[3][3] * k;

	return result;
}

inline Matrix4x4 operator*(float k, const Matrix4x4& mat)
{
	Matrix4x4 result;

	result.mat[0][0] = k * mat.mat[0][0];
	result.mat[0][1] = k * mat.mat[0][1];
	result.mat[0][2] = k * mat.mat[0][2];
	result.mat[0][3] = k * mat.mat[0][3];

	result.mat[1][0] = k * mat.mat[1][0];
	result.mat[1][1] = k * mat.mat[1][1];
	result.mat[1][2] = k * mat.mat[1][2];
	result.mat[1][3] = k * mat.mat[1][3];

	result.mat[2][0] = k * mat.mat[2][0];
	result.mat[2][1] = k * mat.mat[2][1];
	result.mat[2][2] = k * mat.mat[2][2];
	result.mat[2][3] = k * mat.mat[2][3];

	result.mat[3][0] = k * mat.mat[3][0];
	result.mat[3][1] = k * mat.mat[3][1];
	result.mat[3][2] = k * mat.mat[3][2];
	result.mat[3][3] = k * mat.mat[3][3];

	return result;
}

inline Matrix4x4 operator/(const Matrix4x4& mat, float k)
{
	Matrix4x4 result;

	result.mat[0][0] = mat.mat[0][0] / k;
	result.mat[0][1] = mat.mat[0][1] / k;
	result.mat[0][2] = mat.mat[0][2] / k;
	result.mat[0][3] = mat.mat[0][3] / k;

	result.mat[1][0] = mat.mat[1][0] / k;
	result.mat[1][1] = mat.mat[1][1] / k;
	result.mat[1][2] = mat.mat[1][2] / k;
	result.mat[1][3] = mat.mat[1][3] / k;

	result.mat[2][0] = mat.mat[2][0] / k;
	result.mat[2][1] = mat.mat[2][1] / k;
	result.mat[2][2] = mat.mat[2][2] / k;
	result.mat[2][3] = mat.mat[2][3] / k;

	result.mat[3][0] = mat.mat[3][0] / k;
	result.mat[3][1] = mat.mat[3][1] / k;
	result.mat[3][2] = mat.mat[3][2] / k;
	result.mat[3][3] = mat.mat[3][3] / k;

	return result;
}

//------------------------------------------------------------------------------------------------------------

typedef Matrix4x4 mat4;
typedef Matrix3x3 mat3;
typedef Matrix2x2 mat2;

//QUATERNION
//------------------------------------------------------------------------------------------------------------
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

inline Quaternion::Quaternion() : x(0.0f), y(0.0f), z(0.0f), scalar(0.0f)
{}

inline Quaternion::Quaternion(float x, float y, float z, float scalar) : x(x), y(y), z(z), scalar(scalar)
{}

inline float Quaternion::GetX() const
{
	return x;
}

inline float Quaternion::GetY() const
{
	return y;
}

inline float Quaternion::GetZ() const
{
	return z;
}

inline float Quaternion::GetScalar() const
{
	return scalar;
}

inline void Quaternion::SetX(float x)
{
	this->x = x;
}

inline void Quaternion::SetY(float y)
{
	this->y = y;
}

inline void Quaternion::SetZ(float z)
{
	this->z = z;
}

inline void Quaternion::SetScalar(float scalar)
{
	this->scalar = scalar;
}

inline void Quaternion::Set(float x, float y, float z, float scalar)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->scalar = scalar;
}

inline Quaternion& Quaternion::operator+=(const Quaternion& b)
{
	this->x += b.x;
	this->y += b.y;
	this->z += b.z;
	this->scalar += b.scalar;

	return *this;
}

inline Quaternion& Quaternion::operator-=(const Quaternion& b)
{
	this->x -= b.x;
	this->y -= b.y;
	this->z -= b.z;
	this->scalar -= b.scalar;

	return *this;
}

inline Quaternion& Quaternion::operator*=(const Quaternion& b)
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

inline Quaternion& Quaternion::operator*=(float k)
{
	this->x *= k;
	this->y *= k;
	this->z *= k;
	this->scalar *= k;

	return *this;
}

inline Quaternion& Quaternion::operator/=(float k)
{
	this->x /= k;
	this->y /= k;
	this->z /= k;
	this->scalar /= k;

	return *this;
}

inline bool Quaternion::operator==(const Quaternion& b) const
{
	return CompareFloats(this->x, b.x) && CompareFloats(this->y, b.y) && CompareFloats(this->z, b.z) && CompareFloats(this->scalar, b.scalar);
}

inline bool Quaternion::operator!=(const Quaternion& b) const
{
	return !(*this == b);
}

inline Quaternion Quaternion::MakeIdentity()
{
	return Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
}

inline Quaternion Quaternion::MakeRotation(float angle, float x, float y, float z)
{
	float ang = (angle / 2.0f) * (PI / 180.0f);
	float cosAngle = cos(ang);
	float sinAngle = sin(ang);
	Vector3 axis = Normalize(Vector3(x, y, z));
	return Quaternion(sinAngle * axis.GetX(), sinAngle * axis.GetY(), sinAngle * axis.GetZ(), cosAngle);
}

inline Quaternion Quaternion::MakeRotation(float angle, const Vector3& axis)
{
	float ang = (angle / 2.0f) * (PI / 180.0f);
	float cosAngle = cos(ang);
	float sinAngle = sin(ang);
	Vector3 axisN = Normalize(axis);
	return Quaternion(sinAngle * axisN.GetX(), sinAngle * axisN.GetY(), sinAngle * axisN.GetZ(), cosAngle);
}

inline Quaternion operator+(const Quaternion& a, const Quaternion& b)
{
	return Quaternion(a.x + b.x, a.y + b.y, a.z + b.z, a.scalar + b.scalar);
}

inline Quaternion operator-(const Quaternion& a, const Quaternion& b)
{
	return Quaternion(a.x - b.x, a.y - b.y, a.z - b.z, a.scalar - b.scalar);
}

inline Quaternion operator-(const Quaternion& a)
{
	return Quaternion(-a.x, -a.y, -a.z, -a.scalar);
}

inline Quaternion operator*(const Quaternion& a, const Quaternion& b)
{
	float x = a.scalar * b.x + a.x * b.scalar + a.y * b.z - a.z * b.y;
	float y = a.scalar * b.y + a.y * b.scalar + a.z * b.x - a.x * b.z;
	float z = a.scalar * b.z + a.z * b.scalar + a.x * b.y - a.y * b.x;
	float scalar = a.scalar * b.scalar - a.x * b.x - a.y * b.y - a.z * b.z;

	return Quaternion(x, y, z, scalar);
}

inline Quaternion operator*(const Quaternion& a, float k)
{
	return Quaternion(a.x * k, a.y * k, a.z * k, a.scalar * k);
}

inline Quaternion operator*(float k, const Quaternion& a)
{
	return Quaternion(k * a.x, k * a.y, k * a.z, k * a.scalar);
}

inline Quaternion operator/(const Quaternion& a, float k)
{
	return Quaternion(a.x / k, a.y / k, a.z / k, a.scalar / k);
}

inline float DotProduct(const Quaternion& a, const Quaternion& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z * a.scalar * b.scalar;
}

inline float Length(const Quaternion& a)
{
	return sqrt(a.x * a.x + a.y * a.y + a.z * a.z + a.scalar * a.scalar);
}

inline Quaternion Normalize(const Quaternion& a)
{
	float mag = Length(a);
	assert(!CompareFloats(mag, 0.0f) && "Magnitude of the quaternion is 0.");

	return a / mag;
}

inline Quaternion Conjugate(const Quaternion& a)
{
	return Quaternion(-a.x, -a.y, -a.z, a.scalar);
}

inline Quaternion Inverse(const Quaternion& a)
{
	return Conjugate(a) / Length(a);
}

inline bool IsIdentity(const Quaternion& a)
{
	return CompareFloats(a.x, 0.0f) && CompareFloats(a.y, 0.0f) && CompareFloats(a.z, 0.0f) && CompareFloats(a.scalar, 1.0f);
}

inline Vector3 Rotate(const Quaternion& q, const Vector3& p)
{
	Quaternion point(p.GetX(), p.GetY(), p.GetZ(), 0.0f);

	Quaternion result = q * point * Conjugate(q);

	return Vector3(result.x, result.y, result.z);
}

inline Vector4 Rotate(const Quaternion& q, const Vector4& p)
{
	Quaternion point(p.GetX(), p.GetY(), p.GetZ(), 0.0f);

	Quaternion result = q * point * Conjugate(q);

	return Vector4(result.x, result.y, result.z, p.GetW());
}

inline Matrix4x4 QuaternionToMatrix(const Quaternion& q)
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
//------------------------------------------------------------------------------------------------------------

typedef Quaternion quat;