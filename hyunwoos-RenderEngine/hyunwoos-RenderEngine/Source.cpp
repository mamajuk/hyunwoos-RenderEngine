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

	float     degree = 0.f;
	Vector2   pos    = Vector2::Zero;
	Vector2   pos2   = Vector2::Zero;




	//==========================================================
	////////			 Override methods..				////////
	//==========================================================
protected:
	virtual void OnStart() override final
	{
		Renderer& renderer    = GetRenderer();
		renderer.UseAutoClear = true;
		renderer.ClearColor   = LinearColor::White;
		SetTargetFrameRate(60);
	}


	virtual void OnEnterFrame(float deltaTime) override final
	{
		Renderer& renderer        = GetRenderer();
		const InputManager& input = GetInputManager();


		/*********************************************
		 *    측정한 초당 프레임을 표시한다....
		 *******/
		frameCount++;
		if ((totalTime += deltaTime) >= 1.f) {
			lastFps = frameCount;
			frameCount = 0;
			totalTime = 0.f;
		}

		renderer.DrawTextField(w$(L"fps: ", lastFps), Vector2Int::Zero);
	}


	//=========================================================
	////////			  Private methods..			  /////////
	//=========================================================
private:
	void DrawRectangle(float degree, Vector2 worldPos, const LinearColor& color)
	{
		/***************************************************
		 *    주어진 인자의 위치에 회전된 사각형을 그립니다..
		 ********/
		Renderer& renderer = GetRenderer();

		float c = Math::Cos(Math::Angle2Rad * degree);
		float s = Math::Sin(Math::Angle2Rad * degree);

		Matrix3x3 rotMat = Matrix3x3(Vector3(c, s), Vector3(-s, c), Vector3(worldPos.x, worldPos.y, 1));
		for (float i = 0; i < 100; i+=1.f) 
		{
			for (int j = 0; j < 100; j+=1.f) {
				renderer.SetPixel(
					color, 
					renderer.WorldToScreen(rotMat * Vector3(i, j, 1))
				);
			}
		}

		renderer.DrawTextField(w$(L"rotMat\n", rotMat), renderer.WorldToScreen(Vector2(-300)));
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