#include <cassert>
#include <cmath>
#include "matrices.h"
#include "../Math/math_utility.h"

//MATRIX2X2 IMPLEMENTATION
Matrix2x2::Matrix2x2()
{
    mat[0][0] = 0.0f;
    mat[0][1] = 0.0f;

    mat[1][0] = 0.0f;
    mat[1][1] = 0.0f;
}

Matrix2x2::Matrix2x2(vec2 row0, vec2 row1)
{
    SetRow(0, row0);
    SetRow(1, row1);
}

Matrix2x2::Matrix2x2(float m00, float m01,
                     float m10, float m11)
{
    mat[0][0] = m00;
    mat[0][1] = m01;

    mat[1][0] = m10;
    mat[1][1] = m11;
}

float Matrix2x2::GetElement(uint32_t row, uint32_t col) const
{
    assert((row < 2 && col < 2) && "Row or Col is greater than 1");

    return mat[row][col];
}

vec2 Matrix2x2::GetRow(uint32_t row) const
{
    assert(row < 2 && "Row is greater than 1");
    vec2 result(mat[row][0], mat[row][1]);
    return result;
}

vec2 Matrix2x2::GetCol(uint32_t col) const
{
    assert(col < 2 && "Col is greater than 1");
    vec2 result(mat[0][col], mat[1][col]);
    return result;
}

void Matrix2x2::SetElement(uint32_t row, uint32_t col, float value)
{
    assert((row < 2 && col < 2) && "Row or Col is greater than 1");
    mat[row][col] = value;
}

void Matrix2x2::SetRow(uint32_t row, vec2 values)
{
    assert(row < 2 && "Row is greater than 1");

    mat[row][0] = values.GetX();
    mat[row][1] = values.GetY();
}

void Matrix2x2::SetRow(uint32_t row, float val0, float val1)
{
    assert(row < 2 && "Row is greater than 1");

    mat[row][0] = val0;
    mat[row][1] = val1;
}

void Matrix2x2::SetCol(uint32_t col, float val0, float val1)
{
    assert(col < 2 && "Col is greater than 1");

    mat[0][col] = val0;
    mat[1][col] = val1;
}

void Matrix2x2::SetCol(uint32_t col, vec2 values)
{
    assert(col < 2 && "Col is greater than 1");
    return;

    mat[0][col] = values.GetX();
    mat[1][col] = values.GetY();
}

Matrix2x2 Matrix2x2::MakeIdentity()
{
    return Matrix2x2(1.0f, 0.0f,
                     0.0f, 1.0f);
}

Matrix2x2 Matrix2x2::Scale(float x, float y)
{
    return Matrix2x2(x, 0.0f,
                     0.0f, y);
}

Matrix2x2 Matrix2x2::RotZ(float angle)
{
    float ang = angle * (PI / 180.0f);
    float cosAngle = cos(ang);
    float sinAngle = sin(ang);

    return Matrix2x2(cosAngle, sinAngle,
                     -sinAngle, cosAngle);
}

Matrix2x2& Matrix2x2::operator+=(const Matrix2x2& matB)
{
    this->mat[0][0] += matB.mat[0][0];
    this->mat[0][1] += matB.mat[0][1];

    this->mat[1][0] += matB.mat[1][0];
    this->mat[1][1] += matB.mat[1][1];

    return *this;
}

Matrix2x2& Matrix2x2::operator-=(const Matrix2x2& matB)
{
    this->mat[0][0] -= matB.mat[0][0];
    this->mat[0][1] -= matB.mat[0][1];

    this->mat[1][0] -= matB.mat[1][0];
    this->mat[1][1] -= matB.mat[1][1];

    return *this;
}

Matrix2x2& Matrix2x2::operator*=(const Matrix2x2& matB)
{
    Matrix2x2 result;

    result.mat[0][0] = this->mat[0][0] * matB.mat[0][0] + this->mat[0][1] * matB.mat[1][0];
    result.mat[0][1] = this->mat[0][0] * matB.mat[0][1] + this->mat[0][1] * matB.mat[1][1];

    result.mat[1][0] = this->mat[1][0] * matB.mat[0][0] + this->mat[1][1] * matB.mat[1][0];
    result.mat[1][1] = this->mat[1][0] * matB.mat[0][1] + this->mat[1][1] * matB.mat[1][1];

    *this = result;

    return *this;
}

Matrix2x2& Matrix2x2::operator*=(float k)
{
    this->mat[0][0] *= k;
    this->mat[0][1] *= k;

    this->mat[1][0] *= k;
    this->mat[1][1] *= k;


    return *this;
}

Matrix2x2& Matrix2x2::operator/=(float k)
{
    this->mat[0][0] /= k;
    this->mat[0][1] /= k;

    this->mat[1][0] /= k;
    this->mat[1][1] /= k;

    return *this;
}

bool IsIdentity(const Matrix2x2& mat)
{
    return
        CompareFloats(mat.mat[0][0], 1.0f) &&
        CompareFloats(mat.mat[0][1], 0.0f) &&

        CompareFloats(mat.mat[1][0], 0.0f) &&
        CompareFloats(mat.mat[1][1], 1.0f);
}

Matrix2x2 Transpose(const Matrix2x2& mat)
{
    Matrix2x2 result;

    result.mat[0][0] = mat.mat[0][0];
    result.mat[0][1] = mat.mat[1][0];

    result.mat[1][0] = mat.mat[0][1];
    result.mat[1][1] = mat.mat[1][1];

    return result;
}

float Determinant(const Matrix2x2& mat)
{
    return mat.mat[0][0] * mat.mat[1][1] - mat.mat[0][1] * mat.mat[1][0];
}


Matrix2x2 Inverse(const Matrix2x2& mat)
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

Matrix2x2 operator+(const Matrix2x2& matA, const Matrix2x2& matB)
{
    Matrix2x2 result;

    result.mat[0][0] = matA.mat[0][0] + matB.mat[0][0];
    result.mat[0][1] = matA.mat[0][1] + matB.mat[0][1];

    result.mat[1][0] = matA.mat[1][0] + matB.mat[1][0];
    result.mat[1][1] = matA.mat[1][1] + matB.mat[1][1];

    return result;
}

Matrix2x2 operator-(const Matrix2x2& matA, const Matrix2x2& matB)
{
    Matrix2x2 result;

    result.mat[0][0] = matA.mat[0][0] - matB.mat[0][0];
    result.mat[0][1] = matA.mat[0][1] - matB.mat[0][1];

    result.mat[1][0] = matA.mat[1][0] - matB.mat[1][0];
    result.mat[1][1] = matA.mat[1][1] - matB.mat[1][1];

    return result;
}

Matrix2x2 operator*(const Matrix2x2& matA, const Matrix2x2& matB)
{
    Matrix2x2 result;

    result.mat[0][0] = matA.mat[0][0] * matB.mat[0][0] + matA.mat[0][1] * matB.mat[1][0];
    result.mat[0][1] = matA.mat[0][0] * matB.mat[0][1] + matA.mat[0][1] * matB.mat[1][1];

    result.mat[1][0] = matA.mat[1][0] * matB.mat[0][0] + matA.mat[1][1] * matB.mat[1][0];
    result.mat[1][1] = matA.mat[1][0] * matB.mat[0][1] + matA.mat[1][1] * matB.mat[1][1];

    return result;
}

vec2 operator*(const Matrix2x2& mat, const vec2& vec)
{
    vec2 result;

    result.SetX(mat.mat[0][0] * vec.GetX() + mat.mat[0][1] * vec.GetY());
    result.SetY(mat.mat[1][0] * vec.GetX() + mat.mat[1][1] * vec.GetY());

    return result;
}

vec2 operator*(const vec2& vec, const Matrix2x2& mat)
{
    vec2 result;

    result.SetX(vec.GetX() * mat.mat[0][0] + vec.GetY() * mat.mat[1][0]);
    result.SetY(vec.GetX() * mat.mat[0][1] + vec.GetY() * mat.mat[1][1]);

    return result;
}

Matrix2x2 operator*(const Matrix2x2& mat, float k)
{
    Matrix2x2 result;

    result.mat[0][0] = mat.mat[0][0] * k;
    result.mat[0][1] = mat.mat[0][1] * k;

    result.mat[1][0] = mat.mat[1][0] * k;
    result.mat[1][1] = mat.mat[1][1] * k;

    return result;
}

Matrix2x2 operator*(float k, const Matrix2x2& mat)
{
    Matrix2x2 result;

    result.mat[0][0] = k * mat.mat[0][0];
    result.mat[0][1] = k * mat.mat[0][1];

    result.mat[1][0] = k * mat.mat[1][0];
    result.mat[1][1] = k * mat.mat[1][1];

    return result;
}

Matrix2x2 operator/(const Matrix2x2& mat, float k)
{
    Matrix2x2 result;

    result.mat[0][0] = mat.mat[0][0] / k;
    result.mat[0][1] = mat.mat[0][1] / k;

    result.mat[1][0] = mat.mat[1][0] / k;
    result.mat[1][1] = mat.mat[1][1] / k;

    return result;
}


//MATRIX3X3 IMPLEMENTATION
Matrix3x3::Matrix3x3()
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

Matrix3x3::Matrix3x3(vec3 row0, vec3 row1, vec3 row2)
{
    SetRow(0, row0);
    SetRow(1, row1);
    SetRow(2, row2);
}

Matrix3x3::Matrix3x3(float m00, float m01, float m02,
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

float Matrix3x3::GetElement(uint32_t row, uint32_t col) const
{
    assert((row < 3 && col < 3) && "Row or Col is greater than 2");

    return mat[row][col];
}

vec3 Matrix3x3::GetRow(uint32_t row) const
{
    assert(row < 3 && "Row is greater than 2");
    vec3 result(mat[row][0], mat[row][1], mat[row][2]);
    return result;
}

vec3 Matrix3x3::GetCol(uint32_t col) const
{
    assert(col < 3 && "Col is greater than 2");
    vec3 result(mat[0][col], mat[1][col], mat[2][col]);
    return result;
}

void Matrix3x3::SetElement(uint32_t row, uint32_t col, float value)
{
    assert((row < 3 && col < 3) && "Row or Col is greater than 2");
    mat[row][col] = value;
}

void Matrix3x3::SetRow(uint32_t row, vec3 values)
{
    assert(row < 3 && "Row is greater than 2");

    mat[row][0] = values.GetX();
    mat[row][1] = values.GetY();
    mat[row][2] = values.GetZ();
}

void Matrix3x3::SetRow(uint32_t row, float val0, float val1, float val2)
{
    assert(row < 3 && "Row is greater than 2");

    mat[row][0] = val0;
    mat[row][1] = val1;
    mat[row][2] = val2;
}

void Matrix3x3::SetCol(uint32_t col, float val0, float val1, float val2)
{
    assert(col < 3 && "Col is greater than 2");
    mat[0][col] = val0;
    mat[1][col] = val1;
    mat[2][col] = val2;
}

void Matrix3x3::SetCol(uint32_t col, vec3 values)
{
    assert(col < 3 && "Col is greater than 2");

    mat[0][col] = values.GetX();
    mat[1][col] = values.GetY();
    mat[2][col] = values.GetZ();
}

Matrix3x3 Matrix3x3::MakeIdentity()
{
    return Matrix3x3(1.0f, 0.0f, 0.0f,
                     0.0f, 1.0f, 0.0f,
                     0.0f, 0.0f, 1.0f);
}

Matrix3x3 Matrix3x3::Scale(float x, float y, float z)
{
    return Matrix3x3(x, 0.0f, 0.0f,
                     0.0f, y, 0.0f,
                     0.0f, 0.0f, z);
}

Matrix3x3 Matrix3x3::Translate(float x, float y)
{
    return Matrix3x3(1.0f, 0.0f, 0.0f,
                     0.0f, 1.0f, 0.0f,
                     x, y, 1.0f);
}

Matrix3x3 Matrix3x3::RotX(float angle)
{
    float ang = angle * (PI / 180.0f);
    float cosAngle = cos(ang);
    float sinAngle = sin(ang);

    return Matrix3x3(1.0f, 0.0f, 0.0f,
                     0.0f, cosAngle, sinAngle,
                     0.0f, -sinAngle, cosAngle);
}

Matrix3x3 Matrix3x3::RotY(float angle)
{
    float ang = angle * (PI / 180.0f);
    float cosAngle = cos(ang);
    float sinAngle = sin(ang);

    return Matrix3x3(cosAngle, 0.0f, -sinAngle,
                     0.0f, 1.0f, 0.0f,
                     sinAngle, 0.0f, cosAngle);
}

Matrix3x3 Matrix3x3::RotZ(float angle)
{
    float ang = angle * (PI / 180.0f);
    float cosAngle = cos(ang);
    float sinAngle = sin(ang);

    return Matrix3x3(cosAngle, sinAngle, 0.0f,
                     -sinAngle, cosAngle, 0.0f,
                     0.0f, 0.0f, 1.0f);
}

Matrix3x3& Matrix3x3::operator+=(const Matrix3x3& matB)
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

Matrix3x3& Matrix3x3::operator-=(const Matrix3x3& matB)
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

Matrix3x3& Matrix3x3::operator*=(const Matrix3x3& matB)
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

Matrix3x3& Matrix3x3::operator*=(float k)
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

Matrix3x3& Matrix3x3::operator/=(float k)
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

bool IsIdentity(const Matrix3x3& mat)
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

Matrix3x3 Transpose(const Matrix3x3& mat)
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

float Determinant(const Matrix3x3& mat)
{
    float t0 = mat.mat[0][0] * (mat.mat[1][1] * mat.mat[2][2] - mat.mat[1][2] * mat.mat[2][1]);

    float t1 = mat.mat[0][1] * (mat.mat[1][2] * mat.mat[2][0] - mat.mat[1][0] * mat.mat[2][2]);

    float t2 = mat.mat[0][2] * (mat.mat[1][0] * mat.mat[2][1] - mat.mat[1][1] * mat.mat[2][0]);

    return t0 + t1 + t2;
}


Matrix3x3 Inverse(const Matrix3x3& mat)
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

Matrix3x3 operator+(const Matrix3x3& matA, const Matrix3x3& matB)
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

Matrix3x3 operator-(const Matrix3x3& matA, const Matrix3x3& matB)
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

Matrix3x3 operator*(const Matrix3x3& matA, const Matrix3x3& matB)
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

vec3 operator*(const Matrix3x3& mat, const vec3& vec)
{
    vec3 result;

    result.SetX(mat.mat[0][0] * vec.GetX() + mat.mat[0][1] * vec.GetY() + mat.mat[0][2] * vec.GetZ());
    result.SetY(mat.mat[1][0] * vec.GetX() + mat.mat[1][1] * vec.GetY() + mat.mat[1][2] * vec.GetZ());
    result.SetZ(mat.mat[2][0] * vec.GetX() + mat.mat[2][1] * vec.GetY() + mat.mat[2][2] * vec.GetZ());

    return result;
}

vec3 operator*(const vec3& vec, const Matrix3x3& mat)
{
    vec3 result;

    result.SetX(vec.GetX() * mat.mat[0][0] + vec.GetY() * mat.mat[1][0] + vec.GetZ() * mat.mat[2][0]);
    result.SetY(vec.GetX() * mat.mat[0][1] + vec.GetY() * mat.mat[1][1] + vec.GetZ() * mat.mat[2][1]);
    result.SetZ(vec.GetX() * mat.mat[0][2] + vec.GetY() * mat.mat[1][2] + vec.GetZ() * mat.mat[2][2]);

    return result;
}

Matrix3x3 operator*(const Matrix3x3& mat, float k)
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

Matrix3x3 operator*(float k, const Matrix3x3& mat)
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

Matrix3x3 operator/(const Matrix3x3& mat, float k)
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


//Matrix4X4 IMPLEMENTATION
Matrix4x4::Matrix4x4()
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

Matrix4x4::Matrix4x4(vec4 row0, vec4 row1, vec4 row2, vec4 row3)
{
    SetRow(0, row0);
    SetRow(1, row1);
    SetRow(2, row2);
    SetRow(3, row3);
}

Matrix4x4::Matrix4x4(float m00, float m01, float m02, float m03, 
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

float Matrix4x4::GetElement(uint32_t row, uint32_t col) const
{
    assert((row < 4 && col < 4) && "Row or Col is greater than 3");

    return mat[row][col];
}

vec4 Matrix4x4::GetRow(uint32_t row) const
{
    assert(row < 4 && "Row is greater than 3");
    vec4 result(mat[row][0], mat[row][1], mat[row][2], mat[row][3]);
    return result;
}

vec4 Matrix4x4::GetCol(uint32_t col) const
{
    assert(col < 4 && "Col is greater than 3");
    vec4 result(mat[0][col], mat[1][col], mat[2][col], mat[3][col]);
    return result;
}

void Matrix4x4::SetElement(uint32_t row, uint32_t col, float value)
{
    assert((row < 4 && col < 4) && "Row or Col is greater than 3");
    mat[row][col] = value;
}

void Matrix4x4::SetRow(uint32_t row, vec4 values)
{
    assert(row < 4 && "Row is greater than 3");

    mat[row][0] = values.GetX();
    mat[row][1] = values.GetY();
    mat[row][2] = values.GetZ();
    mat[row][3] = values.GetW();
}

void Matrix4x4::SetRow(uint32_t row, float val0, float val1, float val2, float val3)
{
    assert(row < 4 && "Row is greater than 3");

    mat[row][0] = val0;
    mat[row][1] = val1;
    mat[row][2] = val2;
    mat[row][3] = val3;
}

void Matrix4x4::SetCol(uint32_t col, float val0, float val1, float val2, float val3)
{
    assert(col < 4 && "Col is greater than 3");

    mat[0][col] = val0;
    mat[1][col] = val1;
    mat[2][col] = val2;
    mat[3][col] = val3;
}

void Matrix4x4::SetCol(uint32_t col, vec4 values)
{
    assert(col < 4 && "Col is greater than 3");

    mat[0][col] = values.GetX();
    mat[1][col] = values.GetY();
    mat[2][col] = values.GetZ();
    mat[3][col] = values.GetW();
}

Matrix4x4 Matrix4x4::MakeIdentity()
{
    return Matrix4x4(1.0f, 0.0f, 0.0f, 0.0f,
                     0.0f, 1.0f, 0.0f, 0.0f,
                     0.0f, 0.0f, 1.0f, 0.0f,
                     0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4x4 Matrix4x4::Scale(float x, float y, float z)
{
    return Matrix4x4(x, 0.0f, 0.0f, 0.0f,
        0.0f, y, 0.0f, 0.0f,
        0.0f, 0.0f, z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4x4 Matrix4x4::Translate(float x, float y, float z)
{
    return Matrix4x4(1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        x, y, z, 1.0f);
}

Matrix4x4 Matrix4x4::RotX(float angle)
{
    float ang = angle * (PI / 180.0f);
    float cosAngle = cos(ang);
    float sinAngle = sin(ang);

    return Matrix4x4(1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, cosAngle, sinAngle, 0.0f,
        0.0f, -sinAngle, cosAngle, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4x4 Matrix4x4::RotY(float angle)
{
    float ang = angle * (PI / 180.0f);
    float cosAngle = cos(ang);
    float sinAngle = sin(ang);

    return Matrix4x4(cosAngle, 0.0f, -sinAngle, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        sinAngle, 0.0f, cosAngle, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4x4 Matrix4x4::RotZ(float angle)
{
    float ang = angle * (PI / 180.0f);
    float cosAngle = cos(ang);
    float sinAngle = sin(ang);

    return Matrix4x4(cosAngle, sinAngle, 0.0f, 0.0f,
        -sinAngle, cosAngle, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4x4& Matrix4x4::operator+=(const Matrix4x4& matB)
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

Matrix4x4& Matrix4x4::operator-=(const Matrix4x4& matB)
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

Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& matB)
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

Matrix4x4& Matrix4x4::operator*=(float k)
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

Matrix4x4& Matrix4x4::operator/=(float k)
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

bool IsIdentity(const Matrix4x4& mat)
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

Matrix4x4 Transpose(const Matrix4x4& mat)
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

float Determinant(const Matrix4x4& mat)
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


Matrix4x4 Inverse(const Matrix4x4& mat)
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

Matrix4x4 operator+(const Matrix4x4& matA, const Matrix4x4& matB)
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

Matrix4x4 operator-(const Matrix4x4& matA, const Matrix4x4& matB)
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

Matrix4x4 operator*(const Matrix4x4& matA, const Matrix4x4& matB)
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

vec4 operator*(const Matrix4x4& mat, const vec4& vec)
{
    vec4 result;

    result.SetX(mat.mat[0][0] * vec.GetX() + mat.mat[0][1] * vec.GetY() + mat.mat[0][2] * vec.GetZ() + mat.mat[0][3] * vec.GetW());
    result.SetY(mat.mat[1][0] * vec.GetX() + mat.mat[1][1] * vec.GetY() + mat.mat[1][2] * vec.GetZ() + mat.mat[1][3] * vec.GetW());
    result.SetZ(mat.mat[2][0] * vec.GetX() + mat.mat[2][1] * vec.GetY() + mat.mat[2][2] * vec.GetZ() + mat.mat[2][3] * vec.GetW());
    result.SetW(mat.mat[3][0] * vec.GetX() + mat.mat[3][1] * vec.GetY() + mat.mat[3][2] * vec.GetZ() + mat.mat[3][3] * vec.GetW());

    return result;
}

vec4 operator*(const vec4& vec, const Matrix4x4& mat)
{
    vec4 result;

    result.SetX(vec.GetX() * mat.mat[0][0] + vec.GetY() * mat.mat[1][0] + vec.GetZ() * mat.mat[2][0] + vec.GetW() * mat.mat[3][0]);
    result.SetY(vec.GetX() * mat.mat[0][1] + vec.GetY() * mat.mat[1][1] + vec.GetZ() * mat.mat[2][1] + vec.GetW() * mat.mat[3][1]);
    result.SetZ(vec.GetX() * mat.mat[0][2] + vec.GetY() * mat.mat[1][2] + vec.GetZ() * mat.mat[2][2] + vec.GetW() * mat.mat[3][2]);
    result.SetW(vec.GetX() * mat.mat[0][3] + vec.GetY() * mat.mat[1][3] + vec.GetZ() * mat.mat[2][3] + vec.GetW() * mat.mat[3][3]);

    return result;
}

Matrix4x4 operator*(const Matrix4x4& mat, float k)
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

Matrix4x4 operator*(float k, const Matrix4x4& mat)
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

Matrix4x4 operator/(const Matrix4x4& mat, float k)
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
