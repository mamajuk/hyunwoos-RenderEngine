#pragma once
#include <windows.h>
#include "../MathModule/Color.h"
#include "../MathModule/Vector.h"
#include "RenderResources.h"

namespace hyunwoo {
	class Renderer;
}


/*====================================================================================================
 *    그래픽 출력에 관한 메소드들을 제공하는 클래스입니다....
 *==============*/
class hyunwoo::Renderer final
{
	//====================================================================
	///////							Defines..				      ////////
	//====================================================================
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



	//===================================================================
	////////					 Properties..				    /////////
	//===================================================================
public:
	/****************************************
	 *   랜더러 상태 관련 프로퍼티...
	 ******/
	bool IsInit() const;


	/****************************************
	 *   화면 클리어 관련 프로퍼티....
	 ******/
	bool        UseAutoClear     : 1 = true;
	bool		UseWireFrameMode : 1 = false;
	Color		WireFrameColor       = LinearColor::Black;
	Color		ClearColor           = LinearColor::White;


	/*************************************
	 *    비트맵 크기 관련 프로퍼티....
	 ******/
	UINT GetWidth()  const;
	UINT GetHeight() const;





	//==========================================================================
	/////////							Fields...					   /////////
	//==========================================================================
private:
	HDC     m_memDC      = NULL;
	bool    m_isInit : 1 = false;
	HWND    m_renderTargetHWND;


	/****************************
	 *   비트맵 관련 필드....
	 ******/
	UINT m_width         = 0;
	UINT m_height        = 0;
	UINT m_totalPixelNum = 0;
	float m_widthf	     = 0.f;
	float m_heightf      = 0.f;
	float m_widthf_half  = 0;
	float m_heightf_half = 0;

	HBITMAP m_backBufferBitmap     = NULL;
	HBITMAP m_oldBitmap			   = NULL;
	DWORD*  m_backBufferBitmapPtr  = nullptr;
	




	//============================================================
	//////////				Public methods..			 /////////
	//============================================================
public:
	InitResult Init(HWND renderTargetHwnd, UINT initWidth, UINT initHeight);


	/***************************
	 *   좌표계 변환 메소드....
	 ******/
	Vector2 WorldToScreen(const Vector2& cartesianPos);
	Vector2 ScreenToWorld(const Vector2& screenPos);
	

	/*******************************
	 *   그래픽 출력 관련 메소드....
	 ******/
	void Present();
	void ClearScreen();
	void DrawTextField(const std::wstring& out, const Vector2Int& screenPos);
	void SetPixel(const Color& color, const Vector2Int& screenPos);
	void DrawLine(const Color& color, const Vector2& startScreenPos, const Vector2& endScreenPos, bool useClipping = true);
	void DrawTriangle(const Color& color, const Vector2& screenPos1, const Vector2& screenPos2, const Vector2& screenPos3);
	void DrawTriangleWithTexture(const Texture2D& texture, const Vector2& screenPos1, const Vector2& uvPos1, const Vector2& screenPos2, const Vector2& uvPos2, const Vector2& screenPos3, const Vector2& uvPos3);
};