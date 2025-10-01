#pragma once
#include "../MathModule/Vector.h"
#include "../MathModule/Matrix.h"

namespace hyunwoo {
	struct Plane;
	struct BoundingBox;
	struct BoundingSphere;
	struct Frustum;
}



/*===========================================================================================================================================
 *   경계 상자를 나타내는 구조체입니다...
 ********/
struct hyunwoo::BoundingBox
{
	Vector3 Min;
	Vector3 Max;
};





/*===========================================================================================================================================
 *   경계 볼륨를 나타내는 구조체입니다...
 ********/
struct hyunwoo::BoundingSphere
{
	Vector3 Center;
	float   Radius;
};









/*===========================================================================================================================================
 *   3차원 평면 형태의 도형을 나타내는 구조체입니다...
 ******/
struct hyunwoo::Plane
{
	//===========================================================================================
	/////////////							Defines..							   //////////////
	//==========================================================================================
public:
	struct TestResult
	{
		bool Overlapped : 1;
		bool Outside	: 1;
		bool Inside		: 1;
	};



	//===============================================================================================
	/////////////							Properties..							   //////////////
	//===============================================================================================
public:
	union 
	{
		Vector4 PlaneEquation;
		
		struct {
			Vector3 Dir;
			float   MinDistance;
		};
	};




	//===================================================================================================
	/////////////							Public methods..							   //////////////
	//===================================================================================================
public:
	Plane() = default;
	Plane(const Vector4& planeEquation) : PlaneEquation(planeEquation) { Normalized(); }
	

	void Normalized();

	const TestResult Test(const Vector4& affinePoint);
	const TestResult Test(const BoundingBox& boundingBox );
	const TestResult Test(const BoundingSphere& boundingSphere);
};


















/*=============================================================================================================================================
 *    절두체 공간을 나타내는 구조체입니다....
 *******/
struct hyunwoo::Frustum final
{
	//===============================================================================================
	///////////								Properties..								/////////////
	//===============================================================================================
public:
	union 
	{
		Plane Planes[6];

		struct {
			Plane LeftDirPlane;
			Plane RightDirPlane;
			Plane BackDirPlane;
			Plane ForwardDirPlane;
			Plane UpDirPlane;
			Plane DownDirPlane;
		};
	};




	//=============================================================================================
	/////////////						  Public methods..							///////////////
	//=============================================================================================
public:
	Frustum() = default;
	Frustum(const Matrix4x4& finalMat) { SetPlanesFromFinalMatrix(finalMat); }
	Frustum(const Plane& leftPlane, const Plane& rightPlane, const Plane& backPlane, const Plane& forwardPlane, const Plane& upPlane, const Plane& downPlane): Planes{leftPlane, rightPlane, backPlane, forwardPlane, upPlane, downPlane} {};


	void SetPlanesFromFinalMatrix(const Matrix4x4& finalMat);
	
	const bool IsOverlapped(const Vector4& affinePoint);
	const bool IsOverlapped(const BoundingBox& boundingBox);
	const bool IsOverlapped(const BoundingSphere& boundingSphere);
};