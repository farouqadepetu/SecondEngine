#pragma once

#include <nmmintrin.h> //SSE 4.2
#include <cstring>
#include <cstdint>
#include <cassert>

#include "SEMath_Utility.h"


//VECTOR2
//------------------------------------------------------------------------------------------------------------
class Vector2_Intrinsics
{
public:

	Vector2_Intrinsics();
	Vector2_Intrinsics(float x, float y);

	float GetX() const;
	float GetY() const;

	void SetX(float x);
	void SetY(float y);
	void Set(float x, float y);

	//Returns the result of this + b.
	Vector2_Intrinsics& operator+=(const Vector2_Intrinsics& b);

	//Returns the result of this - b.
	Vector2_Intrinsics& operator-=(const Vector2_Intrinsics& b);

	//Returns the result of this * b (component multiplication).
	Vector2_Intrinsics& operator*=(const Vector2_Intrinsics& b);

	//Returns the result of this / k.
	Vector2_Intrinsics& operator/=(float k);

	//Returns the result of a + b.
	friend Vector2_Intrinsics operator+(const Vector2_Intrinsics& a, const Vector2_Intrinsics& b);

	//Returns the result of a - b.
	friend Vector2_Intrinsics operator-(const Vector2_Intrinsics& a, const Vector2_Intrinsics& b);

	//Returns the result of the negation of a;
	friend Vector2_Intrinsics operator-(const Vector2_Intrinsics& a);

	//Returns the result of the component multiplication between a and b.
	friend Vector2_Intrinsics operator*(const Vector2_Intrinsics& a, const Vector2_Intrinsics& b);

	//Returns the result of a * k.
	friend Vector2_Intrinsics operator*(const Vector2_Intrinsics& a, float k);

	//Returns the result of k * a.
	friend Vector2_Intrinsics operator*(float k, const Vector2_Intrinsics& a);

	//Returns the result of a / k.
	friend Vector2_Intrinsics operator/(const Vector2_Intrinsics& a, float k);

	//Returns the result of a dot b.
	friend float DotProduct(const Vector2_Intrinsics& a, const Vector2_Intrinsics& b);

	//Returns the magntidue of a.
	friend float Length(const Vector2_Intrinsics& a);

	//Returns a unit vector by normalizing a.
	friend Vector2_Intrinsics Normalize(const Vector2_Intrinsics& a);

	//Returns the distance between a and b.
	friend float Distance(const Vector2_Intrinsics& a, const Vector2_Intrinsics& b);

private:
	Vector2_Intrinsics(__m128 vec);

public:
	union
	{
		__m128 vec;
		float vecA[2];
	};

};

inline Vector2_Intrinsics::Vector2_Intrinsics()
{
	vec = _mm_set_ps1(0.0f);
}

inline Vector2_Intrinsics::Vector2_Intrinsics(float x, float y)
{
	vec = _mm_set_ps(0.0f, 0.0f, y, x);
}

inline Vector2_Intrinsics::Vector2_Intrinsics(__m128 vec)
{
	this->vec = vec;
}

inline float Vector2_Intrinsics::GetX() const
{
	return vecA[0];
}

inline float Vector2_Intrinsics::GetY() const
{
	return vecA[1];
}

inline void Vector2_Intrinsics::SetX(float x)
{
	vecA[0] = x;
}

inline void Vector2_Intrinsics::SetY(float y)
{
	vecA[1] = y;
}

inline void Vector2_Intrinsics::Set(float x, float y)
{
	vec = _mm_set_ps(0.0f, 0.0f, y, x);
}

inline Vector2_Intrinsics& Vector2_Intrinsics::operator+=(const Vector2_Intrinsics& b)
{
	this->vec = _mm_add_ps(this->vec, b.vec);

	return *this;
}

inline Vector2_Intrinsics& Vector2_Intrinsics::operator-=(const Vector2_Intrinsics& b)
{
	this->vec = _mm_sub_ps(this->vec, b.vec);

	return *this;
}

inline Vector2_Intrinsics& Vector2_Intrinsics::operator*=(const Vector2_Intrinsics& b)
{
	this->vec = _mm_mul_ps(this->vec, b.vec);

	return *this;
}

inline Vector2_Intrinsics& Vector2_Intrinsics::operator/=(float k)
{
	this->vec = _mm_div_ps(this->vec, _mm_set_ps1(k));

	return *this;
}

inline Vector2_Intrinsics operator+(const Vector2_Intrinsics& a, const Vector2_Intrinsics& b)
{
	return Vector2_Intrinsics(_mm_add_ps(a.vec, b.vec));
}

inline Vector2_Intrinsics operator-(const Vector2_Intrinsics& a, const Vector2_Intrinsics& b)
{
	return Vector2_Intrinsics(_mm_sub_ps(a.vec, b.vec));
}

inline Vector2_Intrinsics operator-(const Vector2_Intrinsics& a)
{
	return Vector2_Intrinsics(_mm_sub_ps(_mm_set_ps1(0.0f), a.vec));
}

inline Vector2_Intrinsics operator*(const Vector2_Intrinsics& a, const Vector2_Intrinsics& b)
{
	return Vector2_Intrinsics(_mm_mul_ps(a.vec, b.vec));
}

inline Vector2_Intrinsics operator*(const Vector2_Intrinsics& a, float k)
{
	return Vector2_Intrinsics(_mm_mul_ps(a.vec, _mm_set_ps1(k)));
}

inline Vector2_Intrinsics operator*(float k, const Vector2_Intrinsics& a)
{
	return Vector2_Intrinsics(_mm_mul_ps(_mm_set_ps1(k), a.vec));
}

inline Vector2_Intrinsics operator/(const Vector2_Intrinsics& a, float k)
{
	return Vector2_Intrinsics(_mm_div_ps(a.vec, _mm_set_ps1(k)));
}

inline float DotProduct(const Vector2_Intrinsics& a, const Vector2_Intrinsics& b)
{
	return _mm_cvtss_f32(_mm_dp_ps(a.vec, b.vec, 0xff));
}

inline float Length(const Vector2_Intrinsics& a)
{
	return _mm_cvtss_f32(_mm_sqrt_ps(_mm_dp_ps(a.vec, a.vec, 0xff)));
}

inline Vector2_Intrinsics Normalize(const Vector2_Intrinsics& a)
{
	float mag = 1.0f / Length(a);
	return Vector2_Intrinsics(_mm_mul_ps(a.vec, _mm_set_ps1(mag)));
}

inline float Distance(const Vector2_Intrinsics& a, const Vector2_Intrinsics& b)
{
	return Length(b - a);
}

//------------------------------------------------------------------------------------------------------------

//VECTOR3
//------------------------------------------------------------------------------------------------------------

class Vector3_Intrinsics
{
public:

	Vector3_Intrinsics();
	Vector3_Intrinsics(float x, float y, float z);

	float GetX() const;
	float GetY() const;
	float GetZ() const;

	void SetX(float x);
	void SetY(float y);
	void SetZ(float z);
	void Set(float x, float y, float z);

	//Returns the result of this + b.
	Vector3_Intrinsics& operator+=(const Vector3_Intrinsics& b);

	//Returns the result of this - b.
	Vector3_Intrinsics& operator-=(const Vector3_Intrinsics& b);

	//Returns the result of this * b (component multiplication).
	Vector3_Intrinsics& operator*=(const Vector3_Intrinsics& b);

	//Returns the result of this / k.
	Vector3_Intrinsics& operator/=(float k);

	//Returns the result of a + b.
	friend Vector3_Intrinsics operator+(const Vector3_Intrinsics& a, const Vector3_Intrinsics& b);

	//Returns the result of a - b.
	friend Vector3_Intrinsics operator-(const Vector3_Intrinsics& a, const Vector3_Intrinsics& b);

	//Returns the result of the negation of a;
	friend Vector3_Intrinsics operator-(const Vector3_Intrinsics& a);

	//Returns the result of the component multiplication between a and b.
	friend Vector3_Intrinsics operator*(const Vector3_Intrinsics& a, const Vector3_Intrinsics& b);

	//Returns the result of a * k.
	friend Vector3_Intrinsics operator*(const Vector3_Intrinsics& a, float k);

	//Returns the result of k * a.
	friend Vector3_Intrinsics operator*(float k, const Vector3_Intrinsics& a);

	//Returns the result of a / k.
	friend Vector3_Intrinsics operator/(const Vector3_Intrinsics& a, float k);

	//Returns the result of a dot b.
	friend float DotProduct(const Vector3_Intrinsics& a, const Vector3_Intrinsics& b);

	//Returns the magntidue of a.
	friend float Length(const Vector3_Intrinsics& a);

	//Returns a unit vector by normalizing a.
	friend Vector3_Intrinsics Normalize(const Vector3_Intrinsics& a);

	//Returns the distance between a and b.
	friend float Distance(const Vector3_Intrinsics& a, const Vector3_Intrinsics& b);

	//Returns the cross product between a and b.
	friend Vector3_Intrinsics CrossProduct(const Vector3_Intrinsics& a, const Vector3_Intrinsics& b);

private:
	Vector3_Intrinsics(__m128 vec);

private:
	union
	{
		__m128 vec;
		float vecA[3];
	};
};

inline Vector3_Intrinsics::Vector3_Intrinsics()
{
	vec = _mm_set_ps1(0.0f);
}

inline Vector3_Intrinsics::Vector3_Intrinsics(float x, float y, float z)
{
	vec = _mm_set_ps(0.0f, z, y, x);
}

inline Vector3_Intrinsics::Vector3_Intrinsics(__m128 vec)
{
	this->vec = vec;
}

inline float Vector3_Intrinsics::GetX() const
{
	return vecA[0];
}

inline float Vector3_Intrinsics::GetY() const
{
	return vecA[1];
}

inline float Vector3_Intrinsics::GetZ() const
{
	return vecA[2];
}

inline void Vector3_Intrinsics::SetX(float x)
{
	vecA[0] = x;
}

inline void Vector3_Intrinsics::SetY(float y)
{
	vecA[1] = y;
}

inline void Vector3_Intrinsics::SetZ(float z)
{
	vecA[2] = z;
}

inline void Vector3_Intrinsics::Set(float x, float y, float z)
{
	vec = _mm_set_ps(0.0f, z, y, x);
}

inline Vector3_Intrinsics& Vector3_Intrinsics::operator+=(const Vector3_Intrinsics& b)
{
	this->vec = _mm_add_ps(this->vec, b.vec);

	return *this;
}

inline Vector3_Intrinsics& Vector3_Intrinsics::operator-=(const Vector3_Intrinsics& b)
{
	this->vec = _mm_sub_ps(this->vec, b.vec);

	return *this;
}

inline Vector3_Intrinsics& Vector3_Intrinsics::operator*=(const Vector3_Intrinsics& b)
{
	this->vec = _mm_mul_ps(this->vec, b.vec);

	return *this;
}

inline Vector3_Intrinsics& Vector3_Intrinsics::operator/=(float k)
{
	this->vec = _mm_div_ps(this->vec, _mm_set_ps1(k));

	return *this;
}

inline Vector3_Intrinsics operator+(const Vector3_Intrinsics& a, const Vector3_Intrinsics& b)
{
	return Vector3_Intrinsics(_mm_add_ps(a.vec, b.vec));
}

inline Vector3_Intrinsics operator-(const Vector3_Intrinsics& a, const Vector3_Intrinsics& b)
{
	return Vector3_Intrinsics(_mm_sub_ps(a.vec, b.vec));
}

inline Vector3_Intrinsics operator-(const Vector3_Intrinsics& a)
{
	return Vector3_Intrinsics(_mm_sub_ps(_mm_set_ps1(0.0f), a.vec));
}

inline Vector3_Intrinsics operator*(const Vector3_Intrinsics& a, const Vector3_Intrinsics& b)
{
	return Vector3_Intrinsics(_mm_mul_ps(a.vec, b.vec));
}

inline Vector3_Intrinsics operator*(const Vector3_Intrinsics& a, float k)
{
	return Vector3_Intrinsics(_mm_mul_ps(a.vec, _mm_set_ps1(k)));
}

inline Vector3_Intrinsics operator*(float k, const Vector3_Intrinsics& a)
{
	return Vector3_Intrinsics(_mm_mul_ps(_mm_set_ps1(k), a.vec));
}

inline Vector3_Intrinsics operator/(const Vector3_Intrinsics& a, float k)
{
	return Vector3_Intrinsics(_mm_div_ps(a.vec, _mm_set_ps1(k)));
}

inline float DotProduct(const Vector3_Intrinsics& a, const Vector3_Intrinsics& b)
{
	return _mm_cvtss_f32(_mm_dp_ps(a.vec, b.vec, 0xff));
}

inline float Length(const Vector3_Intrinsics& a)
{
	return _mm_cvtss_f32(_mm_sqrt_ps(_mm_dp_ps(a.vec, a.vec, 0xff)));
}

inline Vector3_Intrinsics Normalize(const Vector3_Intrinsics& a)
{
	float mag = 1.0f / Length(a);
	return Vector3_Intrinsics(_mm_mul_ps(a.vec, _mm_set_ps1(mag)));
}

inline float Distance(const Vector3_Intrinsics& a, const Vector3_Intrinsics& b)
{
	return Length(b - a);
}

inline Vector3_Intrinsics CrossProduct(const Vector3_Intrinsics& a, const Vector3_Intrinsics& b)
{
	return Vector3_Intrinsics(
		_mm_sub_ps(
			_mm_mul_ps(_mm_shuffle_ps(a.vec, a.vec, _MM_SHUFFLE(3, 0, 2, 1)), _mm_shuffle_ps(b.vec, b.vec, _MM_SHUFFLE(3, 1, 0, 2))),
			_mm_mul_ps(_mm_shuffle_ps(a.vec, a.vec, _MM_SHUFFLE(3, 1, 0, 2)), _mm_shuffle_ps(b.vec, b.vec, _MM_SHUFFLE(3, 0, 2, 1)))
		)
	);
}
//------------------------------------------------------------------------------------------------------------

//VECTOR4
//------------------------------------------------------------------------------------------------------------
class Vector4_Intrinsics
{
public:

	Vector4_Intrinsics();
	Vector4_Intrinsics(float x, float y, float z, float w);

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
	Vector4_Intrinsics& operator+=(const Vector4_Intrinsics& b);

	//Returns the result of this - b.
	Vector4_Intrinsics& operator-=(const Vector4_Intrinsics& b);

	//Returns the result of this * b (component multiplication).
	Vector4_Intrinsics& operator*=(const Vector4_Intrinsics& b);

	//Returns the result of this / k.
	Vector4_Intrinsics& operator/=(float k);

	//Returns the result of a + b.
	friend Vector4_Intrinsics operator+(const Vector4_Intrinsics& a, const Vector4_Intrinsics& b);

	//Returns the result of a - b.
	friend Vector4_Intrinsics operator-(const Vector4_Intrinsics& a, const Vector4_Intrinsics& b);

	//Returns the result of the negation of a;
	friend Vector4_Intrinsics operator-(const Vector4_Intrinsics& a);

	//Returns the result of the component multiplication between a and b.
	friend Vector4_Intrinsics operator*(const Vector4_Intrinsics& a, const Vector4_Intrinsics& b);

	//Returns the result of a * k.
	friend Vector4_Intrinsics operator*(const Vector4_Intrinsics& a, float k);

	//Returns the result of k * a.
	friend Vector4_Intrinsics operator*(float k, const Vector4_Intrinsics& a);

	//Returns the result of a / k.
	friend Vector4_Intrinsics operator/(const Vector4_Intrinsics& a, float k);

	//Returns the result of a dot b.
	friend float DotProduct(const Vector4_Intrinsics& a, const Vector4_Intrinsics& b);

	//Returns the magntidue of a.
	friend float Length(const Vector4_Intrinsics& a);

	//Returns a unit vector by normalizing a.
	friend Vector4_Intrinsics Normalize(const Vector4_Intrinsics& a);

	//Returns the distance between a and b.
	friend float Distance(const Vector4_Intrinsics& a, const Vector4_Intrinsics& b);

private:
	Vector4_Intrinsics(__m128 vec);

private:
	union
	{
		__m128 vec;
		float vecA[4];
	};
};

inline Vector4_Intrinsics::Vector4_Intrinsics()
{
	vec = _mm_set_ps1(0.0f);
}

inline Vector4_Intrinsics::Vector4_Intrinsics(float x, float y, float z, float w)
{
	vec = _mm_set_ps(w, z, y, x);
}

inline Vector4_Intrinsics::Vector4_Intrinsics(__m128 vec)
{
	this->vec = vec;
}

inline float Vector4_Intrinsics::GetX() const
{
	return vecA[0];
}

inline float Vector4_Intrinsics::GetY() const
{
	return vecA[1];
}

inline float Vector4_Intrinsics::GetZ() const
{
	return vecA[2];
}

inline float Vector4_Intrinsics::GetW() const
{
	return vecA[3];
}

inline void Vector4_Intrinsics::SetX(float x)
{
	vecA[0] = x;
}

inline void Vector4_Intrinsics::SetY(float y)
{
	vecA[1] = y;
}

inline void Vector4_Intrinsics::SetZ(float z)
{
	vecA[2] = z;
}

inline void Vector4_Intrinsics::SetW(float w)
{
	vecA[3] = w;
}

inline void Vector4_Intrinsics::Set(float x, float y, float z, float w)
{
	vec = _mm_set_ps(w, z, y, x);
}

inline Vector4_Intrinsics& Vector4_Intrinsics::operator+=(const Vector4_Intrinsics& b)
{
	this->vec = _mm_add_ps(this->vec, b.vec);

	return *this;
}

inline Vector4_Intrinsics& Vector4_Intrinsics::operator-=(const Vector4_Intrinsics& b)
{
	this->vec = _mm_sub_ps(this->vec, b.vec);

	return *this;
}

inline Vector4_Intrinsics& Vector4_Intrinsics::operator*=(const Vector4_Intrinsics& b)
{
	this->vec = _mm_mul_ps(this->vec, b.vec);

	return *this;
}

inline Vector4_Intrinsics& Vector4_Intrinsics::operator/=(float k)
{
	this->vec = _mm_div_ps(this->vec, _mm_set_ps1(k));

	return *this;
}

inline Vector4_Intrinsics operator+(const Vector4_Intrinsics& a, const Vector4_Intrinsics& b)
{
	return Vector4_Intrinsics(_mm_add_ps(a.vec, b.vec));
}

inline Vector4_Intrinsics operator-(const Vector4_Intrinsics& a, const Vector4_Intrinsics& b)
{
	return Vector4_Intrinsics(_mm_sub_ps(a.vec, b.vec));
}

inline Vector4_Intrinsics operator-(const Vector4_Intrinsics& a)
{
	return Vector4_Intrinsics(_mm_xor_ps(a.vec, _mm_set_ps1(-0.0f)));
}

inline Vector4_Intrinsics operator*(const Vector4_Intrinsics& a, const Vector4_Intrinsics& b)
{
	return Vector4_Intrinsics(_mm_mul_ps(a.vec, b.vec));
}

inline Vector4_Intrinsics operator*(const Vector4_Intrinsics& a, float k)
{
	return Vector4_Intrinsics(_mm_mul_ps(a.vec, _mm_set_ps1(k)));
}

inline Vector4_Intrinsics operator*(float k, const Vector4_Intrinsics& a)
{
	return Vector4_Intrinsics(_mm_mul_ps(_mm_set_ps1(k), a.vec));
}

inline Vector4_Intrinsics operator/(const Vector4_Intrinsics& a, float k)
{
	return Vector4_Intrinsics(_mm_div_ps(a.vec, _mm_set_ps1(k)));
}

inline float DotProduct(const Vector4_Intrinsics& a, const Vector4_Intrinsics& b)
{
	return _mm_cvtss_f32(_mm_dp_ps(a.vec, b.vec, 0xff));
}

inline float Length(const Vector4_Intrinsics& a)
{
	return _mm_cvtss_f32(_mm_sqrt_ps(_mm_dp_ps(a.vec, a.vec, 0xff)));
}

inline Vector4_Intrinsics Normalize(const Vector4_Intrinsics& a)
{
	float mag = 1.0f / Length(a);
	return Vector4_Intrinsics(_mm_mul_ps(a.vec, _mm_set_ps1(mag)));
}

inline float Distance(const Vector4_Intrinsics& a, const Vector4_Intrinsics& b)
{
	return Length(b - a);
}
//------------------------------------------------------------------------------------------------------------

typedef Vector4_Intrinsics vec4;
typedef Vector3_Intrinsics vec3;
typedef Vector2_Intrinsics vec2;

//MATRIX2X2
//------------------------------------------------------------------------------------------------------------

class Matrix2x2_Intrinsics
{
public:

	//Creates an identity matrix
	Matrix2x2_Intrinsics();

	//Creates a matrix with the specified rows.
	Matrix2x2_Intrinsics(vec2 row0, vec2 row1);

	//Creates a matrix with the specified values.
	Matrix2x2_Intrinsics(float m00, float m01,
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
	static Matrix2x2_Intrinsics MakeIdentity();

	//Returns a scale matrix with the specified values.
	static Matrix2x2_Intrinsics Scale(float x, float y);

	//Returns a rotation matrix about the z-axis with the specified angle.
	//Angle should be given in degrees.
	static Matrix2x2_Intrinsics RotZ(float angle);

	//Returns the result of this + matB.
	Matrix2x2_Intrinsics& operator+=(const Matrix2x2_Intrinsics& matB);

	//Returns the result of this - matB.
	Matrix2x2_Intrinsics& operator-=(const Matrix2x2_Intrinsics& matB);

	//Returns the result of this * matB (matrix-matrix multiplication).
	Matrix2x2_Intrinsics& operator*=(const Matrix2x2_Intrinsics& matB);

	//Returns the result of this * k.
	Matrix2x2_Intrinsics& operator*=(float k);

	//Returns the result of this / k.
	Matrix2x2_Intrinsics& operator/=(float k);

	//Returns true if the matrix is an identity matrix, false otherwise.
	friend bool IsIdentity(const Matrix2x2_Intrinsics& mat);

	//Returns the transposition of the matrix.
	friend Matrix2x2_Intrinsics Transpose(const Matrix2x2_Intrinsics& mat);

	//Returns the determinant of the matrix.
	friend float Determinant(const Matrix2x2_Intrinsics& mat);

	//Returns the inverse of the matrix.
	//Returns the identity matrix if mat is noninvertible (singular)
	friend Matrix2x2_Intrinsics Inverse(const Matrix2x2_Intrinsics& mat);

	//Returns the result of matA + matB.
	friend Matrix2x2_Intrinsics operator+(const Matrix2x2_Intrinsics& matA, const Matrix2x2_Intrinsics& matB);

	//Returns the result of matA + matB.
	friend Matrix2x2_Intrinsics operator-(const Matrix2x2_Intrinsics& matA, const Matrix2x2_Intrinsics& matB);

	//Returns the result of matA * matB (matrix-matrix multiplication).
	friend Matrix2x2_Intrinsics operator*(const Matrix2x2_Intrinsics& matA, const Matrix2x2_Intrinsics& matB);

	//Returns the result of mat * vec (matrix-vector multiplication).
	friend vec2 operator*(const Matrix2x2_Intrinsics& mat, const vec2& vec);

	//Returns the result of mat * vec (vector-matrix multiplication).
	friend vec2 operator*(const vec2& vec, const Matrix2x2_Intrinsics& mat);

	//Returns the result of mat * k
	friend Matrix2x2_Intrinsics operator*(const Matrix2x2_Intrinsics& mat, float k);

	//Returns the result of k * mat
	friend Matrix2x2_Intrinsics operator*(float k, const Matrix2x2_Intrinsics& mat);

	//Returns the result of mat / k
	friend Matrix2x2_Intrinsics operator/(const Matrix2x2_Intrinsics& mat, float k);


private:
	union
	{
		__m128 mat[2];
		float matA[2][4];
	};
};

inline Matrix2x2_Intrinsics::Matrix2x2_Intrinsics()
{
	mat[0] = _mm_set_ps1(0.0f);
	mat[1] = _mm_set_ps1(0.0f);
}

inline Matrix2x2_Intrinsics::Matrix2x2_Intrinsics(vec2 row0, vec2 row1)
{
	SetRow(0, row0);
	SetRow(1, row1);
}

inline Matrix2x2_Intrinsics::Matrix2x2_Intrinsics(float m00, float m01,
	float m10, float m11)
{
	mat[0] = _mm_set_ps(0.0f, 0.0f, m01, m00);
	mat[1] = _mm_set_ps(0.0f, 0.0f, m11, m10);
}

inline float Matrix2x2_Intrinsics::GetElement(uint32_t row, uint32_t col) const
{
	assert((row < 2 && col < 2) && "Row or Col is greater than 1");

	return matA[row][col];
}

inline vec2 Matrix2x2_Intrinsics::GetRow(uint32_t row) const
{
	assert(row < 2 && "Row is greater than 1");

	return vec2(matA[row][0], matA[row][1]);
}

inline vec2 Matrix2x2_Intrinsics::GetCol(uint32_t col) const
{
	assert(col < 2 && "Col is greater than 1");

	return vec2(matA[0][col], matA[1][col]);

}

inline void Matrix2x2_Intrinsics::SetElement(uint32_t row, uint32_t col, float value)
{
	assert((row < 2 && col < 2) && "Row or Col is greater than 1");

	matA[row][col] = value;
}

inline void Matrix2x2_Intrinsics::SetRow(uint32_t row, vec2 values)
{
	assert(row < 2 && "Row is greater than 1");
	mat[row] = _mm_set_ps(0.0f, 0.0f, values.GetY(), values.GetX());
}

inline void Matrix2x2_Intrinsics::SetRow(uint32_t row, float val0, float val1)
{
	assert(row < 2 && "Row is greater than 1");
	mat[row] = _mm_set_ps(0.0f, 0.0f, val1, val0);
}

inline void Matrix2x2_Intrinsics::SetCol(uint32_t col, float val0, float val1)
{
	assert(col < 2 && "Col is greater than 1");

	matA[0][col] = val0;
	matA[1][col] = val1;
}

inline void Matrix2x2_Intrinsics::SetCol(uint32_t col, vec2 values)
{
	assert(col < 2 && "Col is greater than 1");

	matA[0][col] = values.GetX();
	matA[1][col] = values.GetY();
}

inline Matrix2x2_Intrinsics Matrix2x2_Intrinsics::MakeIdentity()
{
	return Matrix2x2_Intrinsics(1.0f, 0.0f,
		0.0f, 1.0f);
}

inline Matrix2x2_Intrinsics Matrix2x2_Intrinsics::Scale(float x, float y)
{
	return Matrix2x2_Intrinsics(x, 0.0f,
		0.0f, y);
}

inline Matrix2x2_Intrinsics Matrix2x2_Intrinsics::RotZ(float angle)
{
	float ang = angle * (PI / 180.0f);
	float cosAngle = cos(ang);
	float sinAngle = sin(ang);

	return Matrix2x2_Intrinsics(cosAngle, sinAngle,
		-sinAngle, cosAngle);
}

inline Matrix2x2_Intrinsics& Matrix2x2_Intrinsics::operator+=(const Matrix2x2_Intrinsics& matB)
{
	this->mat[0] = _mm_add_ps(this->mat[0], matB.mat[0]);
	this->mat[1] = _mm_add_ps(this->mat[1], matB.mat[1]);
	return *this;
}

inline Matrix2x2_Intrinsics& Matrix2x2_Intrinsics::operator-=(const Matrix2x2_Intrinsics& matB)
{
	this->mat[0] = _mm_sub_ps(this->mat[0], matB.mat[0]);
	this->mat[1] = _mm_sub_ps(this->mat[1], matB.mat[1]);
	return *this;
}

inline Matrix2x2_Intrinsics& Matrix2x2_Intrinsics::operator*=(const Matrix2x2_Intrinsics& matB)
{
	__m128 b0 = matB.mat[0];
	__m128 b1 = matB.mat[1];
	__m128 b2 = _mm_set_ps1(0.0f);
	__m128 b3 = _mm_set_ps1(0.0f);

	_MM_TRANSPOSE4_PS(b0, b1, b2, b3);

	this->mat[0] = _mm_set_ps(
		_mm_cvtss_f32(_mm_dp_ps(this->mat[0], b3, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(this->mat[0], b2, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(this->mat[0], b1, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(this->mat[0], b0, 0xf1))
	);

	this->mat[1] = _mm_set_ps(
		_mm_cvtss_f32(_mm_dp_ps(this->mat[1], b3, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(this->mat[1], b2, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(this->mat[1], b1, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(this->mat[1], b0, 0xf1))
	);

	return *this;
}

inline Matrix2x2_Intrinsics& Matrix2x2_Intrinsics::operator*=(float k)
{
	this->mat[0] = _mm_mul_ps(this->mat[0], _mm_set_ps1(k));
	this->mat[1] = _mm_mul_ps(this->mat[1], _mm_set_ps1(k));

	return *this;
}

inline Matrix2x2_Intrinsics& Matrix2x2_Intrinsics::operator/=(float k)
{
	this->mat[0] = _mm_div_ps(this->mat[0], _mm_set_ps1(k));
	this->mat[1] = _mm_div_ps(this->mat[1], _mm_set_ps1(k));

	return *this;
}


inline bool IsIdentity(const Matrix2x2_Intrinsics& mat)
{
	return
		CompareFloats(mat.matA[0][0], 1.0f) &&
		CompareFloats(mat.matA[0][1], 0.0f) &&

		CompareFloats(mat.matA[1][0], 0.0f) &&
		CompareFloats(mat.matA[1][1], 1.0f);
}

inline Matrix2x2_Intrinsics Transpose(const Matrix2x2_Intrinsics& mat)
{
	Matrix2x2_Intrinsics result;

	memcpy(&result, &mat, sizeof(Matrix2x2_Intrinsics));
	_MM_TRANSPOSE4_PS(result.mat[0], result.mat[1], _mm_set_ps1(0.0f), _mm_set_ps1(0.0f));

	return result;
}

inline float Determinant(const Matrix2x2_Intrinsics& mat)
{
	__m128 r0 = mat.mat[0];
	__m128 r1 = mat.mat[1];

	//Right most value is the first value
	//Ex.  r0 = 0 0 2 1
	//     r1 = 0 0 4 3

   //Swap r1 values
   //Ex. r0 = 0 0 2 1
   //    r1 = 0 0 3 4
	r1 = _mm_shuffle_ps(r1, r1, _MM_SHUFFLE(3, 2, 0, 1));

	//Multiple r0 and r1 and store result in r0
	//Ex.  r0 = 0 0 6 4
	//     r1 = 0 0 3 4
	r0 = _mm_mul_ps(r0, r1);

	//Horizontal subtract r0
	//Ex. Result = 0 -2 0 -2
	//Answer is the right most value
	return _mm_cvtss_f32(_mm_hsub_ps(r0, r0));
}

inline Matrix2x2_Intrinsics Inverse(const Matrix2x2_Intrinsics& mat)
{
	float det = Determinant(mat);
	if (CompareFloats(det, 0.0f))
		return Matrix2x2_Intrinsics();

	Matrix2x2_Intrinsics cofactors;

	cofactors.mat[0] = _mm_shuffle_ps(mat.mat[1], mat.mat[1], _MM_SHUFFLE(3, 2, 0, 1));
	cofactors.mat[1] = _mm_shuffle_ps(mat.mat[0], mat.mat[0], _MM_SHUFFLE(3, 2, 0, 1));

	cofactors.mat[0] = _mm_mul_ps(cofactors.mat[0], _mm_set_ps(0.0f, 0.0f, -1.0f, 1.0f));
	cofactors.mat[1] = _mm_mul_ps(cofactors.mat[1], _mm_set_ps(0.0f, 0.0f, 1.0f, -1.0f));

	return (1.0f / det) * Transpose(cofactors);
}

inline Matrix2x2_Intrinsics operator+(const Matrix2x2_Intrinsics& matA, const Matrix2x2_Intrinsics& matB)
{
	Matrix2x2_Intrinsics result;

	result.mat[0] = _mm_add_ps(matA.mat[0], matB.mat[0]);
	result.mat[1] = _mm_add_ps(matA.mat[1], matB.mat[1]);

	return result;
}

inline Matrix2x2_Intrinsics operator-(const Matrix2x2_Intrinsics& matA, const Matrix2x2_Intrinsics& matB)
{
	Matrix2x2_Intrinsics result;

	result.mat[0] = _mm_sub_ps(matA.mat[0], matB.mat[0]);
	result.mat[1] = _mm_sub_ps(matA.mat[1], matB.mat[1]);

	return result;
}

inline Matrix2x2_Intrinsics operator*(const Matrix2x2_Intrinsics& matA, const Matrix2x2_Intrinsics& matB)
{
	Matrix2x2_Intrinsics result;

	__m128 b0 = matB.mat[0];
	__m128 b1 = matB.mat[1];
	__m128 b2 = _mm_set_ps1(0.0f);
	__m128 b3 = _mm_set_ps1(0.0f);

	_MM_TRANSPOSE4_PS(b0, b1, b2, b3);

	result.mat[0] = _mm_set_ps(
		_mm_cvtss_f32(_mm_dp_ps(matA.mat[0], b3, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(matA.mat[0], b2, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(matA.mat[0], b1, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(matA.mat[0], b0, 0xf1))
	);

	result.mat[1] = _mm_set_ps(
		_mm_cvtss_f32(_mm_dp_ps(matA.mat[1], b3, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(matA.mat[1], b2, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(matA.mat[1], b1, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(matA.mat[1], b0, 0xf1))
	);

	return result;
}

inline vec2 operator*(const Matrix2x2_Intrinsics& mat, const vec2& vec)
{
	vec2 result;

	__m128 tempVec = _mm_set_ps(0.0f, 0.0f, vec.GetY(), vec.GetX());

	result.SetX(_mm_cvtss_f32(_mm_dp_ps(mat.mat[0], tempVec, 0xf1)));
	result.SetY(_mm_cvtss_f32(_mm_dp_ps(mat.mat[1], tempVec, 0xf1)));

	return result;
}

inline vec2 operator*(const vec2& vec, const Matrix2x2_Intrinsics& mat)
{
	vec2 result;

	__m128 tempMat0 = mat.mat[0];
	__m128 tempMat1 = mat.mat[1];
	__m128 tempMat2 = _mm_set_ps1(0.0f);
	__m128 tempMat3 = _mm_set_ps1(0.0f);

	_MM_TRANSPOSE4_PS(tempMat0, tempMat1, tempMat2, tempMat3);

	__m128 tempVec = _mm_set_ps(0.0f, 0.0f, vec.GetY(), vec.GetX());

	result.SetX(_mm_cvtss_f32(_mm_dp_ps(tempMat0, tempVec, 0xf1)));
	result.SetY(_mm_cvtss_f32(_mm_dp_ps(tempMat1, tempVec, 0xf1)));

	return result;
}

inline Matrix2x2_Intrinsics operator*(const Matrix2x2_Intrinsics& mat, float k)
{
	Matrix2x2_Intrinsics result;

	__m128 scalar = _mm_set_ps1(k);
	result.mat[0] = _mm_mul_ps(mat.mat[0], scalar);
	result.mat[1] = _mm_mul_ps(mat.mat[1], scalar);

	return result;
}

inline Matrix2x2_Intrinsics operator*(float k, const Matrix2x2_Intrinsics& mat)
{
	Matrix2x2_Intrinsics result;

	__m128 scalar = _mm_set_ps1(k);
	result.mat[0] = _mm_mul_ps(scalar, mat.mat[0]);
	result.mat[1] = _mm_mul_ps(scalar, mat.mat[1]);

	return result;
}

inline Matrix2x2_Intrinsics operator/(const Matrix2x2_Intrinsics& mat, float k)
{
	Matrix2x2_Intrinsics result;

	__m128 scalar = _mm_set_ps1(k);
	result.mat[0] = _mm_div_ps(mat.mat[0], scalar);
	result.mat[1] = _mm_div_ps(mat.mat[1], scalar);

	return result;
}
//------------------------------------------------------------------------------------------------------------

//MATRIX3X3
//------------------------------------------------------------------------------------------------------------
class Matrix3x3_Intrinsics
{
public:

	//Creates an identity matrix
	Matrix3x3_Intrinsics();

	//Creates a matrix with the specified rows.
	Matrix3x3_Intrinsics(vec3 row0, vec3 row1, vec3 row2);

	//Creates a matrix with the specified values.
	Matrix3x3_Intrinsics(float m00, float m01, float m02,
		float m10, float m11, float m12,
		float m20, float m21, float m22);

	//Returns the element at the specified [row][col]
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
	static Matrix3x3_Intrinsics MakeIdentity();

	//Returns a scale matrix with the specified values.
	static Matrix3x3_Intrinsics Scale(float x, float y, float z);

	//Returns a translation matrix with the specified.
	static Matrix3x3_Intrinsics Translate(float x, float y);

	//Returns a rotation matrix about the x-axis with the specified angle.
	//Angle should be given in degrees.
	static Matrix3x3_Intrinsics RotX(float angle);

	//Returns a rotation matrix about the y-axis with the specified angle.
	//Angle should be given in degrees.
	static Matrix3x3_Intrinsics RotY(float angle);

	//Returns a rotation matrix about the z-axis with the specified angle.
	//Angle should be given in degrees.
	static Matrix3x3_Intrinsics RotZ(float angle);

	//Returns the result of this + matB.
	Matrix3x3_Intrinsics& operator+=(const Matrix3x3_Intrinsics& matB);

	//Returns the result of this - matB.
	Matrix3x3_Intrinsics& operator-=(const Matrix3x3_Intrinsics& matB);

	//Returns the result of this * matB (matrix-matrix multiplication).
	Matrix3x3_Intrinsics& operator*=(const Matrix3x3_Intrinsics& matB);

	//Returns the result of this * k.
	Matrix3x3_Intrinsics& operator*=(float k);

	//Returns the result of this / k.
	Matrix3x3_Intrinsics& operator/=(float k);

	//Returns true if the matrix is an identity matrix, false otherwise.
	friend bool IsIdentity(const Matrix3x3_Intrinsics& mat);

	//Returns the transposition of the matrix.
	friend Matrix3x3_Intrinsics Transpose(const Matrix3x3_Intrinsics& mat);

	//Returns the determinant of the matrix.
	friend float Determinant(const Matrix3x3_Intrinsics& mat);

	//Returns the inverse of the matrix.
	//Returns the identity matrix if mat is noninvertible (singular)
	friend Matrix3x3_Intrinsics Inverse(const Matrix3x3_Intrinsics& mat);

	//Returns the result of matA + matB.
	friend Matrix3x3_Intrinsics operator+(const Matrix3x3_Intrinsics& matA, const Matrix3x3_Intrinsics& matB);

	//Returns the result of matA + matB.
	friend Matrix3x3_Intrinsics operator-(const Matrix3x3_Intrinsics& matA, const Matrix3x3_Intrinsics& matB);

	//Returns the result of matA * matB (matrix-matrix multiplication).
	friend Matrix3x3_Intrinsics operator*(const Matrix3x3_Intrinsics& matA, const Matrix3x3_Intrinsics& matB);

	//Returns the result of mat * vec (matrix-vector multiplication).
	friend vec3 operator*(const Matrix3x3_Intrinsics& mat, const vec3& vec);

	//Returns the result of mat * vec (vector-matrix multiplication).
	friend vec3 operator*(const vec3& vec, const Matrix3x3_Intrinsics& mat);

	//Returns the result of mat * k
	friend Matrix3x3_Intrinsics operator*(const Matrix3x3_Intrinsics& mat, float k);

	//Returns the result of k * mat
	friend Matrix3x3_Intrinsics operator*(float k, const Matrix3x3_Intrinsics& mat);

	//Returns the result of mat / k
	friend Matrix3x3_Intrinsics operator/(const Matrix3x3_Intrinsics& mat, float k);


private:
	union
	{
		__m128 mat[3];
		float matA[3][4];
	};
};

inline Matrix3x3_Intrinsics::Matrix3x3_Intrinsics()
{
	mat[0] = _mm_set_ps1(0.0f);
	mat[1] = _mm_set_ps1(0.0f);
	mat[2] = _mm_set_ps1(0.0f);
}

inline Matrix3x3_Intrinsics::Matrix3x3_Intrinsics(vec3 row0, vec3 row1, vec3 row2)
{
	SetRow(0, row0);
	SetRow(1, row1);
	SetRow(2, row2);
}

inline Matrix3x3_Intrinsics::Matrix3x3_Intrinsics(float m00, float m01, float m02,
	float m10, float m11, float m12,
	float m20, float m21, float m22)
{
	mat[0] = _mm_set_ps(0.0f, m02, m01, m00);
	mat[1] = _mm_set_ps(0.0f, m12, m11, m10);
	mat[2] = _mm_set_ps(0.0f, m22, m21, m20);
}

inline float Matrix3x3_Intrinsics::GetElement(uint32_t row, uint32_t col) const
{
	assert((row < 3 && col < 3) && "Row or Col is greater than 2");

	return matA[row][col];
}

inline vec3 Matrix3x3_Intrinsics::GetRow(uint32_t row) const
{
	assert(row < 3 && "Row is greater than 2");

	return vec3(matA[row][0], matA[row][1], matA[row][2]);
}

inline vec3 Matrix3x3_Intrinsics::GetCol(uint32_t col) const
{
	assert(col < 3 && "Col is greater than 2");

	return vec3(matA[0][col], matA[1][col], matA[2][col]);

}

inline void Matrix3x3_Intrinsics::SetElement(uint32_t row, uint32_t col, float value)
{
	assert((row < 3 && col < 3) && "Row or Col is greater than 2");

	matA[row][col] = value;
}

inline void Matrix3x3_Intrinsics::SetRow(uint32_t row, vec3 values)
{
	assert(row < 3 && "Row is greater than 2");
	mat[row] = _mm_set_ps(0.0f, values.GetZ(), values.GetY(), values.GetX());
}

inline void Matrix3x3_Intrinsics::SetRow(uint32_t row, float val0, float val1, float val2)
{
	assert(row < 3 && "Row is greater than 2");
	mat[row] = _mm_set_ps(0.0f, val2, val1, val0);
}

inline void Matrix3x3_Intrinsics::SetCol(uint32_t col, float val0, float val1, float val2)
{
	assert(col < 3 && "Col is greater than 2");

	matA[0][col] = val0;
	matA[1][col] = val1;
	matA[2][col] = val2;
}

inline void Matrix3x3_Intrinsics::SetCol(uint32_t col, vec3 values)
{
	assert(col < 3 && "Col is greater than 2");

	matA[0][col] = values.GetX();
	matA[1][col] = values.GetY();
	matA[2][col] = values.GetZ();
}

inline Matrix3x3_Intrinsics Matrix3x3_Intrinsics::MakeIdentity()
{
	return Matrix3x3_Intrinsics(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f);
}

inline Matrix3x3_Intrinsics Matrix3x3_Intrinsics::Scale(float x, float y, float z)
{
	return Matrix3x3_Intrinsics(x, 0.0f, 0.0f,
		0.0f, y, 0.0f,
		0.0f, 0.0f, z);
}

inline Matrix3x3_Intrinsics Matrix3x3_Intrinsics::Translate(float x, float y)
{
	return Matrix3x3_Intrinsics(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		x, y, 1.0f);
}

inline Matrix3x3_Intrinsics Matrix3x3_Intrinsics::RotX(float angle)
{
	float ang = angle * (PI / 180.0f);
	float cosAngle = cos(ang);
	float sinAngle = sin(ang);

	return Matrix3x3_Intrinsics(1.0f, 0.0f, 0.0f,
		0.0f, cosAngle, sinAngle,
		0.0f, -sinAngle, cosAngle);
}

inline Matrix3x3_Intrinsics Matrix3x3_Intrinsics::RotY(float angle)
{
	float ang = angle * (PI / 180.0f);
	float cosAngle = cos(ang);
	float sinAngle = sin(ang);

	return Matrix3x3_Intrinsics(cosAngle, 0.0f, -sinAngle,
		0.0f, 1.0f, 0.0f,
		sinAngle, 0.0f, cosAngle);
}

inline Matrix3x3_Intrinsics Matrix3x3_Intrinsics::RotZ(float angle)
{
	float ang = angle * (PI / 180.0f);
	float cosAngle = cos(ang);
	float sinAngle = sin(ang);

	return Matrix3x3_Intrinsics(cosAngle, sinAngle, 0.0f,
		-sinAngle, cosAngle, 0.0f,
		0.0f, 0.0f, 1.0f);
}

inline Matrix3x3_Intrinsics& Matrix3x3_Intrinsics::operator+=(const Matrix3x3_Intrinsics& matB)
{
	this->mat[0] = _mm_add_ps(this->mat[0], matB.mat[0]);
	this->mat[1] = _mm_add_ps(this->mat[1], matB.mat[1]);
	this->mat[2] = _mm_add_ps(this->mat[2], matB.mat[2]);
	return *this;
}

inline Matrix3x3_Intrinsics& Matrix3x3_Intrinsics::operator-=(const Matrix3x3_Intrinsics& matB)
{
	this->mat[0] = _mm_sub_ps(this->mat[0], matB.mat[0]);
	this->mat[1] = _mm_sub_ps(this->mat[1], matB.mat[1]);
	this->mat[2] = _mm_sub_ps(this->mat[2], matB.mat[2]);
	return *this;
}

inline Matrix3x3_Intrinsics& Matrix3x3_Intrinsics::operator*=(const Matrix3x3_Intrinsics& matB)
{
	__m128 b0 = matB.mat[0];
	__m128 b1 = matB.mat[1];
	__m128 b2 = matB.mat[2];
	__m128 b3 = _mm_set_ps1(0.0f);

	_MM_TRANSPOSE4_PS(b0, b1, b2, b3);

	this->mat[0] = _mm_set_ps(
		_mm_cvtss_f32(_mm_dp_ps(this->mat[0], b3, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(this->mat[0], b2, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(this->mat[0], b1, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(this->mat[0], b0, 0xf1))
	);

	this->mat[1] = _mm_set_ps(
		_mm_cvtss_f32(_mm_dp_ps(this->mat[1], b3, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(this->mat[1], b2, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(this->mat[1], b1, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(this->mat[1], b0, 0xf1))
	);


	this->mat[2] = _mm_set_ps(
		_mm_cvtss_f32(_mm_dp_ps(this->mat[2], b3, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(this->mat[2], b2, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(this->mat[2], b1, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(this->mat[2], b0, 0xf1))
	);

	return *this;
}

inline Matrix3x3_Intrinsics& Matrix3x3_Intrinsics::operator*=(float k)
{
	this->mat[0] = _mm_mul_ps(this->mat[0], _mm_set_ps1(k));
	this->mat[1] = _mm_mul_ps(this->mat[1], _mm_set_ps1(k));
	this->mat[2] = _mm_mul_ps(this->mat[2], _mm_set_ps1(k));

	return *this;
}

inline Matrix3x3_Intrinsics& Matrix3x3_Intrinsics::operator/=(float k)
{
	this->mat[0] = _mm_div_ps(this->mat[0], _mm_set_ps1(k));
	this->mat[1] = _mm_div_ps(this->mat[1], _mm_set_ps1(k));
	this->mat[2] = _mm_div_ps(this->mat[2], _mm_set_ps1(k));

	return *this;
}


inline bool IsIdentity(const Matrix3x3_Intrinsics& mat)
{
	return
		CompareFloats(mat.matA[0][0], 1.0f) &&
		CompareFloats(mat.matA[0][1], 0.0f) &&
		CompareFloats(mat.matA[0][2], 0.0f) &&

		CompareFloats(mat.matA[1][0], 0.0f) &&
		CompareFloats(mat.matA[1][1], 1.0f) &&
		CompareFloats(mat.matA[1][2], 0.0f) &&

		CompareFloats(mat.matA[2][0], 0.0f) &&
		CompareFloats(mat.matA[2][1], 0.0f) &&
		CompareFloats(mat.matA[2][2], 1.0f);
}

inline Matrix3x3_Intrinsics Transpose(const Matrix3x3_Intrinsics& mat)
{
	Matrix3x3_Intrinsics result;

	memcpy(&result, &mat, sizeof(Matrix3x3_Intrinsics));
	_MM_TRANSPOSE4_PS(result.mat[0], result.mat[1], result.mat[2], _mm_set_ps1(0.0f));

	return result;
}

inline float Determinant(const Matrix3x3_Intrinsics& mat)
{
	float t0 = mat.matA[0][0] * Determinant(Matrix2x2_Intrinsics(mat.matA[1][1], mat.matA[1][2], mat.matA[2][1], mat.matA[2][2]));
	float t1 = -mat.matA[0][1] * Determinant(Matrix2x2_Intrinsics(mat.matA[1][0], mat.matA[1][2], mat.matA[2][0], mat.matA[2][2]));
	float t2 = mat.matA[0][2] * Determinant(Matrix2x2_Intrinsics(mat.matA[1][0], mat.matA[1][1], mat.matA[2][0], mat.matA[2][1]));

	return t0 + t1 + t2;
}

inline Matrix3x3_Intrinsics Inverse(const Matrix3x3_Intrinsics& mat)
{
	float det = Determinant(mat);
	if (CompareFloats(det, 0.0f))
		return Matrix3x3_Intrinsics();

	Matrix3x3_Intrinsics cofactors;

	cofactors.matA[0][0] = Determinant(Matrix2x2_Intrinsics(mat.matA[1][1], mat.matA[1][2], mat.matA[2][1], mat.matA[2][2]));
	cofactors.matA[0][1] = -Determinant(Matrix2x2_Intrinsics(mat.matA[1][0], mat.matA[1][2], mat.matA[2][0], mat.matA[2][2]));
	cofactors.matA[0][2] = Determinant(Matrix2x2_Intrinsics(mat.matA[1][0], mat.matA[1][1], mat.matA[2][0], mat.matA[2][1]));

	cofactors.matA[1][0] = -Determinant(Matrix2x2_Intrinsics(mat.matA[0][1], mat.matA[0][2], mat.matA[2][1], mat.matA[2][2]));
	cofactors.matA[1][1] = Determinant(Matrix2x2_Intrinsics(mat.matA[0][0], mat.matA[0][2], mat.matA[2][0], mat.matA[2][2]));
	cofactors.matA[1][2] = -Determinant(Matrix2x2_Intrinsics(mat.matA[0][0], mat.matA[0][1], mat.matA[2][0], mat.matA[2][1]));

	cofactors.matA[2][0] = Determinant(Matrix2x2_Intrinsics(mat.matA[0][1], mat.matA[0][2], mat.matA[1][1], mat.matA[1][2]));
	cofactors.matA[2][1] = -Determinant(Matrix2x2_Intrinsics(mat.matA[0][0], mat.matA[0][2], mat.matA[1][0], mat.matA[1][2]));
	cofactors.matA[2][2] = Determinant(Matrix2x2_Intrinsics(mat.matA[0][0], mat.matA[0][1], mat.matA[1][0], mat.matA[1][1]));

	return (1.0f / det) * Transpose(cofactors);
}

inline Matrix3x3_Intrinsics operator+(const Matrix3x3_Intrinsics& matA, const Matrix3x3_Intrinsics& matB)
{
	Matrix3x3_Intrinsics result;

	result.mat[0] = _mm_add_ps(matA.mat[0], matB.mat[0]);
	result.mat[1] = _mm_add_ps(matA.mat[1], matB.mat[1]);
	result.mat[2] = _mm_add_ps(matA.mat[2], matB.mat[2]);

	return result;
}

inline Matrix3x3_Intrinsics operator-(const Matrix3x3_Intrinsics& matA, const Matrix3x3_Intrinsics& matB)
{
	Matrix3x3_Intrinsics result;

	result.mat[0] = _mm_sub_ps(matA.mat[0], matB.mat[0]);
	result.mat[1] = _mm_sub_ps(matA.mat[1], matB.mat[1]);
	result.mat[2] = _mm_sub_ps(matA.mat[2], matB.mat[2]);

	return result;
}

inline Matrix3x3_Intrinsics operator*(const Matrix3x3_Intrinsics& matA, const Matrix3x3_Intrinsics& matB)
{
	Matrix3x3_Intrinsics result;

	__m128 b0 = matB.mat[0];
	__m128 b1 = matB.mat[1];
	__m128 b2 = matB.mat[2];
	__m128 b3 = _mm_set_ps1(0.0f);

	_MM_TRANSPOSE4_PS(b0, b1, b2, b3);

	result.mat[0] = _mm_set_ps(
		_mm_cvtss_f32(_mm_dp_ps(matA.mat[0], b3, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(matA.mat[0], b2, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(matA.mat[0], b1, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(matA.mat[0], b0, 0xf1))
	);

	result.mat[1] = _mm_set_ps(
		_mm_cvtss_f32(_mm_dp_ps(matA.mat[1], b3, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(matA.mat[1], b2, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(matA.mat[1], b1, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(matA.mat[1], b0, 0xf1))
	);

	result.mat[2] = _mm_set_ps(
		_mm_cvtss_f32(_mm_dp_ps(matA.mat[2], b3, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(matA.mat[2], b2, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(matA.mat[2], b1, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(matA.mat[2], b0, 0xf1))
	);

	return result;
}

inline vec3 operator*(const Matrix3x3_Intrinsics& mat, const vec3& vec)
{
	vec3 result;

	__m128 tempVec = _mm_set_ps(0.0f, vec.GetZ(), vec.GetY(), vec.GetX());

	result.SetX(_mm_cvtss_f32(_mm_dp_ps(mat.mat[0], tempVec, 0xf1)));
	result.SetY(_mm_cvtss_f32(_mm_dp_ps(mat.mat[1], tempVec, 0xf1)));
	result.SetZ(_mm_cvtss_f32(_mm_dp_ps(mat.mat[2], tempVec, 0xf1)));

	return result;
}

inline vec3 operator*(const vec3& vec, const Matrix3x3_Intrinsics& mat)
{
	vec3 result;

	__m128 tempMat0 = mat.mat[0];
	__m128 tempMat1 = mat.mat[1];
	__m128 tempMat2 = mat.mat[2];
	__m128 tempMat3 = _mm_set_ps1(0.0f);

	_MM_TRANSPOSE4_PS(tempMat0, tempMat1, tempMat2, tempMat3);

	__m128 tempVec = _mm_set_ps(0.0f, vec.GetZ(), vec.GetY(), vec.GetX());

	result.SetX(_mm_cvtss_f32(_mm_dp_ps(tempMat0, tempVec, 0xf1)));
	result.SetY(_mm_cvtss_f32(_mm_dp_ps(tempMat1, tempVec, 0xf1)));
	result.SetZ(_mm_cvtss_f32(_mm_dp_ps(tempMat2, tempVec, 0xf1)));

	return result;
}

inline Matrix3x3_Intrinsics operator*(const Matrix3x3_Intrinsics& mat, float k)
{
	Matrix3x3_Intrinsics result;

	__m128 scalar = _mm_set_ps1(k);
	result.mat[0] = _mm_mul_ps(mat.mat[0], scalar);
	result.mat[1] = _mm_mul_ps(mat.mat[1], scalar);
	result.mat[2] = _mm_mul_ps(mat.mat[2], scalar);

	return result;
}

inline Matrix3x3_Intrinsics operator*(float k, const Matrix3x3_Intrinsics& mat)
{
	Matrix3x3_Intrinsics result;

	__m128 scalar = _mm_set_ps1(k);
	result.mat[0] = _mm_mul_ps(scalar, mat.mat[0]);
	result.mat[1] = _mm_mul_ps(scalar, mat.mat[1]);
	result.mat[2] = _mm_mul_ps(scalar, mat.mat[2]);

	return result;
}

inline Matrix3x3_Intrinsics operator/(const Matrix3x3_Intrinsics& mat, float k)
{
	Matrix3x3_Intrinsics result;

	__m128 scalar = _mm_set_ps1(k);
	result.mat[0] = _mm_div_ps(mat.mat[0], scalar);
	result.mat[1] = _mm_div_ps(mat.mat[1], scalar);
	result.mat[2] = _mm_div_ps(mat.mat[2], scalar);

	return result;
}
//------------------------------------------------------------------------------------------------------------

//MATRIX4X4
//------------------------------------------------------------------------------------------------------------
class Matrix4x4_Intrinsics
{
public:

	//Creates an identity matrix
	Matrix4x4_Intrinsics();

	//Creates a matrix with the specified rows.
	Matrix4x4_Intrinsics(vec4 row0, vec4 row1, vec4 row2, vec4 row3);

	//Creates a matrix with the specified values.
	Matrix4x4_Intrinsics(float m00, float m01, float m02, float m03,
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
	static Matrix4x4_Intrinsics MakeIdentity();

	//Returns a scale matrix with the specified values.
	static Matrix4x4_Intrinsics Scale(float x, float y, float z);

	//Returns a translation matrix with the specified.
	static Matrix4x4_Intrinsics Translate(float x, float y, float z);

	//Returns a rotation matrix about the x-axis with the specified angle.
	//Angle should be given in degrees.
	static Matrix4x4_Intrinsics RotX(float angle);

	//Returns a rotation matrix about the y-axis with the specified angle.
	//Angle should be given in degrees.
	static Matrix4x4_Intrinsics RotY(float angle);

	//Returns a rotation matrix about the z-axis with the specified angle.
	//Angle should be given in degrees.
	static Matrix4x4_Intrinsics RotZ(float angle);

	//Returns the result of this + matB.
	Matrix4x4_Intrinsics& operator+=(const Matrix4x4_Intrinsics& matB);

	//Returns the result of this - matB.
	Matrix4x4_Intrinsics& operator-=(const Matrix4x4_Intrinsics& matB);

	//Returns the result of this * matB (matrix-matrix multiplication).
	Matrix4x4_Intrinsics& operator*=(const Matrix4x4_Intrinsics& matB);

	//Returns the result of this * k.
	Matrix4x4_Intrinsics& operator*=(float k);

	//Returns the result of this / k.
	Matrix4x4_Intrinsics& operator/=(float k);

	//Returns true if the matrix is an identity matrix, false otherwise.
	friend bool IsIdentity(const Matrix4x4_Intrinsics& mat);

	//Returns the transposition of the matrix.
	friend Matrix4x4_Intrinsics Transpose(const Matrix4x4_Intrinsics& mat);

	//Returns the determinant of the matrix.
	friend float Determinant(const Matrix4x4_Intrinsics& mat);

	//Returns the inverse of the matrix.
	//Returns the identity matrix if mat is noninvertible (singular)
	friend Matrix4x4_Intrinsics Inverse(const Matrix4x4_Intrinsics& mat);

	//Returns the result of matA + matB.
	friend Matrix4x4_Intrinsics operator+(const Matrix4x4_Intrinsics& matA, const Matrix4x4_Intrinsics& matB);

	//Returns the result of matA + matB.
	friend Matrix4x4_Intrinsics operator-(const Matrix4x4_Intrinsics& matA, const Matrix4x4_Intrinsics& matB);

	//Returns the result of matA * matB (matrix-matrix multiplication).
	friend Matrix4x4_Intrinsics operator*(const Matrix4x4_Intrinsics& matA, const Matrix4x4_Intrinsics& matB);

	//Returns the result of mat * vec (matrix-vector multiplication).
	friend vec4 operator*(const Matrix4x4_Intrinsics& mat, const vec4& vec);

	//Returns the result of mat * vec (vector-matrix multiplication).
	friend vec4 operator*(const vec4& vec, const Matrix4x4_Intrinsics& mat);

	//Returns the result of mat * k
	friend Matrix4x4_Intrinsics operator*(const Matrix4x4_Intrinsics& mat, float k);

	//Returns the result of k * mat
	friend Matrix4x4_Intrinsics operator*(float k, const Matrix4x4_Intrinsics& mat);

	//Returns the result of mat / k
	friend Matrix4x4_Intrinsics operator/(const Matrix4x4_Intrinsics& mat, float k);


private:
	union
	{
		__m128 mat[4];
		float matA[4][4];
	};
};

inline Matrix4x4_Intrinsics::Matrix4x4_Intrinsics()
{
	mat[0] = _mm_set_ps1(0.0f);
	mat[1] = _mm_set_ps1(0.0f);
	mat[2] = _mm_set_ps1(0.0f);
	mat[3] = _mm_set_ps1(0.0f);
}

inline Matrix4x4_Intrinsics::Matrix4x4_Intrinsics(vec4 row0, vec4 row1, vec4 row2, vec4 row3)
{
	SetRow(0, row0);
	SetRow(1, row1);
	SetRow(2, row2);
	SetRow(3, row3);
}

inline Matrix4x4_Intrinsics::Matrix4x4_Intrinsics(float m00, float m01, float m02, float m03,
	float m10, float m11, float m12, float m13,
	float m20, float m21, float m22, float m23,
	float m30, float m31, float m32, float m33)
{
	mat[0] = _mm_set_ps(m03, m02, m01, m00);
	mat[1] = _mm_set_ps(m13, m12, m11, m10);
	mat[2] = _mm_set_ps(m23, m22, m21, m20);
	mat[3] = _mm_set_ps(m33, m32, m31, m30);
}

inline float Matrix4x4_Intrinsics::GetElement(uint32_t row, uint32_t col) const
{
	assert((row < 4 && col < 4) && "Row or Col is greater than 3");

	return matA[row][col];
}

inline vec4 Matrix4x4_Intrinsics::GetRow(uint32_t row) const
{
	assert(row < 4 && "Row is greater than 3");

	return vec4(matA[row][0], matA[row][1], matA[row][2], matA[row][3]);
}

inline vec4 Matrix4x4_Intrinsics::GetCol(uint32_t col) const
{
	assert(col < 4 && "Col is greater than 3");

	return vec4(matA[0][col], matA[1][col], matA[2][col], matA[3][col]);

}

inline void Matrix4x4_Intrinsics::SetElement(uint32_t row, uint32_t col, float value)
{
	assert((row < 4 && col < 4) && "Row or Col is greater than 3");

	matA[row][col] = value;
}

inline void Matrix4x4_Intrinsics::SetRow(uint32_t row, vec4 values)
{
	assert(row < 4 && "Row is greater than 3");
	mat[row] = _mm_set_ps(values.GetW(), values.GetZ(), values.GetY(), values.GetX());
}

inline void Matrix4x4_Intrinsics::SetRow(uint32_t row, float val0, float val1, float val2, float val3)
{
	assert(row < 4 && "Row is greater than 3");
	mat[row] = _mm_set_ps(val3, val2, val1, val0);
}

inline void Matrix4x4_Intrinsics::SetCol(uint32_t col, float val0, float val1, float val2, float val3)
{
	assert(col < 4 && "Col is greater than 3");

	matA[0][col] = val0;
	matA[1][col] = val1;
	matA[2][col] = val2;
	matA[3][col] = val3;
}

inline void Matrix4x4_Intrinsics::SetCol(uint32_t col, vec4 values)
{
	assert(col < 4 && "Col is greater than 3");

	matA[0][col] = values.GetX();
	matA[1][col] = values.GetY();
	matA[2][col] = values.GetZ();
	matA[3][col] = values.GetW();
}

inline Matrix4x4_Intrinsics Matrix4x4_Intrinsics::MakeIdentity()
{
	return Matrix4x4_Intrinsics(1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
}

inline Matrix4x4_Intrinsics Matrix4x4_Intrinsics::Scale(float x, float y, float z)
{
	return Matrix4x4_Intrinsics(x, 0.0f, 0.0f, 0.0f,
		0.0f, y, 0.0f, 0.0f,
		0.0f, 0.0f, z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
}

inline Matrix4x4_Intrinsics Matrix4x4_Intrinsics::Translate(float x, float y, float z)
{
	return Matrix4x4_Intrinsics(1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		x, y, z, 1.0f);
}

inline Matrix4x4_Intrinsics Matrix4x4_Intrinsics::RotX(float angle)
{
	float ang = angle * (PI / 180.0f);
	float cosAngle = cos(ang);
	float sinAngle = sin(ang);

	return Matrix4x4_Intrinsics(1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cosAngle, sinAngle, 0.0f,
		0.0f, -sinAngle, cosAngle, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
}

inline Matrix4x4_Intrinsics Matrix4x4_Intrinsics::RotY(float angle)
{
	float ang = angle * (PI / 180.0f);
	float cosAngle = cos(ang);
	float sinAngle = sin(ang);

	return Matrix4x4_Intrinsics(cosAngle, 0.0f, -sinAngle, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		sinAngle, 0.0f, cosAngle, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
}

inline Matrix4x4_Intrinsics Matrix4x4_Intrinsics::RotZ(float angle)
{
	float ang = angle * (PI / 180.0f);
	float cosAngle = cos(ang);
	float sinAngle = sin(ang);

	return Matrix4x4_Intrinsics(cosAngle, sinAngle, 0.0f, 0.0f,
		-sinAngle, cosAngle, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
}

inline Matrix4x4_Intrinsics& Matrix4x4_Intrinsics::operator+=(const Matrix4x4_Intrinsics& matB)
{
	this->mat[0] = _mm_add_ps(this->mat[0], matB.mat[0]);
	this->mat[1] = _mm_add_ps(this->mat[1], matB.mat[1]);
	this->mat[2] = _mm_add_ps(this->mat[2], matB.mat[2]);
	this->mat[3] = _mm_add_ps(this->mat[3], matB.mat[3]);
	return *this;
}

inline Matrix4x4_Intrinsics& Matrix4x4_Intrinsics::operator-=(const Matrix4x4_Intrinsics& matB)
{
	this->mat[0] = _mm_sub_ps(this->mat[0], matB.mat[0]);
	this->mat[1] = _mm_sub_ps(this->mat[1], matB.mat[1]);
	this->mat[2] = _mm_sub_ps(this->mat[2], matB.mat[2]);
	this->mat[3] = _mm_sub_ps(this->mat[3], matB.mat[3]);
	return *this;
}

inline Matrix4x4_Intrinsics& Matrix4x4_Intrinsics::operator*=(const Matrix4x4_Intrinsics& matB)
{
	__m128 b0 = matB.mat[0];
	__m128 b1 = matB.mat[1];
	__m128 b2 = matB.mat[2];
	__m128 b3 = matB.mat[3];

	_MM_TRANSPOSE4_PS(b0, b1, b2, b3);

	this->mat[0] = _mm_set_ps(
		_mm_cvtss_f32(_mm_dp_ps(this->mat[0], b3, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(this->mat[0], b2, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(this->mat[0], b1, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(this->mat[0], b0, 0xf1))
	);

	this->mat[1] = _mm_set_ps(
		_mm_cvtss_f32(_mm_dp_ps(this->mat[1], b3, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(this->mat[1], b2, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(this->mat[1], b1, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(this->mat[1], b0, 0xf1))
	);


	this->mat[2] = _mm_set_ps(
		_mm_cvtss_f32(_mm_dp_ps(this->mat[2], b3, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(this->mat[2], b2, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(this->mat[2], b1, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(this->mat[2], b0, 0xf1))
	);

	this->mat[3] = _mm_set_ps(
		_mm_cvtss_f32(_mm_dp_ps(this->mat[3], b3, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(this->mat[3], b2, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(this->mat[3], b1, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(this->mat[3], b0, 0xf1))
	);

	return *this;
}

inline Matrix4x4_Intrinsics& Matrix4x4_Intrinsics::operator*=(float k)
{
	this->mat[0] = _mm_mul_ps(this->mat[0], _mm_set_ps1(k));
	this->mat[1] = _mm_mul_ps(this->mat[1], _mm_set_ps1(k));
	this->mat[2] = _mm_mul_ps(this->mat[2], _mm_set_ps1(k));
	this->mat[3] = _mm_mul_ps(this->mat[3], _mm_set_ps1(k));

	return *this;
}

inline Matrix4x4_Intrinsics& Matrix4x4_Intrinsics::operator/=(float k)
{
	this->mat[0] = _mm_div_ps(this->mat[0], _mm_set_ps1(k));
	this->mat[1] = _mm_div_ps(this->mat[1], _mm_set_ps1(k));
	this->mat[2] = _mm_div_ps(this->mat[2], _mm_set_ps1(k));
	this->mat[3] = _mm_div_ps(this->mat[3], _mm_set_ps1(k));

	return *this;
}


inline bool IsIdentity(const Matrix4x4_Intrinsics& mat)
{
	return
		CompareFloats(mat.matA[0][0], 1.0f) &&
		CompareFloats(mat.matA[0][1], 0.0f) &&
		CompareFloats(mat.matA[0][2], 0.0f) &&
		CompareFloats(mat.matA[0][3], 0.0f) &&

		CompareFloats(mat.matA[1][0], 0.0f) &&
		CompareFloats(mat.matA[1][1], 1.0f) &&
		CompareFloats(mat.matA[1][2], 0.0f) &&
		CompareFloats(mat.matA[1][3], 0.0f) &&

		CompareFloats(mat.matA[2][0], 0.0f) &&
		CompareFloats(mat.matA[2][1], 0.0f) &&
		CompareFloats(mat.matA[2][2], 1.0f) &&
		CompareFloats(mat.matA[2][3], 0.0f) &&

		CompareFloats(mat.matA[3][0], 0.0f) &&
		CompareFloats(mat.matA[3][1], 0.0f) &&
		CompareFloats(mat.matA[3][2], 0.0f) &&
		CompareFloats(mat.matA[3][3], 1.0f);
}

inline Matrix4x4_Intrinsics Transpose(const Matrix4x4_Intrinsics& mat)
{
	Matrix4x4_Intrinsics result;

	memcpy((void*)&result, (void*)&mat, sizeof(Matrix4x4_Intrinsics));
	_MM_TRANSPOSE4_PS(result.mat[0], result.mat[1], result.mat[2], result.mat[3]);

	return result;
}

inline float Determinant(const Matrix4x4_Intrinsics& mat)
{
	float c0 = mat.matA[0][0] * Determinant(Matrix3x3_Intrinsics(mat.matA[1][1], mat.matA[1][2], mat.matA[1][3],
		mat.matA[2][1], mat.matA[2][2], mat.matA[2][3],
		mat.matA[3][1], mat.matA[3][2], mat.matA[3][3]));

	float c1 = -mat.matA[0][1] * Determinant(Matrix3x3_Intrinsics(mat.matA[1][0], mat.matA[1][2], mat.matA[1][3],
		mat.matA[2][0], mat.matA[2][2], mat.matA[2][3],
		mat.matA[3][0], mat.matA[3][2], mat.matA[3][3]));

	float c2 = mat.matA[0][2] * Determinant(Matrix3x3_Intrinsics(mat.matA[1][0], mat.matA[1][1], mat.matA[1][3],
		mat.matA[2][0], mat.matA[2][1], mat.matA[2][3],
		mat.matA[3][0], mat.matA[3][1], mat.matA[3][3]));

	float c3 = -mat.matA[0][3] * Determinant(Matrix3x3_Intrinsics(mat.matA[1][0], mat.matA[1][1], mat.matA[1][2],
		mat.matA[2][0], mat.matA[2][1], mat.matA[2][2],
		mat.matA[3][0], mat.matA[3][1], mat.matA[3][2]));

	return c0 + c1 + c2 + c3;
}

inline Matrix4x4_Intrinsics Inverse(const Matrix4x4_Intrinsics& mat)
{
	float det = Determinant(mat);
	if (CompareFloats(det, 0.0f))
		return Matrix4x4_Intrinsics();

	Matrix4x4_Intrinsics cofactors;

	//0th row
	cofactors.matA[0][0] = Determinant(Matrix3x3_Intrinsics(mat.matA[1][1], mat.matA[1][2], mat.matA[1][3],
		mat.matA[2][1], mat.matA[2][2], mat.matA[2][3],
		mat.matA[3][1], mat.matA[3][2], mat.matA[3][3]));

	cofactors.matA[0][1] = -Determinant(Matrix3x3_Intrinsics(mat.matA[1][0], mat.matA[1][2], mat.matA[1][3],
		mat.matA[2][0], mat.matA[2][2], mat.matA[2][3],
		mat.matA[3][0], mat.matA[3][2], mat.matA[3][3]));

	cofactors.matA[0][2] = Determinant(Matrix3x3_Intrinsics(mat.matA[1][0], mat.matA[1][1], mat.matA[1][3],
		mat.matA[2][0], mat.matA[2][1], mat.matA[2][3],
		mat.matA[3][0], mat.matA[3][1], mat.matA[3][3]));

	cofactors.matA[0][3] = -Determinant(Matrix3x3_Intrinsics(mat.matA[1][0], mat.matA[1][1], mat.matA[1][2],
		mat.matA[2][0], mat.matA[2][1], mat.matA[2][2],
		mat.matA[3][0], mat.matA[3][1], mat.matA[3][2]));

	//1st row
	cofactors.matA[1][0] = -Determinant(Matrix3x3_Intrinsics(mat.matA[0][1], mat.matA[0][2], mat.matA[0][3],
		mat.matA[2][1], mat.matA[2][2], mat.matA[2][3],
		mat.matA[3][1], mat.matA[3][2], mat.matA[3][3]));

	cofactors.matA[1][1] = Determinant(Matrix3x3_Intrinsics(mat.matA[0][0], mat.matA[0][2], mat.matA[0][3],
		mat.matA[2][0], mat.matA[2][2], mat.matA[2][3],
		mat.matA[3][0], mat.matA[3][2], mat.matA[3][3]));

	cofactors.matA[1][2] = -Determinant(Matrix3x3_Intrinsics(mat.matA[0][0], mat.matA[0][1], mat.matA[0][3],
		mat.matA[2][0], mat.matA[2][1], mat.matA[2][3],
		mat.matA[3][0], mat.matA[3][1], mat.matA[3][3]));

	cofactors.matA[1][3] = Determinant(Matrix3x3_Intrinsics(mat.matA[0][0], mat.matA[0][1], mat.matA[0][2],
		mat.matA[2][0], mat.matA[2][1], mat.matA[2][2],
		mat.matA[3][0], mat.matA[3][1], mat.matA[3][2]));

	//2nd row
	cofactors.matA[2][0] = Determinant(Matrix3x3_Intrinsics(mat.matA[0][1], mat.matA[0][2], mat.matA[0][3],
		mat.matA[1][1], mat.matA[1][2], mat.matA[1][3],
		mat.matA[3][1], mat.matA[3][2], mat.matA[3][3]));

	cofactors.matA[2][1] = -Determinant(Matrix3x3_Intrinsics(mat.matA[0][0], mat.matA[0][2], mat.matA[0][3],
		mat.matA[1][0], mat.matA[1][2], mat.matA[1][3],
		mat.matA[3][0], mat.matA[3][2], mat.matA[3][3]));

	cofactors.matA[2][2] = Determinant(Matrix3x3_Intrinsics(mat.matA[0][0], mat.matA[0][1], mat.matA[0][3],
		mat.matA[1][0], mat.matA[1][1], mat.matA[1][3],
		mat.matA[3][0], mat.matA[3][1], mat.matA[3][3]));

	cofactors.matA[2][3] = -Determinant(Matrix3x3_Intrinsics(mat.matA[0][0], mat.matA[0][1], mat.matA[0][2],
		mat.matA[1][0], mat.matA[1][1], mat.matA[1][2],
		mat.matA[3][0], mat.matA[3][1], mat.matA[3][2]));

	//3rd row
	cofactors.matA[3][0] = -Determinant(Matrix3x3_Intrinsics(mat.matA[0][1], mat.matA[0][2], mat.matA[0][3],
		mat.matA[1][1], mat.matA[1][2], mat.matA[1][3],
		mat.matA[2][1], mat.matA[2][2], mat.matA[2][3]));

	cofactors.matA[3][1] = Determinant(Matrix3x3_Intrinsics(mat.matA[0][0], mat.matA[0][2], mat.matA[0][3],
		mat.matA[1][0], mat.matA[1][2], mat.matA[1][3],
		mat.matA[2][0], mat.matA[2][2], mat.matA[2][3]));

	cofactors.matA[3][2] = -Determinant(Matrix3x3_Intrinsics(mat.matA[0][0], mat.matA[0][1], mat.matA[0][3],
		mat.matA[1][0], mat.matA[1][1], mat.matA[1][3],
		mat.matA[2][0], mat.matA[2][1], mat.matA[2][3]));

	cofactors.matA[3][3] = Determinant(Matrix3x3_Intrinsics(mat.matA[0][0], mat.matA[0][1], mat.matA[0][2],
		mat.matA[1][0], mat.matA[1][1], mat.matA[1][2],
		mat.matA[2][0], mat.matA[2][1], mat.matA[2][2]));

	return (1.0f / det) * Transpose(cofactors);
}

inline Matrix4x4_Intrinsics operator+(const Matrix4x4_Intrinsics& matA, const Matrix4x4_Intrinsics& matB)
{
	Matrix4x4_Intrinsics result;

	result.mat[0] = _mm_add_ps(matA.mat[0], matB.mat[0]);
	result.mat[1] = _mm_add_ps(matA.mat[1], matB.mat[1]);
	result.mat[2] = _mm_add_ps(matA.mat[2], matB.mat[2]);
	result.mat[3] = _mm_add_ps(matA.mat[3], matB.mat[3]);

	return result;
}

inline Matrix4x4_Intrinsics operator-(const Matrix4x4_Intrinsics& matA, const Matrix4x4_Intrinsics& matB)
{
	Matrix4x4_Intrinsics result;

	result.mat[0] = _mm_sub_ps(matA.mat[0], matB.mat[0]);
	result.mat[1] = _mm_sub_ps(matA.mat[1], matB.mat[1]);
	result.mat[2] = _mm_sub_ps(matA.mat[2], matB.mat[2]);
	result.mat[3] = _mm_sub_ps(matA.mat[3], matB.mat[3]);

	return result;
}

inline Matrix4x4_Intrinsics operator*(const Matrix4x4_Intrinsics& matA, const Matrix4x4_Intrinsics& matB)
{
	Matrix4x4_Intrinsics result;

	__m128 b0 = matB.mat[0];
	__m128 b1 = matB.mat[1];
	__m128 b2 = matB.mat[2];
	__m128 b3 = matB.mat[3];

	_MM_TRANSPOSE4_PS(b0, b1, b2, b3);

	result.mat[0] = _mm_set_ps(
		_mm_cvtss_f32(_mm_dp_ps(matA.mat[0], b3, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(matA.mat[0], b2, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(matA.mat[0], b1, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(matA.mat[0], b0, 0xf1))
	);

	result.mat[1] = _mm_set_ps(
		_mm_cvtss_f32(_mm_dp_ps(matA.mat[1], b3, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(matA.mat[1], b2, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(matA.mat[1], b1, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(matA.mat[1], b0, 0xf1))
	);

	result.mat[2] = _mm_set_ps(
		_mm_cvtss_f32(_mm_dp_ps(matA.mat[2], b3, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(matA.mat[2], b2, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(matA.mat[2], b1, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(matA.mat[2], b0, 0xf1))
	);

	result.mat[3] = _mm_set_ps(
		_mm_cvtss_f32(_mm_dp_ps(matA.mat[3], b3, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(matA.mat[3], b2, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(matA.mat[3], b1, 0xf1)),
		_mm_cvtss_f32(_mm_dp_ps(matA.mat[3], b0, 0xf1))
	);

	return result;
}

inline vec4 operator*(const Matrix4x4_Intrinsics& mat, const vec4& vec)
{
	vec4 result;

	__m128 tempVec = _mm_set_ps(vec.GetW(), vec.GetZ(), vec.GetY(), vec.GetX());

	result.SetX(_mm_cvtss_f32(_mm_dp_ps(mat.mat[0], tempVec, 0xf1)));
	result.SetY(_mm_cvtss_f32(_mm_dp_ps(mat.mat[1], tempVec, 0xf1)));
	result.SetZ(_mm_cvtss_f32(_mm_dp_ps(mat.mat[2], tempVec, 0xf1)));
	result.SetW(_mm_cvtss_f32(_mm_dp_ps(mat.mat[3], tempVec, 0xf1)));

	return result;
}

inline vec4 operator*(const vec4& vec, const Matrix4x4_Intrinsics& mat)
{
	vec4 result;

	__m128 tempMat0 = mat.mat[0];
	__m128 tempMat1 = mat.mat[1];
	__m128 tempMat2 = mat.mat[2];
	__m128 tempMat3 = mat.mat[3];

	_MM_TRANSPOSE4_PS(tempMat0, tempMat1, tempMat2, tempMat3);

	__m128 tempVec = _mm_set_ps(vec.GetW(), vec.GetZ(), vec.GetY(), vec.GetX());

	result.SetX(_mm_cvtss_f32(_mm_dp_ps(tempMat0, tempVec, 0xf1)));
	result.SetY(_mm_cvtss_f32(_mm_dp_ps(tempMat1, tempVec, 0xf1)));
	result.SetZ(_mm_cvtss_f32(_mm_dp_ps(tempMat2, tempVec, 0xf1)));
	result.SetW(_mm_cvtss_f32(_mm_dp_ps(tempMat3, tempVec, 0xf1)));

	return result;
}

inline Matrix4x4_Intrinsics operator*(const Matrix4x4_Intrinsics& mat, float k)
{
	Matrix4x4_Intrinsics result;

	__m128 scalar = _mm_set_ps1(k);
	result.mat[0] = _mm_mul_ps(mat.mat[0], scalar);
	result.mat[1] = _mm_mul_ps(mat.mat[1], scalar);
	result.mat[2] = _mm_mul_ps(mat.mat[2], scalar);
	result.mat[3] = _mm_mul_ps(mat.mat[3], scalar);

	return result;
}

inline Matrix4x4_Intrinsics operator*(float k, const Matrix4x4_Intrinsics& mat)
{
	Matrix4x4_Intrinsics result;

	__m128 scalar = _mm_set_ps1(k);
	result.mat[0] = _mm_mul_ps(scalar, mat.mat[0]);
	result.mat[1] = _mm_mul_ps(scalar, mat.mat[1]);
	result.mat[2] = _mm_mul_ps(scalar, mat.mat[2]);
	result.mat[3] = _mm_mul_ps(scalar, mat.mat[3]);

	return result;
}

inline Matrix4x4_Intrinsics operator/(const Matrix4x4_Intrinsics& mat, float k)
{
	Matrix4x4_Intrinsics result;

	__m128 scalar = _mm_set_ps1(k);
	result.mat[0] = _mm_div_ps(mat.mat[0], scalar);
	result.mat[1] = _mm_div_ps(mat.mat[1], scalar);
	result.mat[2] = _mm_div_ps(mat.mat[2], scalar);
	result.mat[3] = _mm_div_ps(mat.mat[3], scalar);

	return result;
}
//------------------------------------------------------------------------------------------------------------

typedef Matrix4x4_Intrinsics mat4;
typedef Matrix3x3_Intrinsics mat3;
typedef Matrix2x2_Intrinsics mat2;

//QUATERNION
//------------------------------------------------------------------------------------------------------------
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

inline Quaternion_Intrinsics::Quaternion_Intrinsics()
{
	this->quat = _mm_set_ps(0.0f, 0.0f, 0.0f, 0.0f);
}

inline Quaternion_Intrinsics::Quaternion_Intrinsics(float x, float y, float z, float scalar)
{
	this->quat = _mm_set_ps(scalar, z, y, x);
}

inline Quaternion_Intrinsics::Quaternion_Intrinsics(__m128 quat)
{
	this->quat = quat;
}

inline float Quaternion_Intrinsics::GetX() const
{
	return quatA[0];
}

inline float Quaternion_Intrinsics::GetY() const
{
	return quatA[1];
}

inline float Quaternion_Intrinsics::GetZ() const
{
	return quatA[2];
}

inline float Quaternion_Intrinsics::GetScalar() const
{
	return quatA[3];
}

inline void Quaternion_Intrinsics::SetX(float x)
{
	quatA[0] = x;
}

inline void Quaternion_Intrinsics::SetY(float y)
{
	quatA[1] = y;
}

inline void Quaternion_Intrinsics::SetZ(float z)
{
	quatA[2] = z;
}

inline void Quaternion_Intrinsics::SetScalar(float scalar)
{
	quatA[3] = scalar;
}

inline void Quaternion_Intrinsics::Set(float x, float y, float z, float scalar)
{
	this->quat = _mm_set_ps(scalar, z, y, x);
}

inline Quaternion_Intrinsics& Quaternion_Intrinsics::operator+=(const Quaternion_Intrinsics& b)
{
	this->quat = _mm_add_ps(this->quat, b.quat);

	return *this;
}

inline Quaternion_Intrinsics& Quaternion_Intrinsics::operator-=(const Quaternion_Intrinsics& b)
{
	this->quat = _mm_sub_ps(this->quat, b.quat);

	return *this;
}

inline Quaternion_Intrinsics& Quaternion_Intrinsics::operator*=(const Quaternion_Intrinsics& b)
{
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

inline Quaternion_Intrinsics& Quaternion_Intrinsics::operator*=(float k)
{
	this->quat = _mm_mul_ps(this->quat, _mm_set_ps1(k));

	return *this;
}

inline Quaternion_Intrinsics& Quaternion_Intrinsics::operator/=(float k)
{
	this->quat = _mm_div_ps(this->quat, _mm_set_ps1(k));

	return *this;
}

inline bool Quaternion_Intrinsics::operator==(const Quaternion_Intrinsics& b) const
{
	return CompareFloats(this->quatA[0], b.quatA[0]) && CompareFloats(this->quatA[1], b.quatA[1])
		&& CompareFloats(this->quatA[2], b.quatA[2]) && CompareFloats(this->quatA[3], b.quatA[3]);
}

inline bool Quaternion_Intrinsics::operator!=(const Quaternion_Intrinsics& b) const
{
	return !(*this == b);
}

inline Quaternion_Intrinsics Quaternion_Intrinsics::MakeIdentity()
{
	return Quaternion_Intrinsics(0.0f, 0.0f, 0.0f, 1.0f);
}

inline Quaternion_Intrinsics Quaternion_Intrinsics::MakeRotation(float angle, float x, float y, float z)
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

inline Quaternion_Intrinsics Quaternion_Intrinsics::MakeRotation(float angle, const Vector3_Intrinsics& axis)
{
	float ang = (angle / 2.0f) * (PI / 180.0f);
	float cosAngle = cos(ang);
	float sinAngle = sin(ang);
	Vector3_Intrinsics axisN = Normalize(Vector3_Intrinsics(axis.GetX(), axis.GetY(), axis.GetZ()));

	__m128 t0 = _mm_set_ps(cosAngle, sinAngle, sinAngle, sinAngle);
	__m128 t1 = _mm_set_ps(1.0f, axisN.GetZ(), axisN.GetY(), axisN.GetX());

	__m128 t2 = _mm_mul_ps(t0, t1);

	return Quaternion_Intrinsics(t2);
}

inline Quaternion_Intrinsics operator+(const Quaternion_Intrinsics& a, const Quaternion_Intrinsics& b)
{
	return Quaternion_Intrinsics(_mm_add_ps(a.quat, b.quat));
}

inline Quaternion_Intrinsics operator-(const Quaternion_Intrinsics& a, const Quaternion_Intrinsics& b)
{
	return Quaternion_Intrinsics(_mm_sub_ps(a.quat, b.quat));
}

inline Quaternion_Intrinsics operator-(const Quaternion_Intrinsics& a)
{
	return Quaternion_Intrinsics(_mm_sub_ps(_mm_set_ps1(0.0f), a.quat));
}

inline Quaternion_Intrinsics operator*(const Quaternion_Intrinsics& a, const Quaternion_Intrinsics& b)
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

inline Quaternion_Intrinsics operator*(const Quaternion_Intrinsics& a, float k)
{
	return Quaternion_Intrinsics(_mm_mul_ps(a.quat, _mm_set_ps1(k)));
}

inline Quaternion_Intrinsics operator*(float k, const Quaternion_Intrinsics& a)
{
	return Quaternion_Intrinsics(_mm_mul_ps(_mm_set_ps1(k), a.quat));
}

inline Quaternion_Intrinsics operator/(const Quaternion_Intrinsics& a, float k)
{
	return Quaternion_Intrinsics(_mm_div_ps(a.quat, _mm_set_ps1(k)));
}

inline float DotProduct(const Quaternion_Intrinsics& a, const Quaternion_Intrinsics& b)
{
	return _mm_cvtss_f32(_mm_dp_ps(a.quat, b.quat, 0xff));
}

inline float Length(const Quaternion_Intrinsics& a)
{
	return _mm_cvtss_f32(_mm_sqrt_ps(_mm_dp_ps(a.quat, a.quat, 0xff)));
}

inline Quaternion_Intrinsics Normalize(const Quaternion_Intrinsics& a)
{
	float mag = Length(a);
	assert(!CompareFloats(mag, 0.0f) && "Magnitude of the Quaternion_Intrinsics is 0.");

	return a / mag;
}

inline Quaternion_Intrinsics Conjugate(const Quaternion_Intrinsics& a)
{
	return Quaternion_Intrinsics(_mm_xor_ps(_mm_set_ps(0.0f, -0.0f, -0.0f, -0.0f), a.quat));
}

inline Quaternion_Intrinsics Inverse(const Quaternion_Intrinsics& a)
{
	return Conjugate(a) / Length(a);
}

inline bool IsIdentity(const Quaternion_Intrinsics& a)
{
	return CompareFloats(a.quatA[0], 0.0f) && CompareFloats(a.quatA[1], 0.0f)
		&& CompareFloats(a.quatA[2], 0.0f) && CompareFloats(a.quatA[3], 1.0f);
}

inline Vector3_Intrinsics Rotate(const Quaternion_Intrinsics& q, const Vector3_Intrinsics& p)
{
	Quaternion_Intrinsics point(p.GetX(), p.GetY(), p.GetZ(), 0.0f);

	Quaternion_Intrinsics result = q * point * Conjugate(q);

	return Vector3_Intrinsics(result.quatA[0], result.quatA[1], result.quatA[2]);
}

inline Vector4_Intrinsics Rotate(const Quaternion_Intrinsics& q, const Vector4_Intrinsics& p)
{
	Quaternion_Intrinsics point(p.GetX(), p.GetY(), p.GetZ(), 0.0f);

	Quaternion_Intrinsics result = q * point * Conjugate(q);

	return Vector4_Intrinsics(result.quatA[0], result.quatA[1], result.quatA[2], p.GetW());
}

inline Matrix4x4_Intrinsics QuaternionToMatrix(const Quaternion_Intrinsics& q)
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
//------------------------------------------------------------------------------------------------------------

typedef Quaternion_Intrinsics quat;