#include "RenderEngine.h"
#include <windowsx.h>
using KeyCode  = hyunwoo::InputManager::KeyCode;
using KeyState = hyunwoo::InputManager::KeyState;

/*================================================================================================================================================
 *   정적 필드의 초기화를 진행한다....
 *========*/
hyunwoo::RenderEngine* hyunwoo::RenderEngine::m_runningEnginePtr = nullptr;









/*==================================================================================================================================================
 *   엔진을 초기화하고, 프로그램을 진행하는 블록 함수입니다...
 *========*/
hyunwoo::RenderEngine::EndReason hyunwoo::RenderEngine::Run(const std::wstring& appName, HINSTANCE hInstance, LPWSTR commandLine, int bShowCmd)
{
	RECT      rect;				  //랜더러를 초기화할 때, 클라이언트의 크기를 가져오기위해 사용됩니다...
	WNDCLASS  wc        = { 0, }; //생성할 윈도우 창들이 공통적으로 가질 특성을 정의하는 구조체입니다...
	EndReason endReason = { 0, }; //해당 함수가 왜 종료되었는지에 대한 이유를 담는 구조체입니다...


	/*******************************************************
	 *   이미 엔진이 실행중인가? 맞다면 종료 이유를 갱신하고 
	 *   함수를 종료한다...
	 ******/
	if (m_runningEnginePtr!=nullptr) {
		endReason.EngineIsAlreadyRun = true;
		return endReason;
	}


	/*********************************************************
	 *   동일한 창 프로시저에서 생성되는 윈도우 창들의 공통 속성들을 
	 *   나타내는 WNDCLASS 구조체를 작성한다....
	 *********/
	wc.lpfnWndProc   = WndProc;
	wc.lpszClassName = appName.c_str();
	wc.lpszMenuName  = NULL;
	wc.style		 = (CS_HREDRAW | CS_VREDRAW);
	wc.hInstance     = hInstance;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION);

	
	/***********************************************************
	 *    작성한 WNDCLASS를 운영체제에 등록하는게 실패했는가? 
	 *    맞다면 이유를 갱신하고 함수를 종료한다..   
	 *********/
	if (RegisterClassW(&wc)==NULL) {
		endReason.RegisterWndClassIsFail = true;
		return endReason;
	}


	/******************************************************************
	 *    윈도우 창을 생성하고, 생성한 창에 대한 핸들을 얻는데 실패했는가?
	 *    맞다면 이유를 갱신하고 함수를 종료한다...
	 ********/  

	if ((m_mainHwnd = CreateWindowW(
		appName.c_str(), 
		appName.c_str(), 
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL))==NULL)
	{
		endReason.GetWindowHandleIsFail = true;
		return endReason;
	}



	/*****************************************************************
	 *   랜더러를 초기화한다....
	 ********/
	GetClientRect(m_mainHwnd, &rect);

	//랜더러 초기화에 실패했는가? 이유를 갱신하고 함수를 종료한다...
	if ((endReason.RenderTargetInitRet = m_viewPort.RenderTarget.Init(GetClientHwnd(), GetClientSize())).InitSuccess==false) {
		return endReason;
	}

	m_viewPort.ClientRect.LeftTop     = Vector2Int(rect.left, rect.top);
	m_viewPort.ClientRect.RightBottom = Vector2Int(rect.right, rect.bottom);


	/******************************************************************
	 *   초기화가 완료된 RenderEngine 객체의 포인터를 갱신하고,
	 *   윈도우 창을 화면에 표시한다....
	 *******/
	m_runningEnginePtr = this;
	m_hInstance		   = hInstance;

	DragAcceptFiles(m_mainHwnd, TRUE); //파일의 드로그 앤 드롭을 허용한다...
	ShowWindow(m_mainHwnd, bShowCmd);
	OnStart();



	/******************************************************************
	 *    초기화가 완료도었다면, 메인 루프를 진행한다...
	 ********/
	MSG       msg         = { 0, };
	float     elapsedTime = 0.f;
	TimePoint prevTime    = HighClock::now();

	while (msg.message!=WM_QUIT) 
	{
		/*--------------------------------------------
		 *   윈도우 큐에 있는 메세지를 모두 처리한다....
		 *---------*/
		if (PeekMessage(&msg, 0,0,0,PM_REMOVE)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}


		/*---------------------------------------------
		 *   프레임 진입 이벤트를 처리한다....
		 *----------*/
		TimePoint now       = HighClock::now();
		float     deltaTime = std::chrono::duration_cast<FloatDuration>(now-prevTime).count();

		prevTime = now;

		//프레임 진입 시간에 도달했을 때, 프레임 진입에 대한 처리를 시작한다...
		if ((elapsedTime += deltaTime) >= m_frameEntryTime) {

			//화면을 특정 색깔로 초기화하는가?
			if (m_renderer.UseAutoClear) {
				m_renderer.ClearScreen(m_viewPort);
			}

			OnEnterFrame(elapsedTime);
			InvalidateRect(m_mainHwnd, NULL, false);

			/**키 상태들의 단일 처리값들을 무효화한다...**/
			for (int i = 0; i < 255; i++) {
				KeyState& state            = m_inputManager.GetKeyState(KeyCode(i));
				state.WasPressedThisFrame  = false;
				state.WasReleasedThisFrame = false;
			}

			elapsedTime = 0.f;
		}
	}



	/*******************************************
	 *   프로그램을 마무리 짓는다...
	 *******/
	m_runningEnginePtr    = nullptr;
	endReason.QuitMessage = true;

	return endReason;
}












/*===================================================================================================================================================
 *   윈도우로부터 전달받은 메세지를 처리하는 메소드입니다...
 *========*/
LRESULT hyunwoo::RenderEngine::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	/********************************************************************
	 *   실행중인 엔진의 참조가 없다면, 함수를 종료한다....
	 ******/
	if (m_runningEnginePtr == nullptr) {
		return DefWindowProcW(hWnd, msg, wParam, lParam);
	}



	/*****************************************************************
	 *   메세지 종류에 따라 적절하게 처리한다...
	 *********/
	RenderEngine& engine = *m_runningEnginePtr;

	switch (msg) 
	{
		/*------------------------------------
		 *  윈도우가 생성되었을 경우....
		 *****/
		case(WM_CREATE): {
			break;
		}


		/*------------------------------------
		 *  화면을 갱신해야하는 경우...
		 *****/
		case(WM_PAINT): {
			engine.m_renderer.Present(hWnd, engine.m_viewPort);
			break;
		}


		/*------------------------------------
		 *  키보드의 버튼이 눌렸을 경우....
		 *****/
		case(WM_KEYDOWN): 
		{
			KeyState& state  = engine.m_inputManager.GetKeyState(KeyCode(wParam));

			if (state.IsInProgress == false) {
				state.WasPressedThisFrame = true;
			}

			state.IsInProgress = true;
			break;
		}


		/*------------------------------------
		 *  키보드의 버튼이 떼어졌을 경우...
		 *****/
		case(WM_KEYUP): 
		{
			KeyState& state            = engine.m_inputManager.GetKeyState(KeyCode(wParam));
			state.IsInProgress         = false;
			state.WasReleasedThisFrame = true;
			break;
		}


		/*------------------------------------
		 *  마우스의 왼쪽 버튼이 눌렸을 경우...
		 *****/
		case(WM_LBUTTONDOWN): 
		{
			KeyState& state = engine.m_inputManager.GetKeyState(KeyCode::Left_Mouse);

			engine.m_inputManager.SetMouseScreenPosition(Vector2(
				GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))
			);

			if (state.IsInProgress == false) {
				state.WasPressedThisFrame = true;
			}

			state.IsInProgress = true;
			break;
		}


		/*------------------------------------
		 *  마우스의 왼쪽 버튼이 떼어졌을 경우..
		 *****/
		case(WM_LBUTTONUP): 
		{
			KeyState& state = engine.m_inputManager.GetKeyState(KeyCode::Left_Mouse);

			engine.m_inputManager.SetMouseScreenPosition(Vector2(
				GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))
			);

			state.IsInProgress		   = false;
			state.WasReleasedThisFrame = true;
			break;
		}


		/*------------------------------------
		 *  마우스의 오른쪽 버튼이 눌렸을 경우...
		 *****/
		case(WM_RBUTTONDOWN): 
		{
			KeyState& state = engine.m_inputManager.GetKeyState(KeyCode::Right_Mouse);

			engine.m_inputManager.SetMouseScreenPosition(Vector2(
				GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))
			);

			if (state.IsInProgress == false) {
				state.WasPressedThisFrame = true;
			}

			state.IsInProgress = true;
			break;
		}


		/*------------------------------------
		 *  마우스의 오른쪽 버튼이 떼어졌을 경우..
		 *****/
		case(WM_RBUTTONUP): 
		{
			KeyState& state = engine.m_inputManager.GetKeyState(KeyCode::Right_Mouse);

			engine.m_inputManager.SetMouseScreenPosition(Vector2(
				GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))
			);

			state.IsInProgress = false;
			state.WasReleasedThisFrame = true;
			break;
		}


		/*------------------------------------
		 *  마우스를 움직였을 경우...
		 *****/
		case(WM_MOUSEMOVE): 
		{
			engine.m_inputManager.SetMouseScreenPosition(Vector2(
				GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))
			);

			break;
		}


		/*------------------------------------
		 *  윈도우 창이 파괴되었을 경우....
		 *****/
		case(WM_DESTROY): 
		{
			PostQuitMessage(0);
			return 0;
		}


		/*-----------------------------------
		 *  파일이 드롭되었을 경우....
		 ******/
		case(WM_DROPFILES):
		{
			HDROP hDrop = (HDROP)wParam;
			UINT  fileCount = DragQueryFile(hDrop, 0xFFFFFFFF, nullptr, 0);

			wchar_t filePath[MAX_PATH];
			for (uint32_t i = 0; i < fileCount; i++) {
				DragQueryFile(hDrop, i, filePath, MAX_PATH);
				engine.OnFileDropped(filePath);
			}

			break;
		}
	}


	return DefWindowProcW(hWnd, msg, wParam, lParam);
}










/*====================================================================
 *   랜더 엔진이 실행 중인지를 확인합니다...
 *==========*/
bool hyunwoo::RenderEngine::EngineIsRunning() const {
	return (m_runningEnginePtr != nullptr);
}







/*====================================================================
 *   현재 랜더 엔진 클라이언트 창의 크기를 얻습니다...
 *==========*/
hyunwoo::Vector2Int hyunwoo::RenderEngine::GetClientSize() const
{
	RECT rect;
	GetClientRect(m_mainHwnd, &rect);

	return Vector2Int(
		(rect.right - rect.left), 
		(rect.bottom - rect.top)
	);
}










/*======================================================================
 *    랜더 엔진의 타깃 프레임을 설정 및 확인합니다....
 *===========*/
int hyunwoo::RenderEngine::GetTargetFrameRate() const 
{
	return m_targetFrameRate;
}

void hyunwoo::RenderEngine::SetTargetFrameRate(int newTargetFrameRate) 
{
	/****************************************
	 *   타깃 프레임을 갱신하고, 
	     적절한 프레임 진입 시간값을 캐싱합니다..
	 *******/
	m_targetFrameRate = (newTargetFrameRate < 0 ? 0 : newTargetFrameRate);
	m_frameEntryTime  = (1.f / newTargetFrameRate);
}











/*=============================================================================
 *    참조를 얻어오는 메소드들입니다....
 *=============*/
const hyunwoo::InputManager& hyunwoo::RenderEngine::GetInputManager() const {
	return m_inputManager;
}

HWND hyunwoo::RenderEngine::GetClientHwnd() const
{
	return m_mainHwnd;
}

hyunwoo::Renderer& hyunwoo::RenderEngine::GetRenderer() {
	return m_renderer;
}
