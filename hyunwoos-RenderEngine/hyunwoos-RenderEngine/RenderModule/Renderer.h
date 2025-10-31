#pragma once
#include <windows.h>
#include "../MathModule/Color.h"
#include "../MathModule/Vector.h"
#include "Material.h"

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
	/********************************************
	 *   랜더러의 초기화 결과가 담긴 구조체입니다..
	 *****/
	struct InitResult
	{
		bool InitSuccess		  : 1;
		bool IsAlreadyInit		  : 1;
		bool CreateBitmapIsFailed : 1;
		bool CreateMemDCIsFailed  : 1;
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
		bool             WireFrameMode		= false;
		float			 Depths[3]			= { 0, };
		Vector2			 ScreenPositions[3] = { 0, };
		Vector2		     Uvs[3]				= { 0, };
		Color			 FillUpColor		= Color::Pink;
		const Texture2D* MappedTexture		= nullptr;

		void SetScreenPositions(const Vector2& screenPos1, const Vector2& screenPos2, const Vector2& screenPos3) {
			ScreenPositions[0] = screenPos1;
			ScreenPositions[1] = screenPos2;
			ScreenPositions[2] = screenPos3;
		}

		void SetUvPositions(const Vector2& uvPos1, const Vector2& uvPos2, const Vector2& uvPos3) {
			Uvs[0] = uvPos1;
			Uvs[1] = uvPos2;
			Uvs[2] = uvPos3;
		}

		void SetDepths(const float depth1, const float depth2, const float depth3) {
			Depths[0] = depth1;
			Depths[1] = depth2;
			Depths[2] = depth3;
		}
	};





	//========================================================================================
	////////								Properties..							/////////
	//=======================================================================================
public:
	/****************************************
	 *   랜더러 상태 관련 프로퍼티...
	 ******/
	bool IsInit() const;


	/****************************************
	 *   화면 클리어 관련 프로퍼티....
	 ******/
	bool        UseAutoClear       : 1 = true;
	bool		UseWireFrameMode   : 1 = false;
	bool		UseBackfaceCulling : 1 = true;
	bool		UseAlphaBlending   : 1 = true;
	bool		DrawTriangleNormal : 1 = false;

	Color		WireFrameColor         = Color::Black;
	Color		ClearColor             = Color::White;
	Color	    InvalidTriangleColor   = Color::Pink;


	/*************************************
	 *    비트맵 크기 관련 프로퍼티....
	 ******/
	UINT  GetWidth() const;
	UINT  GetHeight() const;

	float GetWidthf() const;
	float GetHeightf() const;

	float GetAspectRatio() const;







	//=======================================================================================
	/////////								Fields...								/////////
	//=======================================================================================
private:
	HDC     m_memDC			   = NULL;
	bool    m_isInit : 1	   = false;
	HWND    m_renderTargetHWND = NULL;


	/****************************
	 *   비트맵 관련 필드....
	 ******/
	UINT m_width         = 0;
	UINT m_height        = 0;
	UINT m_totalPixelNum = 0;
	float m_aspectRatio  = 0.f;
	float m_widthf	     = 0.f;
	float m_heightf      = 0.f;
	float m_widthf_half  = 0;
	float m_heightf_half = 0;

	HBITMAP m_backBufferBitmap     = NULL;
	HBITMAP m_oldBitmap			   = NULL;
	DWORD*  m_backBufferBitmapPtr  = nullptr;
	float*  m_depthBufferPtr	   = nullptr;
	






	//===============================================================================
	//////////						  Public methods..						/////////
	//===============================================================================
public:
	Renderer()				  = default;
	Renderer(const Renderer&) = delete;
	~Renderer();

	InitResult Init(HWND renderTargetHwnd, UINT initWidth, UINT initHeight);



	/*********************************
	 *   좌표계 변환 메소드....
	 ******/
	Vector2 NDCToScreen(const Vector4& ndcPos);
	Vector3 ClipToNDC(const Vector4& clipPos);
	Vector2 WorldToScreen(const Vector2& cartesianPos);
	Vector2 ScreenToWorld(const Vector2& screenPos);
	


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
	void Present();
	void ClearScreen();
	void DrawTextField(const std::wstring& out, const Vector2Int& screenPos);
	void SetPixel(const Color& color, const Vector2Int& screenPos);
	void DrawLine(const Color& color, const Vector2& startScreenPos, const Vector2& endScreenPos, bool useClipping = true);
	void DrawTriangle(const TriangleDescription& triangleDesc);






	//=================================================================================
	////////////					   Private methods..				 //////////////
	//=================================================================================
private:
	inline void SetPixel_internal(const Color& color, const uint32_t index, const float depth);
};