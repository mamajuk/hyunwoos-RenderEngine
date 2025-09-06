#include <Windows.h>
#include "EngineModule/RenderEngine.h"
#include "UtilityModule/StringLamda.h"
#include "MathModule/Vector.h"
#include "MathModule/Quaternion.h"
#include "MathModule/Matrix.h"
#include "ImportModule/PngImporter.h"
#include "UtilityModule/Zlib.h"
#include "UtilityModule/BitStream.h"
using namespace hyunwoo;
using KeyCode = hyunwoo::InputManager::KeyCode;


/*=======================================================================================================
 *    사용자 정의 랜더 엔진을 정의한다...
 *==========*/
class MyRenderEngine final : public RenderEngine
{
	//=======================================================
	////////			    Fields..				 ////////
	//=======================================================
private:
	Texture2D tex;



	//==========================================================
	////////			 Override methods..				////////
	//==========================================================
protected:
	virtual void OnStart() override final{

		Renderer& renderer        = GetRenderer();
		renderer.UseAutoClear     = true;	
		renderer.WireFrameColor   = Color::White;
		renderer.ClearColor       = Color::Black;
		SetTargetFrameRate(60);

		if (PngImporter::Import(tex, L"test1.png").Success==false) {
			throw "texture load is failed!!";
		}
	}


	virtual void OnEnterFrame(float deltaTime) override final{

		Renderer&			renderer = GetRenderer();
		const InputManager& input    = GetInputManager();

		/*******************************************
		 *   와이어 프레임 모드를 사용하는가?
		 ******/
		if (input.WasPressedThisFrame(KeyCode::Space)) {
			renderer.UseWireFrameMode = !renderer.UseWireFrameMode;
		}

		DrawRectangle2(tex, 100.f, 1.f, 100.f, deltaTime);
		DrawFps(deltaTime);
	}




	//=============================================================
	////////			  Contents methods..			  /////////
	//=============================================================
private:
	void DrawRectangle(const Texture2D& tex, float moveSpeed, float scaleSpeed, float rotSpeed, float deltaTime)
	{
		const InputManager& input         = GetInputManager();
		Renderer&			renderer      = GetRenderer();
		const float         moveSpeedSec  = (moveSpeed  * deltaTime);
		const float         rotSpeedSec   = (rotSpeed   * deltaTime);
		const float         scaleSpeedSec = (scaleSpeed * deltaTime);

		static Vector2 worldPos = (Vector2::One * 100.f);
		static Vector2 size		= (Vector2::One * 2.f);
		static float   degree   = 0.f;


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

		const Vector2 p1  = renderer.WorldToScreen((finalMat * Vector3(-wHalf, hHalf, 1.f)));
		const Vector2 p2  = renderer.WorldToScreen((finalMat * Vector3(wHalf, hHalf, 1.f)));
		const Vector3 p3  = renderer.WorldToScreen((finalMat * Vector3(-wHalf, -hHalf, 1.f)));
		const Vector3 p4  = renderer.WorldToScreen((finalMat * Vector3(wHalf, -hHalf, 1.f )));

		const Vector2 uv1 = Vector2(0.f, 0.f);
		const Vector2 uv2 = Vector2(1.f, 0.f);
		const Vector2 uv3 = Vector2(0.f, 1.f);
		const Vector2 uv4 = Vector2(1.f, 1.f);

		renderer.DrawTriangleWithTexture(tex, p1, uv1, p2, uv2, p3, uv3);
		renderer.DrawTriangleWithTexture(tex, p2, uv2, p3, uv3, p4, uv4);

		renderer.DrawTextField(w$(L"p1: ", p1, L"\nuv: ", uv1), p1 + Vector2::Left * 200.f);
		renderer.DrawTextField(w$(L"p2: ", p2, L"\nuv: ", uv2), p2);
		renderer.DrawTextField(w$(L"p3: ", p3, L"\nuv: ", uv3), p3 + Vector2::Left * 200.f);
		renderer.DrawTextField(w$(L"p4: ", p4, L"\nuv: ", uv4), p4);
		renderer.DrawTextField(w$(L"rotMat\n", finalMat), Vector2Int(0, 500));
	}
	void DrawFps(float deltaTime) 
	{
		/*********************************************
		 *    측정한 초당 프레임을 표시한다....
		 *******/
		Renderer& renderer = GetRenderer();

		static int   frameCount = 0;
		static int   lastFps    = 0;
		static float totalTime  = 0.f;

		frameCount++;
		if ((totalTime += deltaTime) >= 1.f) {
			lastFps = frameCount;
			frameCount = 0;
			totalTime = 0.f;
		}

		renderer.DrawTextField(w$(L"fps: ", lastFps), Vector2Int::Zero);
	}
	void DrawTriangle(float deltaTime, float speed) 
	{
		const InputManager& input	 = GetInputManager();
		Renderer&			renderer = GetRenderer();
		const float         speedSec = (speed * deltaTime);

		static Vector2 pos  = Vector2(100.f, 100.f);
		static Vector2 pos2 = Vector2(200.f, 100.f);
		static Vector2 pos3 = Vector2(100.f, 200.f);



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
	void DrawRodrigues(const Vector3& rotAxis, const Vector3& rotVec, float rotSpeed, float deltaTime)
	{
		const InputManager& input	    = GetInputManager();
		Renderer&			renderer    = GetRenderer();
		const float			rotSpeedSec = (rotSpeed * deltaTime);

		static float angle = 0.f;

		/*********************************************************
		 *  각도를 조작한다....
		 ********/
		angle += input.GetAxis(KeyCode::Left, KeyCode::Right) * rotSpeedSec;


		/**********************************************************
		 *   로드리게스 회전을 적용한다. 왼손 좌표계이기 때문에 외적의
		 *   순서는 x기저 -> z기저 순으로 해야한다....
		 *********/
		float rad = (Math::Angle2Rad * angle);
		float c   = Math::Cos(rad);
		float s   = Math::Sin(rad);

		Vector3 axis   = rotAxis.GetNormalized();
		Vector3 proj   = (axis * Vector3::Dot(axis, rotVec));
		Vector3 xBasis = (rotVec - proj).GetNormalized();
		Vector3 zBasis = Vector3::Cross(xBasis, axis).GetNormalized();
		Vector3 final  = Vector3::Rodrigues(angle, axis, rotVec);

		renderer.DrawLine(Color::White, renderer.WorldToScreen(Vector2::Zero), renderer.WorldToScreen(rotVec * 100.f));
		renderer.DrawLine(Color::Red, renderer.WorldToScreen(Vector2::Zero), renderer.WorldToScreen(axis * 100.f));
		renderer.DrawLine(Color::Pink, renderer.WorldToScreen(Vector2::Zero), renderer.WorldToScreen(final * 100.f));
		renderer.DrawTextField(w$(L"<-, ->: angle - ~ + \nangle: ", angle, L"\nrotVec: ", rotVec, L"\nrotAxis: ", rotAxis), renderer.WorldToScreen(Vector2::Down * 50.f));
	}
	void DrawRectangle2(const Texture2D& tex, float moveSpeed, float scaleSpeed, float rotSpeed, float deltaTime)
	{
		const InputManager& input = GetInputManager();
		Renderer& renderer = GetRenderer();
		const float         moveSpeedSec = (moveSpeed * deltaTime);
		const float         rotSpeedSec = (rotSpeed * deltaTime);
		const float         scaleSpeedSec = (scaleSpeed * deltaTime);

		static Vector2    worldPos = (Vector2::One * 100.f);
		static Vector2    size     = (Vector2::One * 2.f);
		static Quaternion quat     = Quaternion::Identity;


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

		quat *= Quaternion::AngleAxis(
			input.GetAxis(KeyCode::A, KeyCode::D) * rotSpeedSec,
			Vector3::Up
		);

		quat *= Quaternion::AngleAxis(
			input.GetAxis(KeyCode::S, KeyCode::W) * rotSpeedSec,
			Vector3::Right
		);



		/***********************************************************
		 *    사각형의 회전과 위치를 구성하는 행렬을 만든다....
		 ********/
		const Matrix4x4 T = Matrix4x4(
			Vector4::BasisX,
			Vector4::BasisY,
			Vector4::BasisZ,
			Vector4(worldPos.x, worldPos.y, 0.f, 1.f)
		);

		const Matrix4x4 R = quat;

		const Matrix4x4 S = Matrix4x4(
			(Vector4::BasisX * size.x),
			(Vector4::BasisY * size.y),
			Vector4::BasisZ,
			Vector4::BasisW
		);


		const Matrix4x4 finalMat = (T * R * S);



		/***************************************************
		 *    주어진 위치에 회전된 사각형을 그립니다..
		 ********/
		const float wHalf = 50.f;
		const float hHalf = 50.f;

		const Vector4 objPos1 = Vector4(-wHalf, hHalf, 0.f, 1.f);
		const Vector4 objPos2 = Vector4(wHalf, hHalf, 0.f, 1.f);
		const Vector4 objPos3 = Vector4(-wHalf, -hHalf, 0.f, 1.f);
		const Vector4 objPos4 = Vector4(wHalf, -hHalf, 0.f, 1.f);

		const Vector2 p1 = renderer.WorldToScreen((finalMat * objPos1));
		const Vector2 p2 = renderer.WorldToScreen((finalMat * objPos2));
		const Vector3 p3 = renderer.WorldToScreen((finalMat * objPos3));
		const Vector3 p4 = renderer.WorldToScreen((finalMat * objPos4));

		const Vector2 uv1 = Vector2(0.f, 0.f);
		const Vector2 uv2 = Vector2(1.f, 0.f);
		const Vector2 uv3 = Vector2(0.f, 1.f);
		const Vector2 uv4 = Vector2(1.f, 1.f);

		renderer.DrawTriangleWithTexture(tex, p1, uv1, p2, uv2, p3, uv3);
		renderer.DrawTriangleWithTexture(tex, p2, uv2, p3, uv3, p4, uv4);

		renderer.DrawTextField(w$(L"p1: ", p1, L"\nuv: ", uv1), p1 + Vector2::Left * 200.f);
		renderer.DrawTextField(w$(L"p2: ", p2, L"\nuv: ", uv2), p2);
		renderer.DrawTextField(w$(L"p3: ", p3, L"\nuv: ", uv3), p3 + Vector2::Left * 200.f);
		renderer.DrawTextField(w$(L"p4: ", p4, L"\nuv: ", uv4), p4);
		renderer.DrawTextField(w$(L"T\n", T, L"\n\nR\n", R, L"\n\nS\n", S, L"\n\nfinalMat\n", finalMat), Vector2Int(0, 300));
	}
};











/*========================================================================================================
 *    프로그램의 진입점....
 *===========*/
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE prevHInstance, _In_ LPWSTR commandLine, _In_ int bShowCmd)
{
	MyRenderEngine myEngine;
	myEngine.Run(L"hyunwoo's RenderEngine", hInstance, commandLine, bShowCmd);
	return 0;
}