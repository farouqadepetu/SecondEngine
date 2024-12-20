#pragma once

#include <cstdint>
#include "vectors_intrinsics.h"

//Row-Major
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

#ifdef _DEBUG
#include <iostream>
inline void Print(const Matrix2x2_Intrinsics& a)
{
	std::cout << "[" << a.GetElement(0, 0) << ", " << a.GetElement(0, 1) << "]" << std::endl;
	std::cout << "[" << a.GetElement(1, 0) << ", " << a.GetElement(1, 1) << "]" << std::endl;
	std::cout << std::endl;
}
#endif

//Row-Major
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

#ifdef _DEBUG
#include <iostream>
inline void Print(const Matrix3x3_Intrinsics& a)
{
	std::cout << "[" << a.GetElement(0, 0) << ", " << a.GetElement(0, 1) << ", " << a.GetElement(0, 2) << "]" << std::endl;
	std::cout << "[" << a.GetElement(1, 0) << ", " << a.GetElement(1, 1) << ", " << a.GetElement(1, 2) << "]" << std::endl;
	std::cout << "[" << a.GetElement(2, 0) << ", " << a.GetElement(2, 1) << ", " << a.GetElement(2, 2) << "]" << std::endl;
	std::cout << std::endl;
}
#endif

//Row-Major
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

#ifdef _DEBUG
#include <iostream>
inline void Print(const Matrix4x4_Intrinsics& a)
{
	std::cout << "[" << a.GetElement(0, 0) << ", " << a.GetElement(0, 1) << ", " << a.GetElement(0, 2) << ", " << a.GetElement(0, 3) << "]" << std::endl;
	std::cout << "[" << a.GetElement(1, 0) << ", " << a.GetElement(1, 1) << ", " << a.GetElement(1, 2) << ", " << a.GetElement(1, 3) << "]" << std::endl;
	std::cout << "[" << a.GetElement(2, 0) << ", " << a.GetElement(2, 1) << ", " << a.GetElement(2, 2) << ", " << a.GetElement(2, 3) << "]" << std::endl;
	std::cout << "[" << a.GetElement(3, 0) << ", " << a.GetElement(3, 1) << ", " << a.GetElement(3, 2) << ", " << a.GetElement(3, 3) << "]" << std::endl;
	std::cout << std::endl;
}
#endif

typedef Matrix4x4_Intrinsics mat4;
typedef Matrix3x3_Intrinsics mat3;
typedef Matrix2x2_Intrinsics mat2;