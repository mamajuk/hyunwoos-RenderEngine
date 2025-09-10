#pragma once
#include "Math.h"
#include "Vector.h"
#include "Matrix.h"
#include <string>

namespace hyunwoo {
	struct Quaternion;
}




/*===============================================================================================================================
 *    쿼터니언이 구현된 구조체입니다....
 *==============*/
struct hyunwoo::Quaternion
{
	//=====================================================================
	///////////						Properties..				///////////
	//=====================================================================
public:
	float   W;
	Vector3 XYZ;

	static const Quaternion Identity;



	//=========================================================================
	////////////					Public methods..			   ////////////
	//=========================================================================
public:
	Quaternion(const float W = 0.f, const Vector3& XYZ = Vector3::Zero):W(W), XYZ(XYZ) {}
	Quaternion(const Quaternion&) = default;
	~Quaternion()				  = default;


	const float		  Magnitude() const;
	const Quaternion& Conjugate();

	const Quaternion  GetConjugate() const;
	Matrix4x4		  GetRotateMatrix() const;


	void AddString(std::string& sharedStr) const;
	void AddWString(std::wstring& sharedStr) const;


	static Quaternion AngleAxis(const float angle, const Vector3& rotAxis);
	static Quaternion Euler(const float xAngle, const float yAngle, const float zAngle);
	static Quaternion FromTo(const Vector3& from ,const Vector3& to, const float angleScale = 1.f);



	//=================================================================================
	/////////////					Operator methods..				      /////////////
	//=================================================================================
public:
	Quaternion    operator *(const Quaternion& rhs) const;
	const Vector3 operator *(const Vector3& rhs) const;

	Quaternion& operator *=(const Quaternion& rhs);
	Quaternion& operator *=(const Vector3& rhs);
};