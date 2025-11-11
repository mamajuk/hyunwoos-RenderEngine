#pragma once
#include "Camera.h"
#include "../EngineModule/UniqueableObject.h"
#include "../MathModule/Vector.h"
#include <windows.h>

namespace hyunwoo {
	class RenderTarget;
}



/*==================================================================================================================================================================
 *   그려진 데이터를 담을 백버퍼 데이터를 나타내는 구조체입니다....
 ***********/
struct hyunwoo::RenderTarget final
{
	//===========================================================================================
	////////////								Defines..							/////////////
	//===========================================================================================
public:
	/********************************************
	 *  랜더 타깃의 초기화 결과가 담긴 구조체....
	 ******/
	struct InitResult final
	{
		bool InitSuccess		      : 1;
		bool CreateBackBufferIsFailed : 1;
		bool CreateMemDCIsFailed      : 1;
		bool InvalidBackBufferSize    : 1;
	};



	//===========================================================================================
	////////////								Fields..							/////////////
	//===========================================================================================
private:
	bool m_isInit = false;

	UINT  m_totalPixelNum     = 0;
	UINT  m_backBufferWidth	  = 0;
	UINT  m_backBufferHeight  = 0;
	float m_backBufferWidthf  = 0.f;
	float m_backBufferHeightf = 0.f;
	float m_aspectRatio		  = 0.f;

	HDC		m_memDC				  = NULL;
	HBITMAP m_backBufferBitmap    = NULL;
	HBITMAP m_oldBitmap		      = NULL;
	DWORD*  m_backBufferBitmapPtr = nullptr;
	float*  m_depthBufferPtr	  = nullptr;



	//===============================================================================================
	////////////							  Properties..							    /////////////
	//===============================================================================================
public:
	bool IsInit() const { return m_isInit; }
	
	UINT GetTotalPixelNum() const { return m_totalPixelNum; }

	UINT GetBackBufferWidth() const { return m_backBufferWidth; }
	UINT GetBackBufferHeight() const { return m_backBufferHeight; }

	float GetAspectRatio() const { return m_aspectRatio; }
	float GetBackBufferWidthf() const { return m_backBufferWidthf; }
	float GetBackBufferHeightf() const { return m_backBufferHeightf; }

	Vector2Int GetBackBufferSize() const { return Vector2Int(m_backBufferWidth, m_backBufferHeight); }
	Vector2    GetBackBufferSizef() const { return Vector2(m_backBufferWidthf, m_backBufferHeightf); }

	HDC		GetMemoryDC()		   const { return m_memDC; }
	HBITMAP GetBackBufferBitmap()  const { return m_backBufferBitmap; }
	DWORD*  GetBackBufferPixels()  const { return m_backBufferBitmapPtr; }
	float*  GetDepthBufferValues() const { return m_depthBufferPtr; }



	//===============================================================================================
	////////////							Public methods..							/////////////
	//===============================================================================================
public:
	RenderTarget() = default;
	RenderTarget(const RenderTarget& prev) {}
	RenderTarget(RenderTarget&& prev) noexcept { operator=(std::move(prev)); }
	~RenderTarget() { Clear(); }

	void operator=(const RenderTarget& prev) {}
	void operator=(RenderTarget&& prev) noexcept;

	InitResult Init(HWND clientHwnd, const Vector2Int& backBufferSize);



	//===============================================================================================
	////////////							Private methods..							/////////////
	//===============================================================================================
private:
	void Clear();
};




