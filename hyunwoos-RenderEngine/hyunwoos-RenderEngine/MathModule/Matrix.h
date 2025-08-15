#pragma once
#include "Vector.h"
#include <string>

namespace hyunwoo {
	class Matrix2x2;
	class Matrix3x3;
	class Matrix4x4;
}



/*=====================================================================
 *   2x2 행렬에 대한 연산이 정의된 구조체입니다...
 *==========*/
class hyunwoo::Matrix2x2 final
{
	//======================================================
	////////			   Properties...			////////
	//======================================================
public:
	union 
	{
		Vector2 cols[2];

		struct {
			Vector2 BasisX;
			Vector2 BasisY;
		};
	};

	static const Matrix2x2 Identity;



	//====================================================
	//////			   Construct methods..			//////
	//====================================================
public:
	Matrix2x2(const Vector2& basisX = Vector2::BasisX, const Vector2& basisY = Vector2::BasisY):BasisX(basisX), BasisY(basisY){}
	Matrix2x2(const Matrix2x2& rhs) = default;
	~Matrix2x2()					= default;



	//=======================================================
	////////			Public methods...			/////////
	//=======================================================
public:
	Matrix2x2& Transpose();
	Matrix2x2  GetTranspose() const;

	void AddString(std::string& sharedStr) const {
		sharedStr += "basisX";
		BasisX.AddString(sharedStr);
		sharedStr += "\nbasisY";
		BasisY.AddString(sharedStr);
	}

	void AddWString(std::wstring& sharedStr) const {
		sharedStr += L"basisX";
		BasisX.AddWString(sharedStr);
		sharedStr += L"\nbasisY";
		BasisY.AddWString(sharedStr);
	}



	//=========================================================
	/////////			  Operator methods..		   ////////
	//=========================================================
	Matrix2x2 operator+(const Matrix2x2& rhs) const {
		return Matrix2x2((BasisX + rhs.BasisX), (BasisY + rhs.BasisY));
	}

	Matrix2x2 operator*(const Matrix2x2& rhs) const {
		const Matrix2x2 transpose = GetTranspose();

		return Matrix2x2{
			(Vector2::Dot(transpose.BasisX, rhs.BasisX) + Vector2::Dot(transpose.BasisX, rhs.BasisY)),
			(Vector2::Dot(transpose.BasisY, rhs.BasisX) + Vector2::Dot(transpose.BasisY, rhs.BasisY))
		};
	}

	Matrix2x2 operator*(const float scalar) const {
		return Matrix2x2{ (BasisX * scalar), (BasisY * scalar) };
	}

	Vector2 operator*(const Vector2& rhs) const {
		const Matrix2x2 transpose = GetTranspose();

		return Vector2{
			Vector2::Dot(transpose.BasisX, rhs),
			Vector2::Dot(transpose.BasisY, rhs)
		};
	}

	friend Vector2 operator*(const Vector2& lhs, const Matrix2x2& rhs) {
		return Vector2{
			Vector2::Dot(lhs, rhs.BasisX),
			Vector2::Dot(lhs, rhs.BasisY)
		};
	}

	Matrix2x2& operator+=(const Matrix2x2& rhs) {

		BasisX += rhs.BasisX;
		BasisY += rhs.BasisY;
		return *this;
	}

	Matrix2x2& operator*=(const Matrix2x2& rhs) {
		operator=(operator*(rhs));
		return *this;
	}

	Matrix2x2& operator*=(const float scalar) {
		BasisX *= scalar;
		BasisY *= scalar;
		return *this;
	}

	friend Vector2& operator*=(Vector2& lhs, const Matrix2x2& rhs) {
		lhs = (lhs * rhs);
		return lhs;
	}

	operator Matrix3x3() const;
	operator Matrix4x4() const;
};






/*=====================================================================
 *   3x3 행렬에 대한 연산이 정의된 구조체입니다...
 *==========*/
class hyunwoo::Matrix3x3 final
{
	//======================================================
	////////			    Properties..			////////
	//======================================================
public:
	union
	{
		Vector3 cols[3];

		struct {
			Vector3 BasisX;
			Vector3 BasisY;
			Vector3 BasisZ;
		};
	};

	static const Matrix3x3 Identity;



	//====================================================
	//////			   Construct methods..			//////
	//====================================================
public:
	Matrix3x3(const Vector3& basisX = Vector3::BasisX, const Vector3& basisY = Vector3::BasisY, const Vector3& basisZ = Vector3::BasisZ) :BasisX(basisX), BasisY(basisY), BasisZ(basisZ){}
	Matrix3x3(const Matrix3x3& rhs) = default;
	~Matrix3x3() = default;



	//=======================================================
	////////			Public methods...			/////////
	//=======================================================
public:
	Matrix3x3& Transpose();
	Matrix3x3  GetTranspose() const;

	void AddString(std::string& sharedStr) const {
		sharedStr += "basisX";
		BasisX.AddString(sharedStr);
		sharedStr += "\nbasisY";
		BasisY.AddString(sharedStr);
		sharedStr += "\nbasisZ";
		BasisZ.AddString(sharedStr);
	}

	void AddWString(std::wstring& sharedStr) const {
		sharedStr += L"basisX";
		BasisX.AddWString(sharedStr);
		sharedStr += L"\nbasisY";
		BasisY.AddWString(sharedStr);
		sharedStr += L"\nbasisZ";
		BasisZ.AddWString(sharedStr);
	}



	//=========================================================
	/////////			  Operator methods..		   ////////
	//=========================================================
	Matrix3x3 operator+(const Matrix3x3& rhs) const {
		return Matrix3x3(
			(BasisX + rhs.BasisX), 
			(BasisY + rhs.BasisY), 
			(BasisZ + rhs.BasisZ)
		);
	}

	Matrix3x3 operator*(const Matrix3x3& rhs) const {
		const Matrix3x3 transpose = GetTranspose();

		return Matrix3x3{
			(Vector3::Dot(transpose.BasisX, rhs.BasisX) + Vector3::Dot(transpose.BasisX, rhs.BasisY) + Vector3::Dot(transpose.BasisX, rhs.BasisZ)),
			(Vector3::Dot(transpose.BasisY, rhs.BasisX) + Vector3::Dot(transpose.BasisY, rhs.BasisY) + Vector3::Dot(transpose.BasisY, rhs.BasisZ)),
			(Vector3::Dot(transpose.BasisZ, rhs.BasisX) + Vector3::Dot(transpose.BasisZ, rhs.BasisY) + Vector3::Dot(transpose.BasisZ, rhs.BasisZ))
		};
	}

	Matrix3x3 operator*(const float scalar) const {
		return Matrix3x3{ (BasisX * scalar), (BasisY * scalar), (BasisZ * scalar) };
	}

	Vector3 operator*(const Vector3& rhs) const {
		const Matrix3x3 transpose = GetTranspose();

		return Vector3{
			Vector3::Dot(transpose.BasisX, rhs),
			Vector3::Dot(transpose.BasisY, rhs),
			Vector3::Dot(transpose.BasisZ, rhs)
		};
	}

	friend Vector3 operator*(const Vector3& lhs, const Matrix3x3& rhs) {
		return Vector3{
			Vector3::Dot(lhs, rhs.BasisX),
			Vector3::Dot(lhs, rhs.BasisY),
			Vector3::Dot(lhs, rhs.BasisZ)
		};
	}

	Matrix3x3& operator+=(const Matrix3x3& rhs) {
		BasisX += rhs.BasisX;
		BasisY += rhs.BasisY;
		BasisZ += rhs.BasisZ;
		return *this;
	}

	Matrix3x3& operator*=(const Matrix3x3& rhs) {
		operator=(operator*(rhs));
		return *this;
	}

	Matrix3x3& operator*=(const float scalar) {
		BasisX *= scalar;
		BasisY *= scalar;
		BasisZ *= scalar;
		return *this;
	}

	friend Vector3& operator*=(Vector3& lhs, const Matrix3x3& rhs) {
		lhs = (lhs * rhs);
		return lhs;
	}

	operator Matrix2x2() const;
	operator Matrix4x4() const;
};







/*=====================================================================
 *   4x4 행렬에 대한 연산이 정의된 구조체입니다...
 *==========*/
class hyunwoo::Matrix4x4 final
{
	//======================================================
	////////			    Properties....			////////
	//======================================================
public:
	union
	{
		Vector4 cols[4];

		struct {
			Vector4 BasisX;
			Vector4 BasisY;
			Vector4 BasisZ;
			Vector4 BasisW;
		};
	};

	static const Matrix4x4 Identity;


	//====================================================
	//////			   Construct methods..			//////
	//====================================================
public:
	Matrix4x4(const Vector4& basisX = Vector4::BasisX, const Vector4& basisY = Vector4::BasisY, const Vector4& basisZ = Vector4::BasisZ, const Vector4& basisW = Vector4::BasisW):BasisX(basisX), BasisY(basisY), BasisZ(basisZ), BasisW(basisW) {}
	Matrix4x4(const Matrix4x4& rhs) = default;
	~Matrix4x4() = default;



	//=======================================================
	////////			Public methods...			/////////
	//=======================================================
public:
	Matrix4x4& Transpose();
	Matrix4x4  GetTranspose() const;

	void AddString(std::string& sharedStr) const {
		sharedStr += "basisX";
		BasisX.AddString(sharedStr);
		sharedStr += "\nbasisY";
		BasisY.AddString(sharedStr);
		sharedStr += "\nbasisZ";
		BasisZ.AddString(sharedStr);
		sharedStr += "\nbasisW";
		BasisW.AddString(sharedStr);
	}

	void AddWString(std::wstring& sharedStr) const {
		sharedStr += L"basisX";
		BasisX.AddWString(sharedStr);
		sharedStr += L"\nbasisY";
		BasisY.AddWString(sharedStr);
		sharedStr += L"\nbasisZ";
		BasisZ.AddWString(sharedStr);
		sharedStr += L"\nbasisW";
		BasisW.AddWString(sharedStr);
	}



	//=========================================================
	/////////			  Operator methods..		   ////////
	//=========================================================
	Matrix4x4 operator+(const Matrix4x4& rhs) const {
		return Matrix4x4(
			(BasisX + rhs.BasisX),
			(BasisY + rhs.BasisY),
			(BasisZ + rhs.BasisZ),
			(BasisW + rhs.BasisW)
		);
	}

	Matrix4x4 operator*(const Matrix4x4& rhs) const {
		const Matrix4x4 transpose = GetTranspose();

		return Matrix4x4{
			(Vector4::Dot(transpose.BasisX, rhs.BasisX) + Vector4::Dot(transpose.BasisX, rhs.BasisY) + Vector4::Dot(transpose.BasisX, rhs.BasisZ) + Vector4::Dot(transpose.BasisX, rhs.BasisW)),
			(Vector4::Dot(transpose.BasisY, rhs.BasisX) + Vector4::Dot(transpose.BasisY, rhs.BasisY) + Vector4::Dot(transpose.BasisY, rhs.BasisZ) + Vector4::Dot(transpose.BasisY, rhs.BasisW)),
			(Vector4::Dot(transpose.BasisZ, rhs.BasisX) + Vector4::Dot(transpose.BasisZ, rhs.BasisY) + Vector4::Dot(transpose.BasisZ, rhs.BasisZ) + Vector4::Dot(transpose.BasisZ, rhs.BasisW)),
			(Vector4::Dot(transpose.BasisW, rhs.BasisX) + Vector4::Dot(transpose.BasisW, rhs.BasisY) + Vector4::Dot(transpose.BasisW, rhs.BasisZ) + Vector4::Dot(transpose.BasisW, rhs.BasisW))
		};
	}

	Matrix4x4 operator*(const float scalar) const {
		return Matrix4x4{ (BasisX * scalar), (BasisY * scalar), (BasisZ * scalar), (BasisW * scalar) };
	}

	Vector4 operator*(const Vector4& rhs) const {
		const Matrix4x4 transpose = GetTranspose();

		return Vector4{
			Vector4::Dot(transpose.BasisX, rhs),
			Vector4::Dot(transpose.BasisY, rhs),
			Vector4::Dot(transpose.BasisZ, rhs),
			Vector4::Dot(transpose.BasisW, rhs)
		};
	}

	friend Vector4 operator*(const Vector4& lhs, const Matrix4x4& rhs) {
		return Vector4{
			Vector3::Dot(lhs, rhs.BasisX),
			Vector3::Dot(lhs, rhs.BasisY),
			Vector3::Dot(lhs, rhs.BasisZ),
			Vector3::Dot(lhs, rhs.BasisW)
		};
	}

	Matrix4x4& operator+=(const Matrix4x4& rhs) {
		BasisX += rhs.BasisX;
		BasisY += rhs.BasisY;
		BasisZ += rhs.BasisZ;
		BasisW += rhs.BasisW;
		return *this;
	}

	Matrix4x4& operator*=(const Matrix4x4& rhs) {
		operator=(operator*(rhs));
		return *this;
	}

	Matrix4x4& operator*=(const float scalar) {
		BasisX *= scalar;
		BasisY *= scalar;
		BasisZ *= scalar;
		BasisW *= scalar;
		return *this;
	}

	friend Vector4& operator*=(Vector4& lhs, const Matrix4x4& rhs) {
		lhs = (lhs * rhs);
		return lhs;
	}

	operator Matrix2x2() const;
	operator Matrix3x3() const;
};