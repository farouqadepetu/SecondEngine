#pragma once

#include <nmmintrin.h> //SSE 4.2

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

private:
	union
	{
		__m128 vec;
		float vecA[2];
	};
};

#ifdef _DEBUG
#include <iostream>
inline void Print(const Vector2_Intrinsics& a)
{
	std::cout << "(" << a.GetX() << ", ";
	std::cout << a.GetY() << ")" << std::endl;
}
#endif

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

#ifdef _DEBUG
#include <iostream>
inline void Print(const Vector3_Intrinsics& a)
{
	std::cout << "(" << a.GetX() << ", ";
	std::cout << a.GetY() << ", ";
	std::cout << a.GetZ() << ")" << std::endl;
}
#endif

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

#ifdef _DEBUG
#include <iostream>
inline void Print(const Vector4_Intrinsics& a)
{
	std::cout << "(" << a.GetX() << ", ";
	std::cout << a.GetY() << ", ";
	std::cout << a.GetZ() << ", ";
	std::cout << a.GetW() << ")" << std::endl;
}
#endif

typedef Vector4_Intrinsics vec4;
typedef Vector3_Intrinsics vec3;
typedef Vector2_Intrinsics vec2;