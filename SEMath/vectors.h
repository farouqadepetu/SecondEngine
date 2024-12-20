#pragma once

class Vector2
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

#ifdef _DEBUG
#include <iostream>
inline void Print(const Vector2& a)
{
	std::cout << "(" << a.GetX() << ", ";
	std::cout << a.GetY() << ")" << std::endl;
}
#endif

class Vector3
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

#ifdef _DEBUG
#include <iostream>
inline void Print(const Vector3& a)
{
	std::cout << "(" << a.GetX() << ", ";
	std::cout << a.GetY() << ", ";
	std::cout << a.GetZ() << ")" << std::endl;
}
#endif

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

#ifdef _DEBUG
#include <iostream>
inline void Print(const Vector4& a)
{
	std::cout << "(" << a.GetX() << ", ";
	std::cout << a.GetY() << ", ";
	std::cout << a.GetZ() << ", ";
	std::cout << a.GetW() << ")" << std::endl;
}
#endif

typedef Vector4 vec4;
typedef Vector3 vec3;
typedef Vector2 vec2;
