#pragma once
#include <Windows.h>
#include <string>
#include <chrono>
#include "InputManager.h"
#include "../RenderModule/Renderer.h"

namespace hyunwoo{
	class RenderEngine;
}




/*========================================================================
 *   필수 모듈을 총괄하여 프로그램을 구성하는 기반 클래스입니다...
 *============*/
class hyunwoo::RenderEngine
{
	//======================================================
	/////////			   Defines..			  //////////
	//======================================================
public:

	/*****************************************
	 *  엔진의 종료 이유를 저장하는 구조체입니다..
	 *****/
	struct EndReason
	{
		bool EngineIsAlreadyRun		: 1;
		bool RegisterWndClassIsFail : 1;
		bool GetWindowHandleIsFail  : 1;
		bool QuitMessage			: 1;

		Renderer::InitResult RendererInitResult;
	};
	

	/*******************************
	 *    타이머 관련 별칭 정의....
	 *****/
	using HighClock     = std::chrono::high_resolution_clock;
	using TimePoint     = std::chrono::high_resolution_clock::time_point;
	using FloatDuration = std::chrono::duration<float>;






	//======================================================
	/////////			  Properties...			  //////////
	//======================================================
public:
	/*******************************
	 *   엔진 상태 관련 프로퍼티....
	 *******/
	bool EngineIsRunning() const;


	/*************************************
	 *   타깃 프레임 레이트 관련 프로퍼티...
	 *****/
	int  GetTargetFrameRate() const;
	void SetTargetFrameRate(int newTargetFrameRate);


	/*************************************
	 *   참조를 얻는 프로퍼티...
	 *****/
	const InputManager& GetInputManager() const;
	Renderer&			GetRenderer();





	//====================================================
	/////////			   Fields...			 /////////
	//====================================================
private:
	static RenderEngine* m_runningEnginePtr;

	/**********************************
	 *   WIN32 API 관련 필드...
	 ******/
	HWND      m_mainHwnd  = NULL; //메인 윈도우 창의 핸들입니다...
	HINSTANCE m_hInstance = NULL; //윈도우 프로세스의 인스턴스 핸들입니다....


	/**********************************
	 *   타깃 프레임 관련 필드...
	 *****/
	int     m_targetFrameRate = 60;			  //초당 목표 프레임 수입니다..
	float   m_frameEntryTime  = (1.f / 60.f); //프레임 진입에 필요한 시간입니다....


	/***********************************
	 *   엔진 코어 모듈 관련 필드...
	 *****/
	Renderer     m_renderer;	 //랜더링 기능을 책임지는 모듈입니다...
	InputManager m_inputManager; //사용자 입력을 책임지는 모듈입니다...






	//============================================================
	/////////		    Pure Virtual methods..			 /////////
	//============================================================
protected:
	virtual void OnStart()                     = 0;
	virtual void OnEnterFrame(float deltaTime) = 0;






	//====================================================
	/////////		    Public methods..		//////////
	//====================================================
public:
	EndReason Run(const std::wstring& appName, HINSTANCE hInstance, LPWSTR commandLine, int bShowCmd);






	//========================================================
	/////////			 Private methods..			//////////
	//========================================================
private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

};
