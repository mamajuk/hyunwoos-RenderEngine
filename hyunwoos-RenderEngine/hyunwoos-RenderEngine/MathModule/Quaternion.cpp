#include "Quaternion.h"

/*================================================================================================
 *    Quaternion의 전역 변수를 초기화한다...
 *============*/
const hyunwoo::Quaternion hyunwoo::Quaternion::Identity = { 1.f, Vector3::Zero };









/*================================================================================================
 *    주어진 회전축 rotAxis와 직교하는 회전평면에서 angle만큼 회전시키는 사원수를 얻습니다...
 *===========*/
hyunwoo::Quaternion hyunwoo::Quaternion::AngleAxis(const float angle, const Vector3& rotAxis)
{
	const float rad = (Math::Angle2Rad * angle * .5f);

	return Quaternion(
		Math::Cos(rad),
		(rotAxis * Math::Sin(rad))
	);
}









/*===================================================================================================
 *   주어진 세 각도로 오일러 회전이 적용된 사원수를 얻습니다....
 *===========*/
hyunwoo::Quaternion hyunwoo::Quaternion::Euler(const float xAngle, const float yAngle, const float zAngle)
{
	return AngleAxis(zAngle, Vector3::Back) * AngleAxis(xAngle, Vector3::Right) * AngleAxis(yAngle, Vector3::Up);
}










/*===========================================================================================
 *    Quaternion의 켤레를 구하는 메소드들입니다......
 *=============*/
const hyunwoo::Quaternion& hyunwoo::Quaternion::Conjugate() {
	XYZ *= -1.f;
	return *this;
}

const hyunwoo::Quaternion hyunwoo::Quaternion::GetConjugate() const {
	return Quaternion(W, XYZ * -1.f);
}










/*=============================================================================================
 *    해당 Quaternion의 크기를 얻습니다.....
 *=============*/
const float hyunwoo::Quaternion::Magnitude() const
{
	const Quaternion mulConjugate = operator*(GetConjugate());

	return Math::Sqrt(mulConjugate.W * mulConjugate.W + XYZ.GetSqrMagnitude());
}










/*===============================================================================================
 *    다른 사원수를 곱합니다....
 *=============*/
hyunwoo::Quaternion hyunwoo::Quaternion::operator *(const Quaternion& rhs) const 
{
	return Quaternion(
		(W * rhs.W) - Vector3::Dot(XYZ, rhs.XYZ),
		Vector3::Cross(XYZ, rhs.XYZ) + (rhs.XYZ * W) + (XYZ * rhs.W)
	);
}

hyunwoo::Quaternion& hyunwoo::Quaternion::operator*=(const Quaternion& rhs)
{
	operator=(operator*(rhs));
	return *this;
}










/*===============================================================================================
 *    해당 사원수에 설정된대로 회전된 벡터를 얻습니다....
 *=============*/
const hyunwoo::Vector3 hyunwoo::Quaternion::operator *(const hyunwoo::Vector3& rhs) const
{
	return (GetConjugate() * Quaternion(0, rhs) * *this).XYZ;
}

hyunwoo::Quaternion& hyunwoo::Quaternion::operator *=(const hyunwoo::Vector3& rhs)
{
	operator=(Quaternion(0, operator*(rhs)));
	return *this;
}








/*================================================================================================
 *    해당 사원수만큼 회전이 적용된, 회전 행렬로 캐스트합니다...
 *============*/
hyunwoo::Quaternion::operator hyunwoo::Matrix4x4() const
{
	return Matrix4x4{
		(*this * Vector3::BasisX),
		(*this * Vector3::BasisY),
		(*this * Vector3::BasisZ),
		Vector4::BasisW
	};
}








/*=================================================================================================
 *   문자열 람다에서 출력하기 위한 메소드...
 *================*/
void hyunwoo::Quaternion::AddString(std::string& sharedStr) const
{
	const float rad  = Math::Acos(W);
	const float s    = Math::Sin(rad);
	const float sDiv = (s != 0.f ? (1.f / s) : 1.f);

	sharedStr += "(angle: ";
	sharedStr += std::to_string(rad * Math::Rad2Angle * 2.f);
	sharedStr += "º, axis: ";
	(XYZ * sDiv).AddString(sharedStr);
	sharedStr += ")";
}

void hyunwoo::Quaternion::AddWString(std::wstring& sharedStr) const
{
	const float rad  = Math::Acos(W);
	const float s    = Math::Sin(rad);
	const float sDiv = (s != 0.f ? (1.f / s) : 1.f);

	sharedStr += L"(angle: ";
	sharedStr += std::to_wstring(rad * Math::Rad2Angle * 2.f);
	sharedStr += L"º, axis: ";
	(XYZ * sDiv).AddWString(sharedStr);
	sharedStr += L")";
}