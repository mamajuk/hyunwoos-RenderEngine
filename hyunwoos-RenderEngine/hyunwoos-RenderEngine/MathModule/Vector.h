#pragma once
#include <cmath>
#include <string>
#include "Math.h"

namespace hyunwoo {
	struct Vector2;
	struct Vector2Int;

	struct Vector3;
	struct Vector3Int;

	struct Vector4;
	struct Vector4Int;
}



/*================================================================================
 *   2차원 벡터가 구현된 구조체입니다...
 *=========*/
struct hyunwoo::Vector2
{
	//============================================================
	///////////				  Property...			   ///////////
	//============================================================
	float x, y;

	static const Vector2 Zero;
	static const Vector2 One;
	static const Vector2 BasisX;
	static const Vector2 BasisY;
	static const Vector2 Right;
	static const Vector2 Left;
	static const Vector2 Up;
	static const Vector2 Down;


	
	//===========================================================
	//////////			Constructor methods...			/////////
	//===========================================================
	constexpr Vector2(float x = 0.f, float y = 0.f) :x(x), y(y) {}
	constexpr Vector2(const Vector2&) = default;
	constexpr ~Vector2()              = default;



	//=============================================================
	//////////			   Public methods..				  /////////
	//=============================================================
	constexpr static const float Dot(const Vector2& lhs, const Vector2& rhs) {
		return ((lhs.x*rhs.x) + (lhs.y*rhs.y));
	}

	constexpr const float GetSqrMagnitude() const {
		return Dot(*this, *this);
	}

	const float GetMagnitude() const;

	const Vector2 GetNormalized() const {
		const float size = GetMagnitude();
		if (size == 0.0f) return Vector2::Zero;

		const float invSize = (1.f / size);
		return Vector2((x*invSize), (y*invSize));
	}

	void Normalized() {
		const float size = GetMagnitude();
		if (size == 0.0f) return;;

		const float invSize = (1.f / size);
		x *= invSize;
		y *= invSize;
	}

	void AddString(std::string& sharedStr) const {
		sharedStr += "(";
		sharedStr += std::to_string(x);
		sharedStr += ", ";
		sharedStr += std::to_string(y);
		sharedStr += ")";
	}

	void AddWString(std::wstring& sharedStr) const {
		sharedStr += L"(";
		sharedStr += std::to_wstring(x);
		sharedStr += L", ";
		sharedStr += std::to_wstring(y);
		sharedStr += L")";
	}



	//===========================================================
	/////////			Operator methods...				/////////
	//===========================================================
	constexpr const bool operator==(const Vector2& rhs) const {
		return ((x==rhs.x) && (y==rhs.y));
	}

	constexpr const Vector2 operator+(const Vector2& rhs) const {
		return Vector2((x+rhs.x), (y+rhs.y));
	}

	constexpr const Vector2 operator-(const Vector2& rhs) const {
		return Vector2((x-rhs.x), (y-rhs.y));
	}

	constexpr const Vector2 operator*(const Vector2& rhs) const {
		return Vector2((x*rhs.x), (y*rhs.y));
	}

	constexpr const Vector2 operator*(const float scalar) const {
		return Vector2((x * scalar), (y * scalar));
	}

	Vector2& operator+=(const Vector2& rhs) {
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

	Vector2& operator-=(const Vector2& rhs) {
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}

	Vector2& operator*=(const Vector2& rhs) {
		x *= rhs.x;
		y *= rhs.y;
	}

	Vector2& operator*=(const float scalar) {
		x *= scalar;
		y *= scalar;
	}

	operator Vector3() const;
	operator Vector4() const;

	operator Vector2Int() const;
	operator Vector3Int() const;
	operator Vector4Int() const;
};

/**정수버전....**/
struct hyunwoo::Vector2Int
{
	//============================================================
	///////////				  Property...			   ///////////
	//============================================================
	int x, y;

	static const Vector2Int Zero;
	static const Vector2Int One;
	static const Vector2Int BasisX;
	static const Vector2Int BasisY;
	static const Vector2Int Right;
	static const Vector2Int Left;
	static const Vector2Int Up;
	static const Vector2Int Down;



	//===========================================================
	//////////			Constructor methods...			/////////
	//===========================================================
	constexpr Vector2Int(int x = 0, int y = 0) :x(x), y(y) {}
	constexpr Vector2Int(const Vector2Int&) = default;
	constexpr ~Vector2Int()				    = default;



	//=============================================================
	//////////			   Public methods..				  /////////
	//=============================================================
	constexpr static const int Dot(const Vector2Int& lhs, const Vector2Int& rhs) {
		return ((lhs.x * rhs.x) + (lhs.y * rhs.y));
	}

	constexpr const int GetSqrMagnitude() const {
		return Dot(*this, *this);
	}

	const int GetMagnitude() const;

	const Vector2Int GetNormalized() const {
		const int size = GetMagnitude();
		if (size == 0) return Vector2Int::Zero;

		return Vector2Int((x / size), (y / size));
	}

	void Normalized() {
		const float size = GetMagnitude();
		if (size == 0.0f) return;;

		x /= size;
		y /= size;
	}

	void AddString(std::string& sharedStr) const {
		sharedStr += "(";
		sharedStr += std::to_string(x);
		sharedStr += ", ";
		sharedStr += std::to_string(y);
		sharedStr += ")";
	}

	void AddWString(std::wstring& sharedStr) const {
		sharedStr += L"(";
		sharedStr += std::to_wstring(x);
		sharedStr += L", ";
		sharedStr += std::to_wstring(y);
		sharedStr += L")";
	}



	//===========================================================
	/////////			Operator methods...				/////////
	//===========================================================
	constexpr const bool operator==(const Vector2Int& rhs) const {
		return ((x == rhs.x) && (y == rhs.y));
	}

	constexpr const Vector2Int operator+(const Vector2Int& rhs) const {
		return Vector2Int((x + rhs.x), (y + rhs.y));
	}

	constexpr const Vector2Int operator-(const Vector2Int& rhs) const {
		return Vector2Int((x - rhs.x), (y - rhs.y));
	}

	constexpr const Vector2Int operator*(const Vector2Int& rhs) const {
		return Vector2Int((x * rhs.x), (y * rhs.y));
	}

	constexpr const Vector2Int operator*(const int scalar) const {
		return Vector2Int((x * scalar), (y * scalar));
	}

	Vector2Int& operator+=(const Vector2Int& rhs) {
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

	Vector2Int& operator-=(const Vector2Int& rhs) {
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}

	Vector2Int& operator*=(const Vector2Int& rhs) {
		x *= rhs.x;
		y *= rhs.y;
	}

	Vector2Int& operator*=(const int scalar) {
		x *= scalar;
		y *= scalar;
	}

	operator Vector2() const;
	operator Vector3() const;
	operator Vector4() const;
	
	operator Vector3Int() const;
	operator Vector4Int() const;
};




/*=========================================================================
 *   3차원 벡터가 구현된 구조체입니다...
 *============*/
struct hyunwoo::Vector3
{
	//=======================================================
	/////////				Property...				/////////
	//=======================================================
	float x, y, z;

	static const Vector3 Zero;
	static const Vector3 One;
	static const Vector3 BasisX;
	static const Vector3 BasisY;
	static const Vector3 BasisZ;
	static const Vector3 Right;
	static const Vector3 Left;
	static const Vector3 Up;
	static const Vector3 Down;
	static const Vector3 Forward;
	static const Vector3 Back;



	//===========================================================
	//////////			Constructor methods...			/////////
	//===========================================================
	constexpr Vector3(float x = 0.f, float y = 0.f, float z = 0.f) :x(x), y(y), z(z) {}
	constexpr Vector3(const Vector3&) = default;
	constexpr ~Vector3() = default;




	//=============================================================
	//////////			   Public methods..				  /////////
	//=============================================================
	constexpr static const float Dot(const Vector3& lhs, const Vector3& rhs) {
		return ((lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z));
	}

	constexpr static const Vector3 Cross(const Vector3& lhs, const Vector3& rhs) {
		return { (lhs.y * rhs.z) - (lhs.z * rhs.y),
				 (lhs.z * rhs.x) - (lhs.x * rhs.z),
				 (lhs.x * rhs.y) - (lhs.y * rhs.x) };
	}

	constexpr const float GetSqrMagnitude() const {
		return Dot(*this, *this);
	}

	const float GetMagnitude() const;

	const Vector3 GetNormalized() const {
		const float size = GetMagnitude();
		if (size == 0.0f) return Vector3::Zero;

		const float invSize = (1.f / size);
		return Vector3((x * invSize), (y * invSize), (z*invSize));
	}

	void Normalized() {
		const float size = GetMagnitude();
		if (size == 0.0f) return;

		const float invSize = (1.f / size);
		x *= invSize;
		y *= invSize;
		z *= invSize;
	}

	void AddString(std::string& sharedStr) const {
		sharedStr += "(";
		sharedStr += std::to_string(x);
		sharedStr += ", ";
		sharedStr += std::to_string(y);
		sharedStr += ", ";
		sharedStr += std::to_string(z);
		sharedStr += ")";
	}

	void AddWString(std::wstring& sharedStr) const {
		sharedStr += L"(";
		sharedStr += std::to_wstring(x);
		sharedStr += L", ";
		sharedStr += std::to_wstring(y);
		sharedStr += L", ";
		sharedStr += std::to_wstring(z);
		sharedStr += L")";
	}



	//===========================================================
	/////////			Operator methods...				/////////
	//===========================================================
	constexpr const bool operator==(const Vector3& rhs) const {
		return ((x == rhs.x) && (y == rhs.y) && (z==rhs.z));
	}

	constexpr const Vector3 operator+(const Vector3& rhs) const {
		return Vector3((x + rhs.x), (y + rhs.y), (z + rhs.z));
	}

	constexpr const Vector3 operator-(const Vector3& rhs) const {
		return Vector3((x - rhs.x), (y - rhs.y), (z - rhs.z));
	}

	constexpr const Vector3 operator*(const Vector3& rhs) const {
		return Vector3((x*rhs.x), (y*rhs.y), (z*rhs.z));
	}

	constexpr const Vector3 operator*(const float scalar) const {
		return Vector3((x*scalar), (y*scalar), (z*scalar));
	}

	Vector3& operator+=(const Vector3& rhs) {
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}

	Vector3& operator-=(const Vector3& rhs) {
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}

	Vector3& operator*=(const Vector3& rhs) {
		x *= rhs.x;
		y *= rhs.y;
		z *= rhs.z;
		return *this;
	}

	Vector3& operator*=(const float scalar) {
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	operator Vector2() const;
	operator Vector4() const;
	
	operator Vector2Int() const;
	operator Vector3Int() const;
	operator Vector4Int() const;
};

/**정수버전...**/
struct hyunwoo::Vector3Int
{
	//=======================================================
	/////////				Property...				/////////
	//=======================================================
	int x, y, z;

	static const Vector3Int Zero;
	static const Vector3Int One;
	static const Vector3Int BasisX;
	static const Vector3Int BasisY;
	static const Vector3Int BasisZ;
	static const Vector3Int Right;
	static const Vector3Int Left;
	static const Vector3Int Up;
	static const Vector3Int Down;
	static const Vector3Int Forward;
	static const Vector3Int Back;



	//===========================================================
	//////////			Constructor methods...			/////////
	//===========================================================
	constexpr Vector3Int(int x = 0, int y = 0, int z = 0) :x(x), y(y), z(z) {}
	constexpr Vector3Int(const Vector3Int&) = default;
	constexpr ~Vector3Int() = default;




	//=============================================================
	//////////			   Public methods..				  /////////
	//=============================================================
	constexpr static const int Dot(const Vector3Int& lhs, const Vector3Int& rhs) {
		return ((lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z));
	}

	constexpr static const Vector3Int Cross(const Vector3Int& lhs, const Vector3Int& rhs) {
		return { (lhs.y * rhs.z) - (lhs.z * rhs.y),
				 (lhs.z * rhs.x) - (lhs.x * rhs.z),
				 (lhs.x * rhs.y) - (lhs.y * rhs.x) };
	}

	constexpr const int GetSqrMagnitude() const {
		return Dot(*this, *this);
	}

	const int GetMagnitude() const;

	const Vector3Int GetNormalized() const {
		const int size = GetMagnitude();
		if (size == 0) return Vector3Int::Zero;

		return Vector3Int((x / size), (y / size), (z / size));
	}

	void Normalized() {
		const int size = GetMagnitude();
		if (size == 0) return;

		x /= size;
		y /= size;
		z /= size;
	}

	void AddString(std::string& sharedStr) const {
		sharedStr += "(";
		sharedStr += std::to_string(x);
		sharedStr += ", ";
		sharedStr += std::to_string(y);
		sharedStr += ", ";
		sharedStr += std::to_string(z);
		sharedStr += ")";
	}

	void AddWString(std::wstring& sharedStr) const {
		sharedStr += L"(";
		sharedStr += std::to_wstring(x);
		sharedStr += L", ";
		sharedStr += std::to_wstring(y);
		sharedStr += L", ";
		sharedStr += std::to_wstring(z);
		sharedStr += L")";
	}



	//===========================================================
	/////////			Operator methods...				/////////
	//===========================================================
	constexpr const bool operator==(const Vector3Int& rhs) const {
		return ((x == rhs.x) && (y == rhs.y) && (z == rhs.z));
	}

	constexpr const Vector3Int operator+(const Vector3Int& rhs) const {
		return Vector3Int((x + rhs.x), (y + rhs.y), (z + rhs.z));
	}

	constexpr const Vector3Int operator-(const Vector3Int& rhs) const {
		return Vector3Int((x - rhs.x), (y - rhs.y), (z - rhs.z));
	}

	constexpr const Vector3Int operator*(const Vector3Int& rhs) const {
		return Vector3Int((x * rhs.x), (y * rhs.y), (z * rhs.z));
	}

	constexpr const Vector3Int operator*(const int scalar) const {
		return Vector3Int((x * scalar), (y * scalar), (z * scalar));
	}

	Vector3Int& operator+=(const Vector3Int& rhs) {
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}

	Vector3Int& operator-=(const Vector3Int& rhs) {
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}

	Vector3Int& operator*=(const Vector3Int& rhs) {
		x *= rhs.x;
		y *= rhs.y;
		z *= rhs.z;
		return *this;
	}

	Vector3Int& operator*=(const int scalar) {
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	operator Vector2() const;
	operator Vector3() const;
	operator Vector4() const;

	operator Vector2Int() const;
	operator Vector4Int() const;
};



/*==========================================================================
 *   4차원 벡터가 구현된 구조체입니다...
 *=============*/
struct hyunwoo::Vector4
{
	//=======================================================
	/////////				Property...				/////////
	//=======================================================
	float x, y, z, w;

	static const Vector4 Zero;
	static const Vector4 One;
	static const Vector4 BasisX;
	static const Vector4 BasisY;
	static const Vector4 BasisZ;
	static const Vector4 BasisW;



	//===========================================================
	//////////			Constructor methods...			/////////
	//===========================================================
	constexpr Vector4(float x = 0.f, float y = 0.f, float z = 0.f, float w = 0.f) :x(x), y(y), z(z), w(w) {}
	constexpr Vector4(const Vector4&) = default;
	constexpr ~Vector4() = default;




	//=============================================================
	//////////			   Public methods..				  /////////
	//=============================================================
	constexpr static const float Dot(const Vector4& lhs, const Vector4& rhs) {
		return ((lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z) + (lhs.w * rhs.w));
	}

	constexpr const float GetSqrMagnitude() const {
		return Dot(*this, *this);
	}

	const float GetMagnitude() const;

	const Vector4 GetNormalized() const {
		const float size = GetMagnitude();
		if (size == 0.0f) return Vector4::Zero;

		const float invSize = (1.f / size);
		return Vector4((x * invSize), (y * invSize), (z * invSize), (w * invSize));
	}

	void Normalized() {
		const float size = GetMagnitude();
		if (size == 0.0f) return;

		const float invSize = (1.f / size);
		x *= invSize;
		y *= invSize;
		z *= invSize;
		w *= invSize;
	}

	void AddString(std::string& sharedStr) const {
		sharedStr += "(";
		sharedStr += std::to_string(x);
		sharedStr += ", ";
		sharedStr += std::to_string(y);
		sharedStr += ", ";
		sharedStr += std::to_string(z);
		sharedStr += ", ";
		sharedStr += std::to_string(w);
		sharedStr += ")";
	}

	void AddWString(std::wstring& sharedStr) const {
		sharedStr += L"(";
		sharedStr += std::to_wstring(x);
		sharedStr += L", ";
		sharedStr += std::to_wstring(y);
		sharedStr += L", ";
		sharedStr += std::to_wstring(z);
		sharedStr += L", ";
		sharedStr += std::to_wstring(w);
		sharedStr += L")";
	}


	//===========================================================
	/////////			Operator methods...				/////////
	//===========================================================
	constexpr const bool operator==(const Vector4& rhs) const {
		return ((x == rhs.x) && (y == rhs.y) && (z == rhs.z) && (w==rhs.w));
	}

	constexpr const Vector4 operator+(const Vector4& rhs) const {
		return Vector4((x + rhs.x), (y + rhs.y), (z + rhs.z), (w+rhs.w));
	}

	constexpr const Vector4 operator-(const Vector4& rhs) const {
		return Vector4((x - rhs.x), (y - rhs.y), (z - rhs.z), (w-rhs.w));
	}

	constexpr const Vector4 operator*(const Vector4& rhs) const {
		return Vector4((x * rhs.x), (y * rhs.y), (z * rhs.z), (w*rhs.w));
	}

	constexpr const Vector4 operator*(const float scalar) const {
		return Vector4((x * scalar), (y * scalar), (z * scalar), (w*scalar));
	}

	Vector4& operator+=(const Vector4& rhs) {
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		w += rhs.w;
		return *this;
	}

	Vector4& operator-=(const Vector4& rhs) {
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		w -= rhs.w;
		return *this;
	}

	Vector4& operator*=(const Vector4& rhs) {
		x *= rhs.x;
		y *= rhs.y;
		z *= rhs.z;
		w *= rhs.w;
		return *this;
	}

	Vector4& operator*=(const float scalar) {
		x *= scalar;
		y *= scalar;
		z *= scalar;
		w *= scalar;
		return *this;
	}

	operator Vector2() const;
	operator Vector3() const;
	
	operator Vector2Int() const;
	operator Vector3Int() const;
	operator Vector4Int() const;
};


/**정수버전....**/
struct hyunwoo::Vector4Int
{
	//=======================================================
	/////////				Property...				/////////
	//=======================================================
	int x, y, z, w;

	static const Vector4Int Zero;
	static const Vector4Int One;
	static const Vector4Int BasisX;
	static const Vector4Int BasisY;
	static const Vector4Int BasisZ;
	static const Vector4Int BasisW;



	//===========================================================
	//////////			Constructor methods...			/////////
	//===========================================================
	constexpr Vector4Int(int x = 0, int y = 0, int z = 0, int w = 0) :x(x), y(y), z(z), w(w) {}
	constexpr Vector4Int(const Vector4Int&) = default;
	constexpr ~Vector4Int() = default;




	//=============================================================
	//////////			   Public methods..				  /////////
	//=============================================================
	constexpr static const int Dot(const Vector4Int& lhs, const Vector4Int& rhs) {
		return ((lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z) + (lhs.w * rhs.w));
	}

	constexpr const int GetSqrMagnitude() const {
		return Dot(*this, *this);
	}

	const int GetMagnitude() const;

	const Vector4Int GetNormalized() const {
		const int size = GetMagnitude();
		if (size == 0) return Vector4Int::Zero;

		return Vector4Int((x / size), (y / size), (z / size), (w / size));
	}

	void Normalized() {
		const int size = GetMagnitude();
		if (size == 0) return;

		x /= size;
		y /= size;
		z /= size;
		w /= size;
	}

	void AddString(std::string& sharedStr) {
		sharedStr += "(";
		sharedStr += std::to_string(x);
		sharedStr += ", ";
		sharedStr += std::to_string(y);
		sharedStr += ", ";
		sharedStr += std::to_string(z);
		sharedStr += ", ";
		sharedStr += std::to_string(w);
		sharedStr += ")";
	}

	void AddWString(std::wstring& sharedStr) {
		sharedStr += L"(";
		sharedStr += std::to_wstring(x);
		sharedStr += L", ";
		sharedStr += std::to_wstring(y);
		sharedStr += L", ";
		sharedStr += std::to_wstring(z);
		sharedStr += L", ";
		sharedStr += std::to_wstring(w);
		sharedStr += L")";
	}



	//===========================================================
	/////////			Operator methods...				/////////
	//===========================================================
	constexpr const bool operator==(const Vector4Int& rhs) const {
		return ((x == rhs.x) && (y == rhs.y) && (z == rhs.z) && (w == rhs.w));
	}

	constexpr const Vector4Int operator+(const Vector4Int& rhs) const {
		return Vector4Int((x + rhs.x), (y + rhs.y), (z + rhs.z), (w + rhs.w));
	}

	constexpr const Vector4Int operator-(const Vector4Int& rhs) const {
		return Vector4Int((x - rhs.x), (y - rhs.y), (z - rhs.z), (w - rhs.w));
	}

	constexpr const Vector4Int operator*(const Vector4Int& rhs) const {
		return Vector4Int((x * rhs.x), (y * rhs.y), (z * rhs.z), (w * rhs.w));
	}

	constexpr const Vector4Int operator*(const int scalar) const {
		return Vector4Int((x * scalar), (y * scalar), (z * scalar), (w * scalar));
	}

	Vector4Int& operator+=(const Vector4Int& rhs) {
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		w += rhs.w;
		return *this;
	}

	Vector4Int& operator-=(const Vector4Int& rhs) {
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		w -= rhs.w;
		return *this;
	}

	Vector4Int& operator*=(const Vector4Int& rhs) {
		x *= rhs.x;
		y *= rhs.y;
		z *= rhs.z;
		w *= rhs.w;
		return *this;
	}

	Vector4Int& operator*=(const int scalar) {
		x *= scalar;
		y *= scalar;
		z *= scalar;
		w *= scalar;
		return *this;
	}

	operator Vector2() const;
	operator Vector3() const;
	operator Vector4() const;

	operator Vector2Int() const;
	operator Vector3Int() const;
};