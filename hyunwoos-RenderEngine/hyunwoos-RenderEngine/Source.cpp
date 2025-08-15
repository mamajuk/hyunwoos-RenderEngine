#include <Windows.h>
#include "EngineModule/RenderEngine.h"
#include "UtilityModule/StringLamda.h"
#include "MathModule/Vector.h"
#include "MathModule/Matrix.h"
using namespace hyunwoo;
using KeyCode = hyunwoo::InputManager::KeyCode;


/*=========================================================================
 *    사용자 정의 랜더 엔진을 정의한다...
 *==========*/
class MyRenderEngine final : public RenderEngine
{
	//=======================================================
	////////			    Fields..				 ////////
	//=======================================================
private:
	int   frameCount  = 0;
	int   lastFps     = 0;
	float totalTime   = 0.f;

	float     degree   = 0.f;
	float     speed    = 100.f;
	float     rotSpeed = 50.f;
	Vector2   pos      = Vector2(100.f ,100.f);
	Vector2   pos2     = Vector2::Zero;
	Vector2   pos3     = Vector2::Zero;



	//==========================================================
	////////			 Override methods..				////////
	//==========================================================
protected:
	virtual void OnStart() override final
	{
		Renderer& renderer        = GetRenderer();
		renderer.UseAutoClear     = true;
		renderer.UseWireFrameMode = false;
		renderer.ClearColor       = LinearColor::Black;
		SetTargetFrameRate(60);
	}


	virtual void OnEnterFrame(float deltaTime) override final
	{
		Renderer&			renderer = GetRenderer();
		const InputManager& input    = GetInputManager();
		const float         speedSec = (deltaTime * speed);

		//DrawTriangle(deltaTime, speed, pos, pos2, pos3);
		DrawRectangle(degree, pos, speed, rotSpeed, deltaTime, Vector2Int(100, 100), LinearColor::White);
		DrawFps(deltaTime, frameCount, lastFps, totalTime);
	}




	//=========================================================
	////////			  Private methods..			  /////////
	//=========================================================
private:
	void DrawRectangle(float& degree, Vector2& worldPos, float moveSpeed, float rotSpeed, float deltaTime, Vector2Int size, const LinearColor& color)
	{
		const InputManager& input        = GetInputManager();
		Renderer&			renderer     = GetRenderer();
		const float         moveSpeedSec = (moveSpeed * deltaTime);
		const float         rotSpeedSec  = (rotSpeed * deltaTime);

		/****************************************************
		 *   사각형의 회전과 이동을 조작한다...
		 ********/
		worldPos += Vector2(
			input.GetAxis(KeyCode::Left, KeyCode::Right) * moveSpeedSec,
			input.GetAxis(KeyCode::Down, KeyCode::Up) * moveSpeedSec
		);

		degree += input.GetAxis(KeyCode::S, KeyCode::W) * rotSpeedSec;



		/****************************************************
		 *    사각형의 회전과 위치를 구성하는 행렬을 만든다....
		 ********/
		float c = Math::Cos(Math::Angle2Rad * degree);
		float s = Math::Sin(Math::Angle2Rad * degree);

		Matrix3x3 rotMat = Matrix3x3(
			Vector3(c, s, 0.f), 
			Vector3(-s, c, 0.f),
			Vector3(worldPos.x, worldPos.y, 1.f)
		);



		/***************************************************
		 *    주어진 위치에 회전된 사각형을 그립니다..
		 ********/
		const int goalY = size.y / 2;
		const int goalX = size.x / 2;

		for (int y = -goalY; y <= goalY; y++) {
			for (int x = -goalX; x <= goalX; x++) {

				const Vector3 finalVec = renderer.WorldToScreen(rotMat * Vector3(x, y, 1.f));
				renderer.SetPixel(color, finalVec);
			}
		}

		renderer.DrawTextField(w$(L"rotMat\n", rotMat), Vector2Int(0, 500));
	}
	void DrawFps(float deltaTime, int& frameCount, int& lastFps, float& totalTime) 
	{
		/*********************************************
		 *    측정한 초당 프레임을 표시한다....
		 *******/
		Renderer& renderer = GetRenderer();

		frameCount++;
		if ((totalTime += deltaTime) >= 1.f) {
			lastFps = frameCount;
			frameCount = 0;
			totalTime = 0.f;
		}

		renderer.DrawTextField(w$(L"fps: ", lastFps), Vector2Int::Zero);
	}
	void DrawTriangle(float deltaTime, float speed, Vector2& pos, Vector2& pos2, Vector2& pos3) 
	{
		const InputManager& input	 = GetInputManager();
		Renderer&			renderer = GetRenderer();
		const float         speedSec = (speed * deltaTime);

		/*********************************************
		 *   삼각형을 구성하는 세 점들의 이동....
		 *******/
		pos += Vector2(
			input.GetAxis(KeyCode::A, KeyCode::D) * speedSec,
			input.GetAxis(KeyCode::S, KeyCode::W) * speedSec
		);

		pos2 += Vector2(
			input.GetAxis(KeyCode::Left, KeyCode::Right) * speedSec,
			input.GetAxis(KeyCode::Down, KeyCode::Up) * speedSec
		);

		pos3 += Vector2(
			input.GetAxis(KeyCode::J, KeyCode::L) * speedSec,
			input.GetAxis(KeyCode::K, KeyCode::I) * speedSec
		);


		/********************************************
		 *   삼각형을 그린다...
		 *******/
		const Vector2 p1 = renderer.WorldToScreen(pos);
		const Vector2 p2 = renderer.WorldToScreen(pos2);
		const Vector2 p3 = renderer.WorldToScreen(pos3);

		renderer.DrawTriangle(LinearColor::Red, p1, p2, p3);
		renderer.DrawTextField(w$(L"p1: ", p1), p1);
		renderer.DrawTextField(w$(L"p2: ", p2), p2);
		renderer.DrawTextField(w$(L"p3: ", p3), p3);
	}
};




/*======================================================================
 *    프로그램의 진입점....
 *===========*/
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE prevHInstance, _In_ LPWSTR commandLine, _In_ int bShowCmd)
{
	MyRenderEngine myEngine;
	myEngine.Run(L"hyunwoo's RenderEngine", hInstance, commandLine, bShowCmd);
	return 0;
}