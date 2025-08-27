#include <Windows.h>
#include "EngineModule/RenderEngine.h"
#include "UtilityModule/StringLamda.h"
#include "MathModule/Vector.h"
#include "MathModule/Matrix.h"
#include "ImportModule/PngImporter.h"
#include "UtilityModule/Zlib.h"
#include "UtilityModule/BitStream.h"
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

	float     degree          = 0.f;
	Vector2   size			  = (Vector2::One);
	Vector2   pos             = Vector2(100.f ,100.f);
	Vector2   pos2            = Vector2::Zero;
	Vector2   pos3            = Vector2::Zero;

	Texture2D tex;



	//==========================================================
	////////			 Override methods..				////////
	//==========================================================
protected:
	virtual void OnStart() override final
	{
		Renderer& renderer        = GetRenderer();
		renderer.UseAutoClear     = true;
		renderer.WireFrameColor   = LinearColor::White;
		renderer.ClearColor       = LinearColor::Black;
		SetTargetFrameRate(60);

		if (PngImporter::Import(tex, L"test2.png").Success == false) {
			uint32_t sibal = 5;
			sibal++;
		}
	}


	virtual void OnEnterFrame(float deltaTime) override final
	{
		Renderer&			renderer = GetRenderer();
		const InputManager& input    = GetInputManager();

		/*******************************************
		 *   와이어 프레임 모드를 사용하는가?
		 ******/
		if (input.WasPressedThisFrame(KeyCode::Space)) {
			renderer.UseWireFrameMode = !renderer.UseWireFrameMode;
		}

		DrawTexture(tex, 100.f, 100.f, 1.f, deltaTime);
		DrawFps(deltaTime, frameCount, lastFps, totalTime);
	}




	//=========================================================
	////////			  Private methods..			  /////////
	//=========================================================
private:
	void DrawRectangle(float& degree, Vector2& size, Vector2& worldPos, float moveSpeed, float scaleSpeed, float rotSpeed, float deltaTime, const LinearColor& color)
	{
		const InputManager& input         = GetInputManager();
		Renderer&			renderer      = GetRenderer();
		const float         moveSpeedSec  = (moveSpeed * deltaTime);
		const float         rotSpeedSec   = (rotSpeed * deltaTime);
		const float         scaleSpeedSec = (scaleSpeed * deltaTime);

		/************************************************************
		 *   사각형의 회전과 이동, 크기를 조작한다...
		 ********/
		worldPos += Vector2(
			input.GetAxis(KeyCode::Left, KeyCode::Right) * moveSpeedSec,
			input.GetAxis(KeyCode::Down, KeyCode::Up) * moveSpeedSec
		);

		size += Vector2(
			input.GetAxis(KeyCode::J, KeyCode::L) * scaleSpeedSec,
			input.GetAxis(KeyCode::K, KeyCode::I) * scaleSpeedSec
		);

		degree += input.GetAxis(KeyCode::D, KeyCode::A) * rotSpeedSec;


		/***********************************************************
		 *    사각형의 회전과 위치를 구성하는 행렬을 만든다....
		 ********/
		float c = Math::Cos(Math::Angle2Rad * degree);
		float s = Math::Sin(Math::Angle2Rad * degree);

		const Matrix3x3 S = Matrix3x3(
			(Vector3::BasisX * size.x),
			(Vector3::BasisY * size.y),
			Vector3::BasisZ
		);

		const Matrix3x3 TR = Matrix3x3(
			Vector3(c,s,0.f),
			Vector3(-s, c, 0.f),
			Vector3(worldPos.x, worldPos.y, 1.f)
		);

		const Matrix3x3 finalMat = (TR*S);



		/***************************************************
		 *    주어진 위치에 회전된 사각형을 그립니다..
		 ********/
		const float wHalf = 50.f;
		const float hHalf = 50.f;

		const Vector2 p1 = renderer.WorldToScreen((finalMat * Vector3(-wHalf, hHalf, 1.f)));
		const Vector2 p2 = renderer.WorldToScreen((finalMat * Vector3(wHalf, hHalf, 1.f)));
		const Vector3 p3 = renderer.WorldToScreen((finalMat * Vector3(-wHalf, -hHalf, 1.f)));
		const Vector3 p4 = renderer.WorldToScreen((finalMat * Vector3(wHalf, -hHalf, 1.f )));

		renderer.DrawTriangle(color, p1, p2, p3);
		renderer.DrawTriangle(color, p2, p3, p4);
		renderer.DrawTextField(w$(L"rotMat\n", finalMat), Vector2Int(0, 500));
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
	void TestHuffmanTree()
	{
		char     symbol_table[] = { 'A','B','C','D','E','F','G','H' };
		uint32_t code_len_table[] = { 3,3,3,3,3,2,4,4 };

		uint32_t  byteStream = 0b1111011100011101001110010;
		BitStream bitStream((uint8_t*)&byteStream, 4);

		Zlib::HuffmanTree tree;
		tree.Build_Dynamic(code_len_table, sizeof(code_len_table) / 4);

		char symbol1 = symbol_table[tree.GetSymbol(bitStream)];
		char symbol2 = symbol_table[tree.GetSymbol(bitStream)];
		char symbol3 = symbol_table[tree.GetSymbol(bitStream)];
		char symbol4 = symbol_table[tree.GetSymbol(bitStream)];
		char symbol5 = symbol_table[tree.GetSymbol(bitStream)];
		char symbol6 = symbol_table[tree.GetSymbol(bitStream)];
		char symbol7 = symbol_table[tree.GetSymbol(bitStream)];
		char symbol8 = symbol_table[tree.GetSymbol(bitStream)];
	}
	void DrawTexture(const Texture2D texture, float moveSpeed, float rotSpeed, float scaleSpeed, float deltaTime)
	{
		/***********************************************
		 *   텍스쳐의 위치와 회전량을 조작한다...
		 *******/
		Renderer&			renderer      = GetRenderer();
		const InputManager& input         = GetInputManager();
		const float         moveSpeedSec  = (moveSpeed * deltaTime);
		const float			rotSpeedSec   = (rotSpeed * deltaTime);
		const float			scaleSpeedSec = (scaleSpeed * deltaTime);

		static Vector2 worldPos = (Vector2::One * 100.f);
		static Vector2 scale    = Vector2::One;
		static float   degree   = 0.f;

		worldPos += Vector2(
			input.GetAxis(KeyCode::Left, KeyCode::Right) * moveSpeedSec,
			input.GetAxis(KeyCode::Down, KeyCode::Up) * moveSpeedSec
		);

		scale += Vector2(
			input.GetAxis(KeyCode::J, KeyCode::L) * scaleSpeedSec,
			input.GetAxis(KeyCode::K, KeyCode::I) * scaleSpeedSec
		);

		degree += input.GetAxis(KeyCode::S, KeyCode::W) * rotSpeedSec;



		/******************************************************
		 *   각 픽셀들에게 공통적으로 사용할 TRS 행렬을 구축한다..
		 *******/
		const float rad = (degree * Math::Angle2Rad);
		const float c   = Math::Cos(rad);
		const float s   = Math::Sin(rad);

		const Matrix3x3 TR = {
			Vector3(c,s,0.f),
			Vector3(-s,c,0.f),
			Vector3(worldPos.x, worldPos.y, 1.f)
		};

		const Matrix3x3 S = {
			(Vector3::BasisX * scale.x),
			(Vector3::BasisY * scale.y),
			Vector3::BasisZ
		};

		const Matrix3x3 TRS = (TR * S);



		/*****************************************************
		 *   텍스쳐의 각 픽셀들을 그린다...
		 ********/
		const int heightHalf = (texture.Height / 2);
		const int widthHalf  = (texture.Width / 2);

		for (int y = -heightHalf; y < heightHalf; y++)
		{
			for (int x = -widthHalf; x < widthHalf; x++) 
			{
				renderer.SetPixel(
					texture.GetPixel(Vector2Int(x+widthHalf,y+heightHalf)),
					renderer.WorldToScreen(TRS * Vector3(x,y,1.f))
				);
			}
		}

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