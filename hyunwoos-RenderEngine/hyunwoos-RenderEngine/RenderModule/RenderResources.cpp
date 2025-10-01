#include "RenderResources.h"
#include "../MathModule/Quaternion.h"
#undef min
#undef max

/***************************************************
 *   Texture2D 메소드들의 정의....
 ******/

/*============================================================================================================
 *   지정한 위치의 픽셀을 얻습니다...
 *========*/
const hyunwoo::Color hyunwoo::Texture2D::GetPixel(const hyunwoo::Vector2Int& pos) const 
{
	const int idx = (pos.y * Width) + pos.x;

	if (idx >= 0 && idx < (Width * Height)) {
		return Pixels[idx];
	}

 	return Color::Pink;
}










/***************************************************
 *   Mesh 메소드들의 정의....
 ******/

/*==============================================================================================================
 *   메시의 바운딩 스피어를 재계산합니다...
 **********/
void hyunwoo::Mesh::RecalculateBoundingSphere()
{
	/******************************************
	 *  중점을 구한다....
	 *****/
	Vector3 total = Vector3::Zero;

	for (int i = 0; i < Vertices.size(); i++) {
		total += Vertices[i].ObjPos;
	}

	BoundSphere.Center = ( total * (1.f / (float)Vertices.size()) );


	/******************************************
	 *   반지름을 구한다....
	 *****/
	float maxSqrRadius = 0.f;

	for (int i = 0; i < Vertices.size(); i++) 
	{
		const float dst = (Vertices[i].ObjPos - BoundSphere.Center).GetSqrMagnitude();
		if (maxSqrRadius < dst) {
			maxSqrRadius = dst;
		}
	}

	BoundSphere.Radius = Math::Sqrt(maxSqrRadius);
}











/*==============================================================================================================
 *   메시의 바운딩 박스를 재계산합니다...
 **********/
void hyunwoo::Mesh::RecalculateBoundingBox()
{
	constexpr float min = std::numeric_limits<float>::min();
	constexpr float max = std::numeric_limits<float>::max();

	BoundBox.Min = (Vector3::One * max);
	BoundBox.Max = (Vector3::One * min);

	/**********************************************************
	 *   각 점들의 최댓값을 구한다...
	 ******/
	for (int i = 0; i < Vertices.size(); i++) 
	{
		const Vector3& objPos = Vertices[i].ObjPos;

		/*------------------------------
		 *  x차원의 처리....
		 *****/
		if (BoundBox.Max.x < objPos.x) {
			BoundBox.Max.x = objPos.x;
		}

		if (BoundBox.Min.x > objPos.x) {
			BoundBox.Min.x = objPos.x;
		}


		/*------------------------------
		 *  y차원의 처리....
		 *****/
		if (BoundBox.Max.y < objPos.y) {
			BoundBox.Max.y = objPos.y;
		}

		if (BoundBox.Min.y > objPos.y) {
			BoundBox.Min.y = objPos.y;
		}


		/*------------------------------
		 *  z차원의 처리....
		 *****/
		if (BoundBox.Max.z < objPos.z) {
			BoundBox.Max.z = objPos.z;
		}

		if (BoundBox.Min.z > objPos.z) {
			BoundBox.Min.z = objPos.z;
		}
	}
}










/*=================================================================================================================================
 *   메시의 바운딩 박스를 기반으로 메시를 생성합니다....
 ***************/
void hyunwoo::Mesh::CreateBoundingBoxMesh(Mesh& outMesh)
{
	std::vector<Vertex>& vertices    = outMesh.Vertices;
	std::vector<Triangle>& triangles = outMesh.Triangles;

	vertices.clear();
	triangles.clear();

	/****************************************************************************
	 *   바운딩 박스의 기둥을 생성하는 람다함수를 정의한다...
	 *******/

	 //사각형을 구성하는 버텍스의 인덱스값을 나타내는 구조체...
	struct Rect
	{
		uint32_t LeftTopIdx, RightTopIdx;
		uint32_t LeftBottomIdx, RightBottomIdx;
	};

	//연결 포트가 J모양인 바운딩 박스의 기둥을 나타내는 구조체...
	struct J_Pillar
	{
		Rect rects[4];
	};

	const Vector3& min = BoundBox.Min;
	const Vector3& max = BoundBox.Max;
	const Vector2  rectSize = (Vector2::One * .15f);
	const Vector2  rectSizeHalf = (rectSize * .5f);

	const auto AddPillar = [&](J_Pillar& outPillar, const Vector3& pillarCenterPos, const float angle)
	{
		/*-----------------------------------------
		 *   기둥의 사각형들을 구성한다....
		 ******/
		const Vector3   forward = (Quaternion::AngleAxis(angle, Vector3::Up) * Vector3::Forward);
		const Vector3   left    = (Quaternion::AngleAxis(angle, Vector3::Up) * Vector3::Left);

		const Vector3   leftTop				 = (pillarCenterPos + (forward * rectSizeHalf.y) + (left * rectSizeHalf.x));
		const Vector3   rightTop			 = (pillarCenterPos + (forward * rectSizeHalf.y) + (left * -rectSizeHalf.x));
		const Vector3   leftBottom			 = (pillarCenterPos + (forward * -rectSizeHalf.y) + (left * rectSizeHalf.x));
		const Vector3   rightBottom          = (pillarCenterPos + (forward * -rectSizeHalf.y) + (left * -rectSizeHalf.x));
		const float     pillarYoffsetTable[] = { 0.f, (max.y * .05f), (max.y * .9f), max.y };

		for (uint32_t i = 0; i < 4; i++)
		{
			Rect& rect = outPillar.rects[i];
			const Vector3 downOffset = (Vector3::Down * pillarYoffsetTable[i]);

			/**기둥을 구성하는 한 사각형의 버텍스를 구성한다....**/
			rect.LeftTopIdx = vertices.size();
			vertices.push_back(Vertex{ (leftTop + downOffset) });

			rect.RightTopIdx = rect.LeftTopIdx + 1;
			vertices.push_back(Vertex{ (rightTop + downOffset) });

			rect.LeftBottomIdx = rect.LeftTopIdx + 2;
			vertices.push_back(Vertex{ (leftBottom + downOffset) });

			rect.RightBottomIdx = rect.LeftTopIdx + 3;
			vertices.push_back(Vertex{ (rightBottom + downOffset) });
		}


		/*-------------------------------------------------
		 *   기둥의 맨위쪽/아래쪽 사각형의 연결 정보를 구성한다..
		 ********/
		const uint32_t rectStartIdxTable[] = { 0,3 };

		for (uint32_t i = 0; i < 2; i++) {
			const Rect& rect = outPillar.rects[rectStartIdxTable[i]];

			triangles.push_back(Triangle{ rect.LeftTopIdx, rect.RightTopIdx, rect.RightBottomIdx });
			triangles.push_back(Triangle{ rect.LeftTopIdx, rect.LeftBottomIdx, rect.RightBottomIdx });
		}

		/*----------------------------------------------
		 *   기둥의 오른쪽/아래쪽 면의 연결 정보를 구성한다..
		 *******/
		for (uint32_t i = 0; i < 3; i++) {
			const Rect& from = outPillar.rects[i];
			const Rect& to = outPillar.rects[i + 1];

			triangles.push_back(Triangle{ from.LeftBottomIdx, from.RightBottomIdx, to.RightBottomIdx });
			triangles.push_back(Triangle{ from.LeftBottomIdx, to.LeftBottomIdx, to.RightBottomIdx });

			triangles.push_back(Triangle{ from.RightBottomIdx, from.RightTopIdx, to.RightTopIdx });
			triangles.push_back(Triangle{ from.RightBottomIdx, to.RightBottomIdx, to.RightTopIdx });
		}


		/*----------------------------------------------
		 *   중간 기둥의 앞쪽/왼쪽의 연결 정보를 구성한다..
		 ********/
		const Rect& middle_top = outPillar.rects[1];
		const Rect& middle_bottom = outPillar.rects[2];

		triangles.push_back(Triangle{ middle_top.RightTopIdx, middle_top.LeftTopIdx, middle_bottom.LeftTopIdx });
		triangles.push_back(Triangle{ middle_top.RightTopIdx, middle_bottom.LeftTopIdx, middle_bottom.RightTopIdx });

		triangles.push_back(Triangle{ middle_top.LeftTopIdx, middle_top.LeftBottomIdx, middle_bottom.LeftBottomIdx });
		triangles.push_back(Triangle{ middle_top.LeftTopIdx, middle_bottom.LeftBottomIdx, middle_bottom.LeftTopIdx });
	};






	/***************************************************************************
	 *   4개의 기둥 데이터를 생성한다....
	 *********/
	J_Pillar leftTop, rightTop, leftBottom, rightBottom;

	AddPillar(rightBottom, Vector3(max.x, max.y, min.z) + Vector3(-rectSizeHalf.x, 0.f, rectSizeHalf.y), 0.f);
	AddPillar(rightTop, Vector3(max.x, max.y, max.z) + Vector3(-rectSizeHalf.x, 0.f, -rectSizeHalf.y), 90.f);
	AddPillar(leftTop, Vector3(min.x, max.y, max.z) + Vector3(rectSizeHalf.x, 0.f, -rectSizeHalf.y), 180.f);
	AddPillar(leftBottom, Vector3(min.x, max.y, min.z) + Vector3(rectSizeHalf.x, 0.f, rectSizeHalf.y), 270.f);




	/******************************************************************************
	 *   각 기둥들을 연결시키는 람다 함수를 정의한다....
	 *********/
	const auto ConnectPillar = [&](const J_Pillar& up, const J_Pillar& left)
	{
		/*-----------------------------------------------
		 *   from의 앞쪽면의 포트를 연결시킨다...
		 *-----*/

		 //연결면의 세로면들을 처리한다....
		for (uint32_t i = 0; i < 4; i++) {
			const Rect& fromRect = up.rects[i];
			const Rect& toRect   = left.rects[i];

			triangles.push_back(Triangle{ fromRect.LeftTopIdx,toRect.LeftTopIdx, fromRect.RightTopIdx });
			triangles.push_back(Triangle{ toRect.LeftTopIdx, toRect.LeftBottomIdx, fromRect.RightTopIdx });
		}

		//연결면의 가로면들을 처리한다...
		const uint32_t sideRectIdxTable[] = { 0, 2 };

		for (uint32_t i = 0; i < 2; i++) {
			const Rect& fromRect1 = up.rects[sideRectIdxTable[i]];
			const Rect& toRect1   = left.rects[sideRectIdxTable[i]];

			const Rect& fromRect2 = up.rects[sideRectIdxTable[i] + 1];
			const Rect& toRect2   = left.rects[sideRectIdxTable[i] + 1];

			triangles.push_back(Triangle{ fromRect1.RightTopIdx, toRect1.LeftBottomIdx, fromRect2.LeftBottomIdx });
			triangles.push_back(Triangle{ fromRect1.RightTopIdx, toRect2.LeftBottomIdx, fromRect2.RightTopIdx });

			triangles.push_back(Triangle{ fromRect1.LeftTopIdx, toRect1.LeftTopIdx, fromRect2.LeftTopIdx });
			triangles.push_back(Triangle{ fromRect2.LeftTopIdx, toRect1.LeftTopIdx, toRect2.LeftTopIdx });
		}
	};


	/************************************************************************
	 *  4 기둥들의 포트끼리 연결시킨다...
	 *******/
	ConnectPillar(rightBottom, rightTop);
	ConnectPillar(leftBottom, rightBottom);
	ConnectPillar(leftTop, leftBottom);
	ConnectPillar(rightTop, leftTop);

}











/*====================================================================================================================
 *    메시의 바운딩 스피어를 기반으로 메시를 생성합니다....
 *************/
void hyunwoo::Mesh::CreateBoundingSphereMesh(Mesh& outMesh)
{
	std::vector<Vertex>& vertices    = outMesh.Vertices;
	std::vector<Triangle>& triangles = outMesh.Triangles;

	vertices.clear();
	triangles.clear();


	/*********************************************************
	 *   주어진 회전축의 사각형 메시를 생성하는 람다함수의 정의..
	 ******/
	const auto AddRectangles = [&](const Vector3& dirDir, const Vector3& upDir, const Vector3& rotAxis)
	{
		float         angle     = 0.f;
		const float   addAngle  = 20.f;
		const float   goalAngle = (360.f - addAngle);
		const Vector3 forward   = (dirDir * BoundSphere.Radius);
		const Vector3 back	    = (forward * .95f);
		const Vector3 up	    = (upDir * 0.15f);

		
		//회전되지 않은 상/하 버텍스를 삽입한다...
		vertices.push_back(Vertex{ BoundSphere.Center + (forward + up), Vector2::Zero });
		vertices.push_back(Vertex{ BoundSphere.Center + (forward - up), Vector2::Zero });
		vertices.push_back(Vertex{ BoundSphere.Center + (back + up), Vector2::Zero });
		vertices.push_back(Vertex{ BoundSphere.Center + (back - up), Vector2::Zero });

		const uint32_t first_forward_up   = (vertices.size() - 4);
		const uint32_t first_forward_down = (first_forward_up + 1);
		const uint32_t first_back_up	  = (first_forward_up + 2);
		const uint32_t first_back_down	  = (first_forward_up + 3);


		//나머지 사각형을 구성한다...
		while (angle < goalAngle) {
			const uint32_t last_forward_up   = (vertices.size() - 4);
			const uint32_t last_forward_down = (last_forward_up + 1);
			const uint32_t last_back_up		 = (last_forward_up + 2);
			const uint32_t last_back_down	 = (last_forward_up + 3);

			const uint32_t cur_forward_up	= (last_forward_up + 4);
			const uint32_t cur_forward_down = (last_forward_up + 5);
			const uint32_t cur_back_up		= (last_forward_up + 6);
			const uint32_t cur_back_down	= (last_forward_up + 7);

			const Vector3  rotForward = (BoundSphere.Center + (Quaternion::AngleAxis(angle += addAngle, rotAxis) * forward));
			const Vector3  rotBack	  = (BoundSphere.Center + (Quaternion::AngleAxis(angle, rotAxis) * back));

			vertices.push_back(Vertex{ (rotForward + up), Vector2::Zero });
			vertices.push_back(Vertex{ (rotForward - up), Vector2::Zero });
			vertices.push_back(Vertex{ (rotBack + up), Vector2::Zero });
			vertices.push_back(Vertex{ (rotBack - up), Vector2::Zero });

			//앞쪽의 연결방식을 구성한다...
			triangles.push_back(Triangle{ cur_forward_up, last_forward_up, cur_forward_down });
			triangles.push_back(Triangle{ cur_forward_down, last_forward_up, last_forward_down });

			//뒤쪽의 연결방식을 구성한다...
			triangles.push_back(Triangle{ cur_back_up, last_back_up, cur_back_down });
			triangles.push_back(Triangle{ cur_back_down, last_back_up, last_back_down });

			//위쪽의 연결방식을 구성한다...
			triangles.push_back(Triangle{ cur_forward_up, last_forward_up, cur_back_up });
			triangles.push_back(Triangle{ cur_back_up, last_forward_up, last_back_up });
	
			//아래쪽의 연결방식을 구성한다...
			triangles.push_back(Triangle{ cur_forward_down, last_forward_down, cur_back_down });
			triangles.push_back(Triangle{ cur_back_down, last_forward_down, last_back_down });
		}


		//마지막 사각형을 구성한다...
		const uint32_t last_forward_up   = (vertices.size() - 4);
		const uint32_t last_forward_down = (last_forward_up + 1);
		const uint32_t last_back_up		 = (last_forward_up + 2);
		const uint32_t last_back_down	 = (last_forward_up + 3);

		//앞쪽의 연결방식을 구성한다...
		triangles.push_back(Triangle{ first_forward_up, last_forward_up, first_forward_down });
		triangles.push_back(Triangle{ first_forward_down, last_forward_up, last_forward_down });

		//뒤쪽의 연결방식을 구성한다...
		triangles.push_back(Triangle{ first_back_up, last_back_up, first_back_down });
		triangles.push_back(Triangle{ first_back_down, last_back_up, last_back_down });

		//위쪽의 연결방식을 구성한다...
		triangles.push_back(Triangle{ first_forward_up, last_forward_up, first_back_up });
		triangles.push_back(Triangle{ first_back_up, last_forward_up, last_back_up });
		
		//아래쪽의 연결방식을 구성한다...
		triangles.push_back(Triangle{ first_forward_down, last_forward_down, first_back_down });
		triangles.push_back(Triangle{ first_back_down, last_forward_down, last_back_down });
	};


	/****************************************************
	 *  세 축에 대한 사각형을 구성한다...
	 ******/
	AddRectangles(Vector3::Forward, Vector3::Up, Vector3::Up);
	AddRectangles(Vector3::Forward, Vector3::Right, Vector3::Right);
	AddRectangles(Vector3::Right, Vector3::Forward, Vector3::Forward);
}
