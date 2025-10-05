#include "Geometry.h"

/*********************************************************
 *   Plane 메소드들의 정의....
 *******/

/*===================================================================================================================
 *    평면의 방정식을 정규화합니다....
 ********/
void hyunwoo::Plane::Normalized()
{
	const float dirSize    = Dir.GetMagnitude();
	const float dirSizeDiv = (dirSize == 0.f ? 0.f : (1.f / dirSize));

	PlaneEquation *= dirSizeDiv;
}



/*===================================================================================================================
 *   아핀 공간의 점/바운딩 박스/바운딩 스피어가 평면과 어떤 관계가 있는지 검사합니다...
 ********/
const hyunwoo::Plane::TestResult hyunwoo::Plane::Test(const Vector4& affinePoint)
{
	Plane::TestResult ret = { 0, };
	const float		  dot = Vector4::Dot(PlaneEquation, affinePoint);


	/*----------------------------
	 *  평면 안의 점인가..?
	 *----*/
	if (dot==0.f) {
		ret.Overlapped = true;
		return ret;
	}

	/*----------------------------
	 *  평면의 앞쪽에 있는가..?
	 *----*/
	else if (dot >= 0.f) {
		ret.Outside = true;
		return ret;
	}

	/*----------------------------
	 *  평면의 뒤쪽에 있는가..?
	 *----*/
	else ret.Inside = true;
	return ret;
}


const hyunwoo::Plane::TestResult hyunwoo::Plane::Test(const BoundingBox& boundingBox)
{
	Plane::TestResult ret = { 0, };

	const Vector4 testDir = Vector4(
		(Dir.x < 0.f ? boundingBox.Max.x : boundingBox.Min.x),
		(Dir.y < 0.f ? boundingBox.Max.y : boundingBox.Min.y),
		(Dir.z < 0.f ? boundingBox.Max.z : boundingBox.Min.z),
		1.f
	);


	/*------------------------------------------
	 *   평면의 바깥쪽에 있는지를 판별한다...
	 *-----*/
	if (Vector4::Dot(testDir, PlaneEquation) > 0.f) {
		ret.Outside = true;
		return ret;
	}


	/*-----------------------------------------
	 *   평면과 교차하는지를 판별한다...
	 *-----*/
	ret.Inside = true;
	if (Vector4::Dot(-testDir, PlaneEquation) < 0.f) {
		ret.Overlapped = true;
	}

	return ret;
}


const hyunwoo::Plane::TestResult hyunwoo::Plane::Test(const BoundingSphere& boundingSphere)
{
	Plane::TestResult ret = { 0, };
	const float		  dot    = Vector4::Dot(PlaneEquation, Vector4(boundingSphere.Center, 1.f));
	const float       dotLen = Math::Abs(dot);

	/*--------------------------------------
	 *  평면과 겹쳐있는가?
	 *-----*/
	if (dotLen <= boundingSphere.Radius) {
		ret.Overlapped = true;
	}


	/*--------------------------------------
	 *  평면의 바깥쪽에 있는가?
	 *----*/
	if(dot >= 0.f){
		ret.Outside = true;
		return ret;
	}


	/*--------------------------------------
	 *  평면의 안쪽에 있는가?
	 *----*/
	else ret.Inside = true;
	return ret;
}











/*********************************************************
 *   Frustum 메소드들의 정의....
 *******/

/*===================================================================================================================
 *   주어진 최종 행렬로부터, 절두체 영역의 바깥을 향하는 정규화된 평면들을 만든다...
 ******/
void hyunwoo::Frustum::SetPlanesFromFinalMatrix(const Matrix4x4& PerspectiveProjectionMat)
{
	const Matrix4x4 transpose = PerspectiveProjectionMat.GetTranspose();

	LeftDirPlane  = Plane(-(transpose.BasisW + transpose.BasisX));
	RightDirPlane = Plane(transpose.BasisX - transpose.BasisW);

	DownDirPlane = Plane(-(transpose.BasisW + transpose.BasisY));
	UpDirPlane   = Plane(transpose.BasisY - transpose.BasisW);

	BackDirPlane    = Plane(-(transpose.BasisW + transpose.BasisZ));
	ForwardDirPlane = Plane(transpose.BasisZ - transpose.BasisW);
}


/*===================================================================================================================
 *   주어진 아핀공간의 점/바운딩 박스/바운딩 스피어가, 절두체 범위 안에 포함되는지를 판별합니다....
 *******/ 
const bool hyunwoo::Frustum::IsOverlapped(const Vector4& affinePoint)
{
	for (int i = 0; i < 6; i++)
	{
		Plane::TestResult ret = Planes[i].Test(affinePoint);
		if ((ret.Overlapped || ret.Inside) == false) {
			return false;
		}
	}

	return true;
}

const bool hyunwoo::Frustum::IsOverlapped(const BoundingBox& boundingBox)
{
	for (int i = 0; i < 6; i++)
	{
		Plane::TestResult ret = Planes[i].Test(boundingBox);
		if ((ret.Overlapped || ret.Inside) == false) {
			return false;
		}
	}

	return true;
}

const bool hyunwoo::Frustum::IsOverlapped(const BoundingSphere& boundingSphere)
{
	for (int i = 0; i < 6; i++)
	{
		Plane::TestResult ret = Planes[i].Test(boundingSphere);
		if ((ret.Overlapped || ret.Inside)==false) {
			return false;
		}
	}

	return true;
}


