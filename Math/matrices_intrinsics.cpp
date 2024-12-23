#include <cassert>
#include "matrices_intrinsics.h"
#include "../Math/math_utility.h"

//Matrix2x2_Intrinsics IMPLEMENTATION
Matrix2x2_Intrinsics::Matrix2x2_Intrinsics()
{
    mat[0] = _mm_set_ps1(0.0f);
    mat[1] = _mm_set_ps1(0.0f);
}

Matrix2x2_Intrinsics::Matrix2x2_Intrinsics(vec2 row0, vec2 row1)
{
    SetRow(0, row0);
    SetRow(1, row1);
}

Matrix2x2_Intrinsics::Matrix2x2_Intrinsics(float m00, float m01,
                                           float m10, float m11)
{
    mat[0] = _mm_set_ps(0.0f, 0.0f, m01, m00);
    mat[1] = _mm_set_ps(0.0f, 0.0f, m11, m10);
}

float Matrix2x2_Intrinsics::GetElement(uint32_t row, uint32_t col) const
{
    assert((row < 2 && col < 2) && "Row or Col is greater than 1");

    return matA[row][col];
}

vec2 Matrix2x2_Intrinsics::GetRow(uint32_t row) const
{
    assert(row < 2 && "Row is greater than 1");

    return vec2(matA[row][0], matA[row][1]);
}

vec2 Matrix2x2_Intrinsics::GetCol(uint32_t col) const
{
    assert(col < 2 && "Col is greater than 1");

    return vec2(matA[0][col], matA[1][col]);

}

void Matrix2x2_Intrinsics::SetElement(uint32_t row, uint32_t col, float value)
{
    assert((row < 2 && col < 2) && "Row or Col is greater than 1");

    matA[row][col] = value;
}

void Matrix2x2_Intrinsics::SetRow(uint32_t row, vec2 values)
{
    assert(row < 2 && "Row is greater than 1");
    mat[row] = _mm_set_ps(0.0f, 0.0f, values.GetY(), values.GetX());
}

void Matrix2x2_Intrinsics::SetRow(uint32_t row, float val0, float val1)
{
    assert(row < 2 && "Row is greater than 1");
    mat[row] = _mm_set_ps(0.0f, 0.0f, val1, val0);
}

void Matrix2x2_Intrinsics::SetCol(uint32_t col, float val0, float val1)
{
    assert(col < 2 && "Col is greater than 1");

    matA[0][col] = val0;
    matA[1][col] = val1;
}

void Matrix2x2_Intrinsics::SetCol(uint32_t col, vec2 values)
{
    assert(col < 2 && "Col is greater than 1");

    matA[0][col] = values.GetX();
    matA[1][col] = values.GetY();
}

Matrix2x2_Intrinsics Matrix2x2_Intrinsics::MakeIdentity()
{
    return Matrix2x2_Intrinsics(1.0f, 0.0f,
                                0.0f, 1.0f);
}

Matrix2x2_Intrinsics Matrix2x2_Intrinsics::Scale(float x, float y)
{
    return Matrix2x2_Intrinsics(x, 0.0f,
                                0.0f, y);
}

Matrix2x2_Intrinsics Matrix2x2_Intrinsics::RotZ(float angle)
{
    float ang = angle * (PI / 180.0f);
    float cosAngle = cos(ang);
    float sinAngle = sin(ang);

    return Matrix2x2_Intrinsics(cosAngle, sinAngle,
                                -sinAngle, cosAngle);
}

Matrix2x2_Intrinsics& Matrix2x2_Intrinsics::operator+=(const Matrix2x2_Intrinsics& matB)
{
    this->mat[0] = _mm_add_ps(this->mat[0], matB.mat[0]);
    this->mat[1] = _mm_add_ps(this->mat[1], matB.mat[1]);
    return *this;
}

Matrix2x2_Intrinsics& Matrix2x2_Intrinsics::operator-=(const Matrix2x2_Intrinsics& matB)
{
    this->mat[0] = _mm_sub_ps(this->mat[0], matB.mat[0]);
    this->mat[1] = _mm_sub_ps(this->mat[1], matB.mat[1]);
    return *this;
}

Matrix2x2_Intrinsics& Matrix2x2_Intrinsics::operator*=(const Matrix2x2_Intrinsics& matB)
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

Matrix2x2_Intrinsics& Matrix2x2_Intrinsics::operator*=(float k)
{
    this->mat[0] = _mm_mul_ps(this->mat[0], _mm_set_ps1(k));
    this->mat[1] = _mm_mul_ps(this->mat[1], _mm_set_ps1(k));

    return *this;
}

Matrix2x2_Intrinsics& Matrix2x2_Intrinsics::operator/=(float k)
{
    this->mat[0] = _mm_div_ps(this->mat[0], _mm_set_ps1(k));
    this->mat[1] = _mm_div_ps(this->mat[1], _mm_set_ps1(k));

    return *this;
}


bool IsIdentity(const Matrix2x2_Intrinsics& mat)
{
    return
        CompareFloats(mat.matA[0][0], 1.0f) &&
        CompareFloats(mat.matA[0][1], 0.0f) &&

        CompareFloats(mat.matA[1][0], 0.0f) &&
        CompareFloats(mat.matA[1][1], 1.0f);
}

Matrix2x2_Intrinsics Transpose(const Matrix2x2_Intrinsics& mat)
{
    Matrix2x2_Intrinsics result;

    memcpy(&result, &mat, sizeof(Matrix2x2_Intrinsics));
    _MM_TRANSPOSE4_PS(result.mat[0], result.mat[1], _mm_set_ps1(0.0f), _mm_set_ps1(0.0f));

    return result;
}

float Determinant(const Matrix2x2_Intrinsics& mat)
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

Matrix2x2_Intrinsics Inverse(const Matrix2x2_Intrinsics& mat)
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

Matrix2x2_Intrinsics operator+(const Matrix2x2_Intrinsics& matA, const Matrix2x2_Intrinsics& matB)
{
    Matrix2x2_Intrinsics result;

    result.mat[0] = _mm_add_ps(matA.mat[0], matB.mat[0]);
    result.mat[1] = _mm_add_ps(matA.mat[1], matB.mat[1]);

    return result;
}

Matrix2x2_Intrinsics operator-(const Matrix2x2_Intrinsics& matA, const Matrix2x2_Intrinsics& matB)
{
    Matrix2x2_Intrinsics result;

    result.mat[0] = _mm_sub_ps(matA.mat[0], matB.mat[0]);
    result.mat[1] = _mm_sub_ps(matA.mat[1], matB.mat[1]);

    return result;
}

Matrix2x2_Intrinsics operator*(const Matrix2x2_Intrinsics& matA, const Matrix2x2_Intrinsics& matB)
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

vec2 operator*(const Matrix2x2_Intrinsics& mat, const vec2& vec)
{
    vec2 result;

    __m128 tempVec = _mm_set_ps(0.0f, 0.0f, vec.GetY(), vec.GetX());

    result.SetX(_mm_cvtss_f32(_mm_dp_ps(mat.mat[0], tempVec, 0xf1)));
    result.SetY(_mm_cvtss_f32(_mm_dp_ps(mat.mat[1], tempVec, 0xf1)));

    return result;
}

vec2 operator*(const vec2& vec, const Matrix2x2_Intrinsics& mat)
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

Matrix2x2_Intrinsics operator*(const Matrix2x2_Intrinsics& mat, float k)
{
    Matrix2x2_Intrinsics result;

    __m128 scalar = _mm_set_ps1(k);
    result.mat[0] = _mm_mul_ps(mat.mat[0], scalar);
    result.mat[1] = _mm_mul_ps(mat.mat[1], scalar);

    return result;
}

Matrix2x2_Intrinsics operator*(float k, const Matrix2x2_Intrinsics& mat)
{
    Matrix2x2_Intrinsics result;

    __m128 scalar = _mm_set_ps1(k);
    result.mat[0] = _mm_mul_ps(scalar, mat.mat[0]);
    result.mat[1] = _mm_mul_ps(scalar, mat.mat[1]);

    return result;
}

Matrix2x2_Intrinsics operator/(const Matrix2x2_Intrinsics& mat, float k)
{
    Matrix2x2_Intrinsics result;

    __m128 scalar = _mm_set_ps1(k);
    result.mat[0] = _mm_div_ps(mat.mat[0], scalar);
    result.mat[1] = _mm_div_ps(mat.mat[1], scalar);

    return result;
}

//Matrix3x3_Intrinsics IMPLEMENTATION
Matrix3x3_Intrinsics::Matrix3x3_Intrinsics()
{
    mat[0] = _mm_set_ps1(0.0f);
    mat[1] = _mm_set_ps1(0.0f);
    mat[2] = _mm_set_ps1(0.0f);
}

Matrix3x3_Intrinsics::Matrix3x3_Intrinsics(vec3 row0, vec3 row1, vec3 row2)
{
    SetRow(0, row0);
    SetRow(1, row1);
    SetRow(2, row2);
}

Matrix3x3_Intrinsics::Matrix3x3_Intrinsics(float m00, float m01, float m02,
                                           float m10, float m11, float m12,
                                           float m20, float m21, float m22)
{
    mat[0] = _mm_set_ps(0.0f, m02, m01, m00);
    mat[1] = _mm_set_ps(0.0f, m12, m11, m10);
    mat[2] = _mm_set_ps(0.0f, m22, m21, m20);
}

float Matrix3x3_Intrinsics::GetElement(uint32_t row, uint32_t col) const
{
    assert((row < 3 && col < 3) && "Row or Col is greater than 2");

    return matA[row][col];
}

vec3 Matrix3x3_Intrinsics::GetRow(uint32_t row) const
{
    assert(row < 3 && "Row is greater than 2");

    return vec3(matA[row][0], matA[row][1], matA[row][2]);
}

vec3 Matrix3x3_Intrinsics::GetCol(uint32_t col) const
{
    assert(col < 3 && "Col is greater than 2");

    return vec3(matA[0][col], matA[1][col], matA[2][col]);

}

void Matrix3x3_Intrinsics::SetElement(uint32_t row, uint32_t col, float value)
{
    assert((row < 3 && col < 3) && "Row or Col is greater than 2");

    matA[row][col] = value;
}

void Matrix3x3_Intrinsics::SetRow(uint32_t row, vec3 values)
{
    assert(row < 3 && "Row is greater than 2");
    mat[row] = _mm_set_ps(0.0f, values.GetZ(), values.GetY(), values.GetX());
}

void Matrix3x3_Intrinsics::SetRow(uint32_t row, float val0, float val1, float val2)
{
    assert(row < 3 && "Row is greater than 2");
    mat[row] = _mm_set_ps(0.0f, val2, val1, val0);
}

void Matrix3x3_Intrinsics::SetCol(uint32_t col, float val0, float val1, float val2)
{
    assert(col < 3 && "Col is greater than 2");

    matA[0][col] = val0;
    matA[1][col] = val1;
    matA[2][col] = val2;
}

void Matrix3x3_Intrinsics::SetCol(uint32_t col, vec3 values)
{
    assert(col < 3 && "Col is greater than 2");

    matA[0][col] = values.GetX();
    matA[1][col] = values.GetY();
    matA[2][col] = values.GetZ();
}

Matrix3x3_Intrinsics Matrix3x3_Intrinsics::MakeIdentity()
{
    return Matrix3x3_Intrinsics(1.0f, 0.0f, 0.0f,
                                0.0f, 1.0f, 0.0f,
                                0.0f, 0.0f, 1.0f);
}

Matrix3x3_Intrinsics Matrix3x3_Intrinsics::Scale(float x, float y, float z)
{
    return Matrix3x3_Intrinsics(x, 0.0f, 0.0f,
                                0.0f, y, 0.0f,
                                0.0f, 0.0f, z);
}

Matrix3x3_Intrinsics Matrix3x3_Intrinsics::Translate(float x, float y)
{
    return Matrix3x3_Intrinsics(1.0f, 0.0f, 0.0f,
                                0.0f, 1.0f, 0.0f,
                                x, y, 1.0f);
}

Matrix3x3_Intrinsics Matrix3x3_Intrinsics::RotX(float angle)
{
    float ang = angle * (PI / 180.0f);
    float cosAngle = cos(ang);
    float sinAngle = sin(ang);

    return Matrix3x3_Intrinsics(1.0f, 0.0f, 0.0f,
                                0.0f, cosAngle, sinAngle,
                                0.0f, -sinAngle, cosAngle);
}

Matrix3x3_Intrinsics Matrix3x3_Intrinsics::RotY(float angle)
{
    float ang = angle * (PI / 180.0f);
    float cosAngle = cos(ang);
    float sinAngle = sin(ang);

    return Matrix3x3_Intrinsics(cosAngle, 0.0f, -sinAngle,
                                0.0f, 1.0f, 0.0f,
                                sinAngle, 0.0f, cosAngle);
}

Matrix3x3_Intrinsics Matrix3x3_Intrinsics::RotZ(float angle)
{
    float ang = angle * (PI / 180.0f);
    float cosAngle = cos(ang);
    float sinAngle = sin(ang);

    return Matrix3x3_Intrinsics(cosAngle, sinAngle, 0.0f,
                                -sinAngle, cosAngle, 0.0f,
                                0.0f, 0.0f, 1.0f);
}

Matrix3x3_Intrinsics& Matrix3x3_Intrinsics::operator+=(const Matrix3x3_Intrinsics& matB)
{
    this->mat[0] = _mm_add_ps(this->mat[0], matB.mat[0]);
    this->mat[1] = _mm_add_ps(this->mat[1], matB.mat[1]);
    this->mat[2] = _mm_add_ps(this->mat[2], matB.mat[2]);
    return *this;
}

Matrix3x3_Intrinsics& Matrix3x3_Intrinsics::operator-=(const Matrix3x3_Intrinsics& matB)
{
    this->mat[0] = _mm_sub_ps(this->mat[0], matB.mat[0]);
    this->mat[1] = _mm_sub_ps(this->mat[1], matB.mat[1]);
    this->mat[2] = _mm_sub_ps(this->mat[2], matB.mat[2]);
    return *this;
}

Matrix3x3_Intrinsics& Matrix3x3_Intrinsics::operator*=(const Matrix3x3_Intrinsics& matB)
{
    __m128 b0 = matB.mat[0];
    __m128 b1 = matB.mat[1];
    __m128 b2 = matB.mat[2];
    __m128 b3 = _mm_set_ps1(0.0f);

    _MM_TRANSPOSE4_PS(b0, b1, b2, b3);

    this->mat[0] =  _mm_set_ps(
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

Matrix3x3_Intrinsics& Matrix3x3_Intrinsics::operator*=(float k)
{
    this->mat[0] = _mm_mul_ps(this->mat[0], _mm_set_ps1(k));
    this->mat[1] = _mm_mul_ps(this->mat[1], _mm_set_ps1(k));
    this->mat[2] = _mm_mul_ps(this->mat[2], _mm_set_ps1(k));

    return *this;
}

Matrix3x3_Intrinsics& Matrix3x3_Intrinsics::operator/=(float k)
{
    this->mat[0] = _mm_div_ps(this->mat[0], _mm_set_ps1(k));
    this->mat[1] = _mm_div_ps(this->mat[1], _mm_set_ps1(k));
    this->mat[2] = _mm_div_ps(this->mat[2], _mm_set_ps1(k));

    return *this;
}


bool IsIdentity(const Matrix3x3_Intrinsics& mat)
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

Matrix3x3_Intrinsics Transpose(const Matrix3x3_Intrinsics& mat)
{
    Matrix3x3_Intrinsics result;

    memcpy(&result, &mat, sizeof(Matrix3x3_Intrinsics));
    _MM_TRANSPOSE4_PS(result.mat[0], result.mat[1], result.mat[2], _mm_set_ps1(0.0f));

    return result;
}

float Determinant(const Matrix3x3_Intrinsics& mat)
{
   float t0 = mat.matA[0][0] * Determinant(Matrix2x2_Intrinsics(mat.matA[1][1], mat.matA[1][2], mat.matA[2][1], mat.matA[2][2]));
   float t1 = -mat.matA[0][1] * Determinant(Matrix2x2_Intrinsics(mat.matA[1][0], mat.matA[1][2], mat.matA[2][0], mat.matA[2][2]));
   float t2 = mat.matA[0][2] * Determinant(Matrix2x2_Intrinsics(mat.matA[1][0], mat.matA[1][1], mat.matA[2][0], mat.matA[2][1]));

   return t0 + t1 + t2;
}

Matrix3x3_Intrinsics Inverse(const Matrix3x3_Intrinsics& mat)
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

Matrix3x3_Intrinsics operator+(const Matrix3x3_Intrinsics& matA, const Matrix3x3_Intrinsics& matB)
{
    Matrix3x3_Intrinsics result;

    result.mat[0] = _mm_add_ps(matA.mat[0], matB.mat[0]);
    result.mat[1] = _mm_add_ps(matA.mat[1], matB.mat[1]);
    result.mat[2] = _mm_add_ps(matA.mat[2], matB.mat[2]);

    return result;
}

Matrix3x3_Intrinsics operator-(const Matrix3x3_Intrinsics& matA, const Matrix3x3_Intrinsics& matB)
{
    Matrix3x3_Intrinsics result;

    result.mat[0] = _mm_sub_ps(matA.mat[0], matB.mat[0]);
    result.mat[1] = _mm_sub_ps(matA.mat[1], matB.mat[1]);
    result.mat[2] = _mm_sub_ps(matA.mat[2], matB.mat[2]);

    return result;
}

Matrix3x3_Intrinsics operator*(const Matrix3x3_Intrinsics& matA, const Matrix3x3_Intrinsics& matB)
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

vec3 operator*(const Matrix3x3_Intrinsics& mat, const vec3& vec)
{
    vec3 result;

    __m128 tempVec = _mm_set_ps(0.0f, vec.GetZ(), vec.GetY(), vec.GetX());

    result.SetX(_mm_cvtss_f32(_mm_dp_ps(mat.mat[0], tempVec, 0xf1)));
    result.SetY(_mm_cvtss_f32(_mm_dp_ps(mat.mat[1], tempVec, 0xf1)));
    result.SetZ(_mm_cvtss_f32(_mm_dp_ps(mat.mat[2], tempVec, 0xf1)));

    return result;
}

vec3 operator*(const vec3& vec, const Matrix3x3_Intrinsics& mat)
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

Matrix3x3_Intrinsics operator*(const Matrix3x3_Intrinsics& mat, float k)
{
    Matrix3x3_Intrinsics result;

    __m128 scalar = _mm_set_ps1(k);
    result.mat[0] = _mm_mul_ps(mat.mat[0], scalar);
    result.mat[1] = _mm_mul_ps(mat.mat[1], scalar);
    result.mat[2] = _mm_mul_ps(mat.mat[2], scalar);

    return result;
}

Matrix3x3_Intrinsics operator*(float k, const Matrix3x3_Intrinsics& mat)
{
    Matrix3x3_Intrinsics result;

    __m128 scalar = _mm_set_ps1(k);
    result.mat[0] = _mm_mul_ps(scalar, mat.mat[0]);
    result.mat[1] = _mm_mul_ps(scalar, mat.mat[1]);
    result.mat[2] = _mm_mul_ps(scalar, mat.mat[2]);

    return result;
}

Matrix3x3_Intrinsics operator/(const Matrix3x3_Intrinsics& mat, float k)
{
    Matrix3x3_Intrinsics result;

    __m128 scalar = _mm_set_ps1(k);
    result.mat[0] = _mm_div_ps(mat.mat[0], scalar);
    result.mat[1] = _mm_div_ps(mat.mat[1], scalar);
    result.mat[2] = _mm_div_ps(mat.mat[2], scalar);

    return result;
}


//MATRIX4X4_INTRINSICS IMPLEMENTATION
Matrix4x4_Intrinsics::Matrix4x4_Intrinsics()
{
    mat[0] = _mm_set_ps1(0.0f);
    mat[1] = _mm_set_ps1(0.0f);
    mat[2] = _mm_set_ps1(0.0f);
    mat[3] = _mm_set_ps1(0.0f);
}

Matrix4x4_Intrinsics::Matrix4x4_Intrinsics(vec4 row0, vec4 row1, vec4 row2, vec4 row3)
{
    SetRow(0, row0);
    SetRow(1, row1);
    SetRow(2, row2);
    SetRow(3, row3);
}

Matrix4x4_Intrinsics::Matrix4x4_Intrinsics(float m00, float m01, float m02, float m03,
                                           float m10, float m11, float m12, float m13,
                                           float m20, float m21, float m22, float m23,
                                           float m30, float m31, float m32, float m33)
{
    mat[0] = _mm_set_ps(m03, m02, m01, m00);
    mat[1] = _mm_set_ps(m13, m12, m11, m10);
    mat[2] = _mm_set_ps(m23, m22, m21, m20);
    mat[3] = _mm_set_ps(m33, m32, m31, m30);
}

float Matrix4x4_Intrinsics::GetElement(uint32_t row, uint32_t col) const
{
    assert((row < 4 && col < 4) && "Row or Col is greater than 3");

    return matA[row][col];
}

vec4 Matrix4x4_Intrinsics::GetRow(uint32_t row) const
{
    assert(row < 4 && "Row is greater than 3");

    return vec4(matA[row][0], matA[row][1], matA[row][2], matA[row][3]);
}

vec4 Matrix4x4_Intrinsics::GetCol(uint32_t col) const
{
    assert(col < 4 && "Col is greater than 3");

    return vec4(matA[0][col], matA[1][col], matA[2][col], matA[3][col]);
  
}

void Matrix4x4_Intrinsics::SetElement(uint32_t row, uint32_t col, float value)
{
    assert((row < 4 && col < 4) && "Row or Col is greater than 3");

    matA[row][col] = value;
}

void Matrix4x4_Intrinsics::SetRow(uint32_t row, vec4 values)
{
    assert(row < 4 && "Row is greater than 3");
    mat[row] = _mm_set_ps(values.GetW(), values.GetZ(), values.GetY(), values.GetX());
}

void Matrix4x4_Intrinsics::SetRow(uint32_t row, float val0, float val1, float val2, float val3)
{
    assert(row < 4 && "Row is greater than 3");
    mat[row] = _mm_set_ps(val3, val2, val1, val0);
}

void Matrix4x4_Intrinsics::SetCol(uint32_t col, float val0, float val1, float val2, float val3)
{
    assert(col < 4 && "Col is greater than 3");

    matA[0][col] = val0;
    matA[1][col] = val1;
    matA[2][col] = val2;
    matA[3][col] = val3;
}

void Matrix4x4_Intrinsics::SetCol(uint32_t col, vec4 values)
{
    assert(col < 4 && "Col is greater than 3");

    matA[0][col] = values.GetX();
    matA[1][col] = values.GetY();
    matA[2][col] = values.GetZ();
    matA[3][col] = values.GetW();
}

Matrix4x4_Intrinsics Matrix4x4_Intrinsics::MakeIdentity()
{
    return Matrix4x4_Intrinsics(1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4x4_Intrinsics Matrix4x4_Intrinsics::Scale(float x, float y, float z)
{
    return Matrix4x4_Intrinsics(x, 0.0f, 0.0f, 0.0f,
        0.0f, y, 0.0f, 0.0f,
        0.0f, 0.0f, z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4x4_Intrinsics Matrix4x4_Intrinsics::Translate(float x, float y, float z)
{
    return Matrix4x4_Intrinsics(1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        x, y, z, 1.0f);
}

Matrix4x4_Intrinsics Matrix4x4_Intrinsics::RotX(float angle)
{
    float ang = angle * (PI / 180.0f);
    float cosAngle = cos(ang);
    float sinAngle = sin(ang);

    return Matrix4x4_Intrinsics(1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, cosAngle, sinAngle, 0.0f,
        0.0f, -sinAngle, cosAngle, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4x4_Intrinsics Matrix4x4_Intrinsics::RotY(float angle)
{
    float ang = angle * (PI / 180.0f);
    float cosAngle = cos(ang);
    float sinAngle = sin(ang);

    return Matrix4x4_Intrinsics(cosAngle, 0.0f, -sinAngle, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        sinAngle, 0.0f, cosAngle, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4x4_Intrinsics Matrix4x4_Intrinsics::RotZ(float angle)
{
    float ang = angle * (PI / 180.0f);
    float cosAngle = cos(ang);
    float sinAngle = sin(ang);

    return Matrix4x4_Intrinsics(cosAngle, sinAngle, 0.0f, 0.0f,
        -sinAngle, cosAngle, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
}

Matrix4x4_Intrinsics& Matrix4x4_Intrinsics::operator+=(const Matrix4x4_Intrinsics& matB)
{
    this->mat[0] = _mm_add_ps(this->mat[0], matB.mat[0]);
    this->mat[1] = _mm_add_ps(this->mat[1], matB.mat[1]);
    this->mat[2] = _mm_add_ps(this->mat[2], matB.mat[2]);
    this->mat[3] = _mm_add_ps(this->mat[3], matB.mat[3]);
    return *this;
}

Matrix4x4_Intrinsics& Matrix4x4_Intrinsics::operator-=(const Matrix4x4_Intrinsics& matB)
{
    this->mat[0] = _mm_sub_ps(this->mat[0], matB.mat[0]);
    this->mat[1] = _mm_sub_ps(this->mat[1], matB.mat[1]);
    this->mat[2] = _mm_sub_ps(this->mat[2], matB.mat[2]);
    this->mat[3] = _mm_sub_ps(this->mat[3], matB.mat[3]);
    return *this;
}

Matrix4x4_Intrinsics& Matrix4x4_Intrinsics::operator*=(const Matrix4x4_Intrinsics& matB)
{
    __m128 b0 = matB.mat[0];
    __m128 b1 = matB.mat[1];
    __m128 b2 = matB.mat[2];
    __m128 b3 = matB.mat[3];

    _MM_TRANSPOSE4_PS(b0, b1, b2, b3);

    this->mat[0] =  _mm_set_ps(
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

Matrix4x4_Intrinsics& Matrix4x4_Intrinsics::operator*=(float k)
{
    this->mat[0] = _mm_mul_ps(this->mat[0], _mm_set_ps1(k));
    this->mat[1] = _mm_mul_ps(this->mat[1], _mm_set_ps1(k));
    this->mat[2] = _mm_mul_ps(this->mat[2], _mm_set_ps1(k));
    this->mat[3] = _mm_mul_ps(this->mat[3], _mm_set_ps1(k));

    return *this;
}

Matrix4x4_Intrinsics& Matrix4x4_Intrinsics::operator/=(float k)
{
    this->mat[0] = _mm_div_ps(this->mat[0], _mm_set_ps1(k));
    this->mat[1] = _mm_div_ps(this->mat[1], _mm_set_ps1(k));
    this->mat[2] = _mm_div_ps(this->mat[2], _mm_set_ps1(k));
    this->mat[3] = _mm_div_ps(this->mat[3], _mm_set_ps1(k));

    return *this;
}


bool IsIdentity(const Matrix4x4_Intrinsics& mat)
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

Matrix4x4_Intrinsics Transpose(const Matrix4x4_Intrinsics& mat)
{
    Matrix4x4_Intrinsics result;

    memcpy((void*)&result, (void*)&mat, sizeof(Matrix4x4_Intrinsics));
    _MM_TRANSPOSE4_PS(result.mat[0], result.mat[1], result.mat[2], result.mat[3]);

    return result;
}

float Determinant(const Matrix4x4_Intrinsics& mat)
{
    float c0 =  mat.matA[0][0] * Determinant(Matrix3x3_Intrinsics(mat.matA[1][1], mat.matA[1][2], mat.matA[1][3],
                                                                  mat.matA[2][1], mat.matA[2][2], mat.matA[2][3],
                                                                  mat.matA[3][1], mat.matA[3][2], mat.matA[3][3]));

    float c1 = -mat.matA[0][1] * Determinant(Matrix3x3_Intrinsics(mat.matA[1][0], mat.matA[1][2], mat.matA[1][3],
                                                                  mat.matA[2][0], mat.matA[2][2], mat.matA[2][3],
                                                                  mat.matA[3][0], mat.matA[3][2], mat.matA[3][3]));

    float c2 = mat.matA[0][2] * Determinant(Matrix3x3_Intrinsics(mat.matA[1][0], mat.matA[1][1], mat.matA[1][3],
                                                                 mat.matA[2][0], mat.matA[2][1], mat.matA[2][3],
                                                                 mat.matA[3][0], mat.matA[3][1], mat.matA[3][3]));

    float c3 = -mat.matA[0][3] *  Determinant(Matrix3x3_Intrinsics(mat.matA[1][0], mat.matA[1][1], mat.matA[1][2],
                                                                   mat.matA[2][0], mat.matA[2][1], mat.matA[2][2],
                                                                   mat.matA[3][0], mat.matA[3][1], mat.matA[3][2]));

    return c0 + c1 + c2 + c3;
}

Matrix4x4_Intrinsics Inverse(const Matrix4x4_Intrinsics& mat)
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

Matrix4x4_Intrinsics operator+(const Matrix4x4_Intrinsics& matA, const Matrix4x4_Intrinsics& matB)
{
    Matrix4x4_Intrinsics result;

    result.mat[0] = _mm_add_ps(matA.mat[0], matB.mat[0]);
    result.mat[1] = _mm_add_ps(matA.mat[1], matB.mat[1]);
    result.mat[2] = _mm_add_ps(matA.mat[2], matB.mat[2]);
    result.mat[3] = _mm_add_ps(matA.mat[3], matB.mat[3]);

    return result;
}

Matrix4x4_Intrinsics operator-(const Matrix4x4_Intrinsics& matA, const Matrix4x4_Intrinsics& matB)
{
    Matrix4x4_Intrinsics result;

    result.mat[0] = _mm_sub_ps(matA.mat[0], matB.mat[0]);
    result.mat[1] = _mm_sub_ps(matA.mat[1], matB.mat[1]);
    result.mat[2] = _mm_sub_ps(matA.mat[2], matB.mat[2]);
    result.mat[3] = _mm_sub_ps(matA.mat[3], matB.mat[3]);

    return result;
}

Matrix4x4_Intrinsics operator*(const Matrix4x4_Intrinsics& matA, const Matrix4x4_Intrinsics& matB)
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

vec4 operator*(const Matrix4x4_Intrinsics& mat, const vec4& vec)
{
    vec4 result;

    __m128 tempVec = _mm_set_ps(vec.GetW(), vec.GetZ(), vec.GetY(), vec.GetX());

    result.SetX(_mm_cvtss_f32(_mm_dp_ps(mat.mat[0], tempVec, 0xf1)));
    result.SetY(_mm_cvtss_f32(_mm_dp_ps(mat.mat[1], tempVec, 0xf1)));
    result.SetZ(_mm_cvtss_f32(_mm_dp_ps(mat.mat[2], tempVec, 0xf1)));
    result.SetW(_mm_cvtss_f32(_mm_dp_ps(mat.mat[3], tempVec, 0xf1)));

    return result;
}

vec4 operator*(const vec4& vec, const Matrix4x4_Intrinsics& mat)
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

Matrix4x4_Intrinsics operator*(const Matrix4x4_Intrinsics& mat, float k)
{
    Matrix4x4_Intrinsics result;

    __m128 scalar = _mm_set_ps1(k);
    result.mat[0] = _mm_mul_ps(mat.mat[0], scalar);
    result.mat[1] = _mm_mul_ps(mat.mat[1], scalar);
    result.mat[2] = _mm_mul_ps(mat.mat[2], scalar);
    result.mat[3] = _mm_mul_ps(mat.mat[3], scalar);

    return result;
}

Matrix4x4_Intrinsics operator*(float k, const Matrix4x4_Intrinsics& mat)
{
    Matrix4x4_Intrinsics result;

    __m128 scalar = _mm_set_ps1(k);
    result.mat[0] = _mm_mul_ps(scalar, mat.mat[0]);
    result.mat[1] = _mm_mul_ps(scalar, mat.mat[1]);
    result.mat[2] = _mm_mul_ps(scalar, mat.mat[2]);
    result.mat[3] = _mm_mul_ps(scalar, mat.mat[3]);

    return result;
}

Matrix4x4_Intrinsics operator/(const Matrix4x4_Intrinsics& mat, float k)
{
    Matrix4x4_Intrinsics result;

    __m128 scalar = _mm_set_ps1(k);
    result.mat[0] = _mm_div_ps(mat.mat[0], scalar);
    result.mat[1] = _mm_div_ps(mat.mat[1], scalar);
    result.mat[2] = _mm_div_ps(mat.mat[2], scalar);
    result.mat[3] = _mm_div_ps(mat.mat[3], scalar);

    return result;
}
