#pragma once
#include <windows.h>
#include "../MathModule/Color.h"
#include "../MathModule/Vector.h"
#include "Material.h"
#include "Camera.h"
#include "RenderTarget.h"
#include "RenderMesh.h"

namespace hyunwoo {
	class Renderer;
}


/*==========================================================================================================================================================
 *    그래픽 출력에 관한 메소드들을 제공하는 클래스입니다....
 *************/
class hyunwoo::Renderer final
{
	//==========================================================================================
	///////									 Defines..									////////
	//==========================================================================================
public:
	/**********************************************
	 *   랜더링 결과물이 표시될 화면 영역을 서술하는 
	 *   클래스입니다....
	 ***********/
	struct ViewPort final
	{
		struct ClientRect final 
		{
			Vector2Int LeftTop;
			Vector2Int RightBottom;
		};

		WeakPtr<Camera> RenderCamera;
		RenderTarget    RenderTarget;
		ClientRect		ClientRect;
	};


	/**************************************************
	 *   삼각형 클립핑에 필요한 별칭과 구조체들입니다...
	 *******/
	using ClippingTestFunc = bool(const Vector4& clipPos);
	using SolveTFunc	   = float(const Vector4& fromClipPos, const Vector4& toClipPos);

	struct ClipVertex
	{
		Vector4 ClipPos;
		Vector2 UvPos;

		ClipVertex() = default;
		ClipVertex(const Vector4& clipPos, const Vector2& uvPos) :ClipPos(clipPos), UvPos(uvPos) {}
	};

	struct ClipTriangle
	{
		ClipVertex Vertices[3];

		ClipTriangle() = default;
		ClipTriangle(const ClipVertex& clip_vertex1, const ClipVertex& clip_vertex2, const ClipVertex& clip_vertex3) : Vertices{ clip_vertex1, clip_vertex2, clip_vertex3 } {}
	};

	struct ClipTriangleList
	{
		uint32_t	 triangleCount;
		ClipTriangle Triangles[20];
	};


	/*********************************************
	 *  그릴 삼각형을 서술하는 구조체입니다...
	 ********/
	struct TriangleDescription
	{
		float			 Depths[3]			= { 0, };
		Color			 FillUpColor		= Color::Pink;
		Vector2			 ScreenPositions[3] = { 0, };
		Vector2		     Uvs[3]				= { 0, };
		const Texture2D* MappedTexture		= nullptr;

		inline void SetScreenPositions(const Vector2& screenPos1, const Vector2& screenPos2, const Vector2& screenPos3);
		inline void SetUvPositions(const Vector2& uvPos1, const Vector2& uvPos2, const Vector2& uvPos3);
		inline void SetDepths(const float depth1, const float depth2, const float depth3);
	};





	//========================================================================================
	////////								Properties..							/////////
	//=======================================================================================
public:
	/****************************************
	 *   화면 클리어 관련 프로퍼티....
	 ******/
	bool UseAutoClear       : 1 = true;
	bool UseWireFrameMode   : 1 = false;
	bool UseBackfaceCulling : 1 = true;
	bool UseAlphaBlending   : 1 = true;
	bool DrawTriangleNormal : 1 = false;

	Color WireFrameColor       = Color::Black;
	Color ClearColor           = Color::White;
	Color InvalidTriangleColor = Color::Pink;





	//===============================================================================
	//////////						  Public methods..						/////////
	//===============================================================================
public:
	Renderer()					  = default;
	Renderer(const Renderer&)	  = default;
	Renderer(Renderer&&) noexcept = default;
	~Renderer()					  = default;



	/*********************************
	 *   좌표계 변환 메소드....
	 ******/
	Vector2 NDCToScreen(const Vector4& ndcPos, const ViewPort& vp);
	Vector3 ClipToNDC(const Vector4& clipPos);
	Vector2 WorldToScreen(const Vector2& cartesianPos, const ViewPort& vp);
	Vector2 ScreenToWorld(const Vector2& screenPos, const ViewPort& vp);
	


	/********************************
	 *   클립핑 관련 메소드...
	 *******/
	static bool  ClippingTest_Far(const Vector4& clipPos);
	static bool  ClippingTest_Near(const Vector4& clipPos);
	static bool  ClippingTest_Right(const Vector4& clipPos);
	static bool  ClippingTest_Left(const Vector4& clipPos);
	static bool  ClippingTest_Up(const Vector4& clipPos);
	static bool  ClippingTest_Down(const Vector4& clipPos);

	static float SolveT_Far(const Vector4& fromClipPos, const Vector4& toClipPos);
	static float SolveT_Near(const Vector4& fromClipPos, const Vector4& toClipPos);
	static float SolveT_Right(const Vector4& fromClipPos, const Vector4& toClipPos);
	static float SolveT_Left(const Vector4& fromClipPos, const Vector4& toClipPos);
	static float SolveT_Up(const Vector4& fromClipPos, const Vector4& toClipPos);
	static float SolveT_Down(const Vector4& fromClipPos, const Vector4& toClipPos);

	void ClippingTriangle(ClipTriangleList& clipTriangleList, ClippingTestFunc* clippingTestFunc, SolveTFunc* solveTFunc);



	/*******************************
	 *   그래픽 출력 관련 메소드....
	 ******/
	void Present(HWND clientHwnd, const ViewPort& vp);
	void ClearScreen(const ViewPort& vp);
	void DrawTextField(const std::wstring& out, const Vector2Int& screenPos, const ViewPort& vp);
	void SetPixel(const Color& color, const Vector2Int& screenPos, const ViewPort& vp);
	void DrawLine(const Color& color, const Vector2& startScreenPos, const Vector2& endScreenPos, const ViewPort& vp, bool useClipping = true);
	void DrawTriangle(const TriangleDescription& triangleDesc, const ViewPort& vp);
	void DrawRenderMesh(const RenderMesh& renderMesh, const ViewPort& vp);






	//=================================================================================
	////////////					   Private methods..				 //////////////
	//=================================================================================
private:
	inline void SetPixel_internal(const Color& color, const uint32_t index, const float depth, const ViewPort& vp);
	inline void DrawClipTriangle_internal(const ClipTriangle& clipTriangle, const Vector3& normal, Shader::FragmentShaderFunc* fragmentShader, const Texture2D& tex, const ViewPort& vp);
};