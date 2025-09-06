#pragma once
#include "Math.h"
#include "Vector.h"
#include "Matrix.h"
#include <string>

namespace hyunwoo {
	struct Quaternion;
}




/*===========================================================================================
 *    쿼터니언이 구현된 구조체입니다....
 *==============*/
struct hyunwoo::Quaternion
{
	//=============================================================
	///////////				 Properties..				///////////
	//=============================================================
public:
	float   W;
	Vector3 XYZ;

	static const Quaternion Identity;





	//=================================================================
	////////////			  Public methods..			   ////////////
	//=================================================================
public:
	static Quaternion AngleAxis(const float angle, const Vector3& rotAxis);
	static Quaternion Euler(const float xAngle, const float yAngle, const float zAngle);


	/**************************************
	 *    생성자 및 소멸자 메소드...
	 *********/
	Quaternion(const float W = 0.f, const Vector3& XYZ = Vector3::Zero):W(W), XYZ(XYZ) {}
	Quaternion(const Quaternion&) = default;
	~Quaternion()				  = default;


	/*************************************
	 *   켤레와 관련된 메소드....
	 **********/
	const Quaternion& Conjugate();
	const Quaternion  GetConjugate() const;
	const float		  Magnitude() const;


	/*************************************
	 *   문자열 람다 관련 메소드...
	 ********/
	void AddString(std::string& sharedStr) const;
	void AddWString(std::wstring& sharedStr) const;





	//=========================================================================
	/////////////				 Operator methods..				  /////////////
	//=========================================================================
public:
	Quaternion    operator *(const Quaternion& rhs) const;
	const Vector3 operator *(const Vector3& rhs) const;

	Quaternion& operator *=(const Quaternion& rhs);
	Quaternion& operator *=(const Vector3& rhs);

	operator Matrix4x4() const;
};