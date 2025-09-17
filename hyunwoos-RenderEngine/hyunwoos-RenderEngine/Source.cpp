#include <Windows.h>
#include <vector>
#include "EngineModule/RenderEngine.h"
#include "UtilityModule/StringLamda.h"
#include "MathModule/Vector.h"
#include "MathModule/Quaternion.h"
#include "MathModule/Matrix.h"
#include "ImportModule/PngImporter.h"
#include "ImportModule/PmxImporter.h"
#include "UtilityModule/Zlib.h"
#include "UtilityModule/BitStream.h"
using namespace hyunwoo;
using KeyCode = hyunwoo::InputManager::KeyCode;


/*=========================================================================================================================
 *    사용자 정의 랜더 엔진을 정의한다...
 *==========*/
class MyRenderEngine final : public RenderEngine
{
	//==================================================================================
	////////							   Fields..								////////
	//==================================================================================
private:
	Mesh				   mesh_paymon;
	std::vector<Texture2D> textures_paymon;



	//======================================================================================
	////////						  Override methods..							////////
	//======================================================================================
protected:
	virtual void OnStart() override final
	{
		Renderer& renderer        = GetRenderer();
		renderer.UseAutoClear     = true;	
		renderer.UseAlphaBlending = false;
		renderer.WireFrameColor   = Color::White;
		renderer.ClearColor       = Color::Black;
		SetTargetFrameRate(60);


		/********************************************************
		 *   페이몬 메시를 불러온다...
		 *****/
		PmxImporter::ImportResult pmxRet = PmxImporter::Import(mesh_paymon, 
			L"Resources/Paymon/paymon.pmx"
		);

		PngImporter::ImportResult pngRet = PngImporter::Imports(textures_paymon, { 
			  L"Resources/Paymon/Texture/脸.png",
			  L"Resources/Paymon/Texture/头发.png",
			  L"Resources/Paymon/Texture/衣服.png",
			  L"Resources/Paymon/Texture/mc3.png",
			  L"Resources/Paymon/Texture/披风2.png",
			  L"Resources/Paymon/Texture/表情.png" 
		});

		//정상적으로 읽어들이는데 실패했는가?
		if (pmxRet.Success==false || pngRet.Success==false) {
			throw "import failed!!";
		}

	}


	virtual void OnEnterFrame(float deltaTime) override final
	{
		Renderer&			renderer = GetRenderer();
		const InputManager& input    = GetInputManager();

		//와이어 프레임 모드를 사용하는가?
		if (input.WasPressedThisFrame(KeyCode::Space)) {
			renderer.UseWireFrameMode = !renderer.UseWireFrameMode;
		}

		//알파 블랜딩을 사용하는가?
		if (input.WasPressedThisFrame(KeyCode::Shift)) {
			renderer.UseAlphaBlending = !renderer.UseAlphaBlending;
		}

		Example9_DrawSubMeshs(mesh_paymon, textures_paymon, 100.f, 100.f, 200.f, deltaTime);
		Example1_ShowFps(deltaTime);
	}




	//===================================================================================
	////////							Example methods..						/////////
	//===================================================================================
private:
	void Example1_ShowFps(float deltaTime)
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
	void Example3_TestHuffmanTree()
	{
		char     symbol_table[] = { 'A','B','C','D','E','F','G','H' };
		uint32_t code_len_table[] = { 3,3,3,3,3,2,4,4 };

		uint32_t  byteStream = 0b1111011100011101001110010;
		BitStream bitStream  = BitStream((uint8_t*)&byteStream, 4);

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
	void Example4_DrawTexture(const Texture2D texture, float moveSpeed, float rotSpeed, float scaleSpeed, float deltaTime)
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
	void Example5_DrawRectangle(const Texture2D& tex, float moveSpeed, float scaleSpeed, float rotSpeed, float deltaTime)
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

		renderer.DrawTriangle(tex, p1, uv1, p2, uv2, p3, uv3);
		renderer.DrawTriangle(tex, p2, uv2, p3, uv3, p4, uv4);

		renderer.DrawTextField(w$(L"p1: ", p1, L"\nuv: ", uv1), p1 + Vector2::Left * 200.f);
		renderer.DrawTextField(w$(L"p2: ", p2, L"\nuv: ", uv2), p2);
		renderer.DrawTextField(w$(L"p3: ", p3, L"\nuv: ", uv3), p3 + Vector2::Left * 200.f);
		renderer.DrawTextField(w$(L"p4: ", p4, L"\nuv: ", uv4), p4);
		renderer.DrawTextField(w$(L"rotMat\n", finalMat), Vector2Int(0, 500));
	}
	void Example6_DrawRodrigues(const Vector3& rotAxis, const Vector3& rotVec, float rotSpeed, float deltaTime)
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
	void Example7_DrawRectangle_WithQuaternion(const Texture2D& tex, float moveSpeed, float scaleSpeed, float rotSpeed, float deltaTime)
	{
		const InputManager& input		  = GetInputManager();
		Renderer&			renderer	  = GetRenderer();
		const float         moveSpeedSec  = (moveSpeed * deltaTime);
		const float         rotSpeedSec   = (rotSpeed * deltaTime);
		const float         scaleSpeedSec = (scaleSpeed * deltaTime);

		static Vector2    worldPos = (Vector2::One * 100.f);
		static Vector2    size     = (Vector2::One * 2.f);
		static Vector3    euler    = Vector3::Zero;


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

		euler += Vector3(
			input.GetAxis(KeyCode::S, KeyCode::W)* rotSpeedSec,
			input.GetAxis(KeyCode::A, KeyCode::D)* rotSpeedSec,
			input.GetAxis(KeyCode::Q, KeyCode::E)* rotSpeedSec
		);

		const Quaternion quat = Quaternion::Euler(euler.y, euler.x, euler.z);



		/***********************************************************
		 *    사각형의 회전과 위치를 구성하는 행렬을 만든다....
		 ********/
		const Matrix4x4 T = Matrix4x4(
			Vector4::BasisX,
			Vector4::BasisY,
			Vector4::BasisZ,
			Vector4(worldPos.x, worldPos.y, 0.f, 1.f)
		);

		const Matrix4x4 R = quat.GetRotateMatrix();

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

		const Vector4 objPos_Center		  = (finalMat * Vector4::BasisW);
		const Vector4 objPos_LeftTop      = Vector4(-wHalf, hHalf, 0.f, 1.f);
		const Vector4 objPos_RightTop     = Vector4(wHalf, hHalf, 0.f, 1.f);
		const Vector4 objPos_LeftBottom   = Vector4(-wHalf, -hHalf, 0.f, 1.f);
		const Vector4 objPos_RightBottom  = Vector4(wHalf, -hHalf, 0.f, 1.f);

		const Vector3 worldPos_LeftTop     = (finalMat * objPos_LeftTop);
		const Vector3 worldPos_RightTop    = (finalMat * objPos_RightTop);
		const Vector3 worldPos_LeftBottom  = (finalMat * objPos_LeftBottom);
		const Vector3 worldPos_RightBottom = (finalMat * objPos_RightBottom);

		const Vector2 uvPos_LeftTop     = Vector2(0.f, 0.f);
		const Vector2 uvPos_RightTop    = Vector2(1.f, 0.f);
		const Vector2 uvPos_LeftBottom  = Vector2(0.f, 1.f);
		const Vector2 uvPos_RightBottom = Vector2(1.f, 1.f);

		renderer.DrawTriangle(tex, worldPos_LeftTop, uvPos_LeftTop, worldPos_RightTop, uvPos_RightTop, worldPos_LeftBottom, uvPos_LeftBottom);
		renderer.DrawTriangle(tex, worldPos_RightTop, uvPos_RightTop, worldPos_LeftBottom, uvPos_LeftBottom, worldPos_RightBottom, uvPos_RightBottom);

		renderer.DrawLine(LinearColor::Red,   renderer.WorldToScreen(objPos_Center), renderer.WorldToScreen(objPos_Center + R.BasisX * 50.f));
		renderer.DrawLine(LinearColor::Green, renderer.WorldToScreen(objPos_Center), renderer.WorldToScreen(objPos_Center + R.BasisY * 50.f));
		renderer.DrawLine(LinearColor::Blue,  renderer.WorldToScreen(objPos_Center), renderer.WorldToScreen(objPos_Center + R.BasisZ * 50.f));

		renderer.DrawTextField(w$(L"p1: ", worldPos_LeftTop, L"\nuv: ", uvPos_LeftTop), worldPos_LeftTop + Vector2::Left * 200.f);
		renderer.DrawTextField(w$(L"p2: ", worldPos_RightTop, L"\nuv: ", uvPos_RightTop), worldPos_RightTop);
		renderer.DrawTextField(w$(L"p3: ", worldPos_LeftBottom, L"\nuv: ", uvPos_LeftBottom), worldPos_LeftBottom + Vector2::Left * 200.f);
		renderer.DrawTextField(w$(L"p4: ", worldPos_RightBottom, L"\nuv: ", uvPos_RightBottom), worldPos_RightBottom);
		renderer.DrawTextField(w$(L"Euler: ", euler, L"\n\nT\n", T, L"\n\nR\n", R, L"\n\nS\n", S, L"\n\nfinalMat\n", finalMat), Vector2Int(0, 300));
	}
	void Example8_DrawMesh(const Mesh& mesh, float moveSpeed, float scaleSpeed, float rotSpeed, float deltaTime)
	{
		const InputManager& input		  = GetInputManager();
		Renderer&			renderer	  = GetRenderer();
		const float         moveSpeedSec  = (moveSpeed * deltaTime);
		const float         rotSpeedSec   = (rotSpeed * deltaTime);
		const float         scaleSpeedSec = (scaleSpeed * deltaTime);

		static Vector2    worldPos = (Vector2::One * 100.f);
		static Vector3    size     = (Vector3::One * 40.f);
		static Vector3    euler    = Vector3::Zero;


		/************************************************************
		 *   사각형의 회전과 이동, 크기를 조작한다...
		 ********/
		worldPos += Vector2(
			input.GetAxis(KeyCode::Left, KeyCode::Right) * moveSpeedSec,
			input.GetAxis(KeyCode::Down, KeyCode::Up) * moveSpeedSec
		);

		size += Vector3(
			input.GetAxis(KeyCode::J, KeyCode::L) * scaleSpeedSec,
			input.GetAxis(KeyCode::K, KeyCode::I) * scaleSpeedSec,
			input.GetAxis(KeyCode::U, KeyCode::O) * scaleSpeedSec
		);

		euler += Vector3(
			input.GetAxis(KeyCode::S, KeyCode::W) * rotSpeedSec,
			input.GetAxis(KeyCode::A, KeyCode::D) * rotSpeedSec,
			input.GetAxis(KeyCode::Q, KeyCode::E) * rotSpeedSec
		);

		const Quaternion quat = Quaternion::Euler(euler.y, euler.x, euler.z);



		/***********************************************************
		 *    사각형의 회전과 위치를 구성하는 행렬을 만든다....
		 ********/
		const Matrix4x4 T = Matrix4x4(
			Vector4::BasisX,
			Vector4::BasisY,
			Vector4::BasisZ,
			Vector4(worldPos.x, worldPos.y, 0.f, 1.f)
		);

		const Matrix4x4 R = quat.GetRotateMatrix();

		const Matrix4x4 S = Matrix4x4(
			(Vector4::BasisX * size.x),
			(Vector4::BasisY * size.y),
			(Vector4::BasisZ * size.z),
			Vector4::BasisW
		);


		const Matrix4x4 finalMat = (T * R * S);



		/***************************************************
		 *    주어진 위치에 서브메시별로 삼각형들을 그립니다...
		 ********/
		for (uint32_t i = 0; i < mesh.Triangles.size(); i++) {
			const Triangle triangle = mesh.Triangles[i];
			
			const Vertex& vertex1 = mesh.Vertices[triangle.indices[0]];
			const Vertex& vertex2 = mesh.Vertices[triangle.indices[1]];
			const Vertex& vertex3 = mesh.Vertices[triangle.indices[2]];

			const Vector4 objPos1 = Vector4(vertex1.ObjPos.x, vertex1.ObjPos.y, vertex1.ObjPos.z, 1.f);
			const Vector4 objPos2 = Vector4(vertex2.ObjPos.x, vertex2.ObjPos.y, vertex2.ObjPos.z, 1.f);
			const Vector4 objPos3 = Vector4(vertex3.ObjPos.x, vertex3.ObjPos.y, vertex3.ObjPos.z, 1.f);

			const Vector3 worldPos1 = (finalMat * objPos1);
			const Vector3 worldPos2 = (finalMat * objPos2);
			const Vector3 worldPos3 = (finalMat * objPos3);

			renderer.DrawTriangle(Color::Pink, worldPos1, worldPos2, worldPos3);
		}

		renderer.DrawTextField(w$(L"Euler: ", euler, L"\n\nT\n", T, L"\n\nR\n", R, L"\n\nS\n", S, L"\n\nfinalMat\n", finalMat), Vector2Int(0, 300));
	}
	void Example9_DrawSubMeshs(const Mesh& mesh, const std::vector<Texture2D>& texs, float moveSpeed, float scaleSpeed, float rotSpeed, float deltaTime)
	{
		const InputManager& input		  = GetInputManager();
		Renderer&			renderer	  = GetRenderer();
		const float         moveSpeedSec  = (moveSpeed * deltaTime);
		const float         rotSpeedSec	  = (rotSpeed * deltaTime);
		const float         scaleSpeedSec = (scaleSpeed * deltaTime);

		static Vector3 worldPos = Vector3(0.f, 0.f, 100.f);
		static Vector3 size     = (Vector3::One);
		static Vector3 euler    = Vector3::Zero;


		/************************************************************
		 *   사각형의 회전과 이동, 크기를 조작한다...
		 ********/
		worldPos += Vector3(
			input.GetAxis(KeyCode::Left, KeyCode::Right) * moveSpeedSec,
			input.GetAxis(KeyCode::Down, KeyCode::Up) * moveSpeedSec,
			input.GetAxis(KeyCode::NUMPAD_2, KeyCode::NUMPAD_8) * moveSpeedSec
		);

		size += Vector3(
			input.GetAxis(KeyCode::J, KeyCode::L) * scaleSpeedSec,
			input.GetAxis(KeyCode::K, KeyCode::I) * scaleSpeedSec,
			input.GetAxis(KeyCode::U, KeyCode::O) * scaleSpeedSec
		);

		euler += Vector3(
			input.GetAxis(KeyCode::S, KeyCode::W) * rotSpeedSec,
			input.GetAxis(KeyCode::A, KeyCode::D) * rotSpeedSec,
			input.GetAxis(KeyCode::Q, KeyCode::E) * rotSpeedSec
		);

		const Quaternion quat = Quaternion::Euler(euler.y, euler.x, euler.z);



		/***********************************************************
		 *    사각형의 회전과 위치를 구성하는 행렬을 만든다....
		 ********/
		const float fov = 90.f;
		const float n   = 0.3f;
		const float f   = 1000.f;
		const float k   = (n + f) / (f-n);
		const float l   = f - (k*f);
		const float a   = renderer.GetAspectRatio();
		const float d   = 1.f / Math::Tan(fov * Math::Angle2Rad * .5f);

		const Matrix4x4 T = Matrix4x4(
			Vector4::BasisX,
			Vector4::BasisY,
			Vector4::BasisZ,
			Vector4(worldPos.x, worldPos.y, worldPos.z, 1.f)
		);

		const Matrix4x4 R = quat.GetRotateMatrix();

		const Matrix4x4 S = Matrix4x4(
			(Vector4::BasisX * size.x),
			(Vector4::BasisY * size.y),
			(Vector4::BasisZ * size.z),
			Vector4::BasisW
		);

		const Matrix4x4 P = Matrix4x4(
			Vector4(d, 0.f, 0.f, 0.f),
			Vector4(0.f, (d*a), 0.f, 0.f),
			Vector4(0.f, 0.f, k, 1.f),
			Vector4(0.f, 0.f, l, 0)
		);

		const Matrix4x4 finalMat = (P * T * R * S);



		/***************************************************
		 *    주어진 위치에 서브메시별로 삼각형들을 그립니다...
		 ********/
		const uint32_t subMesh_texIdx_List[] = { 0,1,0,2,2,1,4,5 };

		for (uint32_t subMeshIdx = 0, triangleIdx = 0; subMeshIdx < mesh.SubMesh_Triangle_Counts.size(); subMeshIdx++) {
			const uint32_t subMesh_finalIdx = (triangleIdx + mesh.SubMesh_Triangle_Counts[subMeshIdx]);

			while(triangleIdx < subMesh_finalIdx){

				const Triangle& triangle = mesh.Triangles[triangleIdx++];

				const Vertex& vertex1 = mesh.Vertices[triangle.indices[0]];
				const Vertex& vertex2 = mesh.Vertices[triangle.indices[1]];
				const Vertex& vertex3 = mesh.Vertices[triangle.indices[2]];

				const Vector4 objPos1 = Vector4(vertex1.ObjPos.x, vertex1.ObjPos.y, vertex1.ObjPos.z, 1.f);
				const Vector4 objPos2 = Vector4(vertex2.ObjPos.x, vertex2.ObjPos.y, vertex2.ObjPos.z, 1.f);
				const Vector4 objPos3 = Vector4(vertex3.ObjPos.x, vertex3.ObjPos.y, vertex3.ObjPos.z, 1.f);

				const Vector4 clipPos1 = (finalMat * objPos1);
				const Vector4 clipPos2 = (finalMat * objPos2);
				const Vector4 clipPos3 = (finalMat * objPos3);

				renderer.DrawTriangle(texs[subMesh_texIdx_List[subMeshIdx]], clipPos1, vertex1.UvPos, clipPos2, vertex2.UvPos, clipPos3, vertex3.UvPos);
			}
		}

		renderer.DrawTextField(w$(L"near, far: ", Vector2(n, f), L"\nK, l: ", Vector2(k, l), L"\nEuler: ", euler, L"\n\nT\n", T, L"\n\nR\n", R, L"\n\nS\n", S, L"\n\nClip\n", P, L"\n\nfinalMat\n", finalMat), Vector2Int(0, 20));
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