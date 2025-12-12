#pragma once
#include "Math.h"
#include "Vector.h"
#include "Matrix.h"
#include <string>

namespace hyunwoo {
	struct Quaternion;
}




/*=============================================================================================================================================
 *    쿼터니언이 구현된 구조체입니다....
 *==============*/
struct hyunwoo::Quaternion
{
	//=================================================================================================
	///////////////////							Defines...							///////////////////
	//=================================================================================================
public:
	enum class Basis
	{
		X,Y,Z
	};




	//=====================================================================================================
	///////////////////							Properties...							///////////////////
	//=====================================================================================================		
public:
	float   W;
	Vector3 XYZ;

	static const Quaternion Identity;



	//=========================================================================================================
	///////////////////							Public methods...							///////////////////
	//=========================================================================================================
public:
	Quaternion(const float W = 1.f, const Vector3& XYZ = Vector3::Zero):W(W), XYZ(XYZ) {}
	Quaternion(const Quaternion&) = default;
	~Quaternion()				  = default;


	const Quaternion& Conjugate();
	const float		  GetMagnitude() const;
	const Quaternion  GetConjugate() const;
	Matrix4x4		  GetRotateMatrix() const;

	const Quaternion  GetNormalized() const;
	void			  Normalized();


	void AddString(std::string& sharedStr) const;
	void AddWString(std::wstring& sharedStr) const;


	static Quaternion AngleAxis(const float angle, const Vector3& rotAxis);
	static Quaternion Euler(const float yAngle, const float xAngle, const float zAngle);
	static Quaternion FromTo(const Vector3& from ,const Vector3& to, const float angleScale = 1.f);
	static Vector3    ToEuler_XYZ(const Quaternion& fromQuat);




	//=========================================================================================================
	///////////////////							Operator methods...							///////////////////
	//=========================================================================================================
public:
	Quaternion operator +(const Quaternion& rhs) const;
	Quaternion operator -(const Quaternion& rhs) const;

	Quaternion    operator *(const float scalar)    const;
	Quaternion    operator *(const Quaternion& rhs) const;
	const Vector3 operator *(const Vector3& rhs)    const;

	Quaternion& operator +=(const Quaternion& rhs);
	Quaternion& operator -=(const Quaternion& rhs);
	Quaternion& operator *=(const float scalar);
	Quaternion& operator *=(const Quaternion& rhs);
	Quaternion& operator *=(const Vector3& rhs);
};