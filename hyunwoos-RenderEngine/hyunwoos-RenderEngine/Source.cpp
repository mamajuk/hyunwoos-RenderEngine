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
#include "GeometryModule/Geometry.h"
#include "EngineModule/Transform.h"
#include "RenderModule/Mesh.h"
#include "EngineModule/UniqueableObject.h"
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
	Mesh				   m_mesh;
	Mesh				   m_mesh_boundingSphere;
	Mesh				   m_mesh_boundingBox;
	bool				   m_useBoundingSphere;
	std::vector<Texture2D> m_textures;
	std::vector<Material>  m_materials;





	//======================================================================================
	////////						  Override methods..							////////
	//======================================================================================
protected:
	virtual void OnStart() override final
	{
		Renderer& renderer        = GetRenderer();
		renderer.UseAutoClear     = true;	
		renderer.UseAlphaBlending = false;
		renderer.WireFrameColor   = Color::Black;
		renderer.ClearColor       = Color::White;
		SetTargetFrameRate(60);

		//GetViewPort().RenderTarget.Init(GetClientHwnd(), Vector2Int(800, 720));

		return;
		/********************************************************
		 *   메시를 불러온다...
		 *****/
		PmxImporter::StorageDescription pmx_storage_desc;
		pmx_storage_desc.OutMesh	  = &m_mesh;
		pmx_storage_desc.OutTextures  = &m_textures;
		pmx_storage_desc.OutMaterials = &m_materials;

		PmxImporter::ImportResult pmxRet;
		if ((pmxRet=PmxImporter::Import(pmx_storage_desc, L"Resources/steve/steve.pmx")).Success==false) {
			throw "Pmx import failed!!";
		}


		/*********************************************************
		 *   바운딩 스피어 메시를 구축한다...
		 *****/
		m_mesh.RecalculateBoundingSphere();
		m_mesh.CreateBoundingSphereMesh(m_mesh_boundingSphere);

		m_mesh.RecalculateBoundingBox();
		m_mesh.CreateBoundingBoxMesh(m_mesh_boundingBox);
	}


	virtual void OnEnterFrame(float deltaTime) override final
	{
		Renderer&			renderer = GetRenderer();
		const InputManager& input    = GetInputManager();

		//와이어 프레임 모드를 사용하는가?
		if (input.WasPressedThisFrame(KeyCode::Num_1)) {
			renderer.UseWireFrameMode = !renderer.UseWireFrameMode;
		}

		//알파 블랜딩을 사용하는가?
		if (input.WasPressedThisFrame(KeyCode::Num_2)) {
			renderer.UseAlphaBlending = !renderer.UseAlphaBlending;
		}

		//백페이스 컬링을 사용하는가?
		if (input.WasPressedThisFrame(KeyCode::Num_3)) {
			renderer.UseBackfaceCulling = !renderer.UseBackfaceCulling;
		}

		//사용할 바운딩볼륨의 종류를 전환한다..
		if (input.WasPressedThisFrame(KeyCode::Num_4)) {
			m_useBoundingSphere = !m_useBoundingSphere;
		}

		//삼각형들의 노멀을 표시하는가?
		if (input.WasPressedThisFrame(KeyCode::Num_5)) {
			renderer.DrawTriangleNormal = !renderer.DrawTriangleNormal;
		}

		Example15_DrawRenderMesh(deltaTime);
		Example1_ShowInfo(deltaTime);
	}




	//===================================================================================
	////////							Example methods..						/////////
	//===================================================================================
private:
	#pragma region
	void Example1_ShowInfo(float deltaTime)
	{
		#pragma region
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

		renderer.DrawTextField(w$(
			L"fps: ", lastFps, 
			L"\nUseWireFrameMode(1): ", (int)renderer.UseWireFrameMode,
			L"\nUseAlphaBlending(2): ", (int)renderer.UseAlphaBlending,
			L"\nUseBackfaceCulling(3): ", (int)renderer.UseBackfaceCulling,
			L"\nSwitchBoundsType(4): ", (m_useBoundingSphere? L"BoundingSphere":L"BoundingBox")),
			Vector2Int::Zero,
			GetViewPort()
		);
		#pragma endregion
	}

	void DrawTriangle(float deltaTime, float speed) 
	{
		#pragma region
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
			Renderer::TriangleDescription triangle_desc;
			triangle_desc.FillUpColor		 = Color::Red;
			triangle_desc.ScreenPositions[0] = renderer.WorldToScreen(pos, GetViewPort());
			triangle_desc.ScreenPositions[1] = renderer.WorldToScreen(pos, GetViewPort());
			triangle_desc.ScreenPositions[2] = renderer.WorldToScreen(pos, GetViewPort());

			renderer.DrawTriangle(triangle_desc, GetViewPort());
			renderer.DrawTextField(w$(L"p1: ", triangle_desc.ScreenPositions[0]), triangle_desc.ScreenPositions[0], GetViewPort());
			renderer.DrawTextField(w$(L"p2: ", triangle_desc.ScreenPositions[1]), triangle_desc.ScreenPositions[1], GetViewPort());
			renderer.DrawTextField(w$(L"p3: ", triangle_desc.ScreenPositions[2]), triangle_desc.ScreenPositions[2], GetViewPort());
		#pragma endregion
	}

	void Example3_TestHuffmanTree()
	{
		#pragma region
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
		#pragma endregion
	}

	void Example4_DrawTexture(const Texture2D texture, float moveSpeed, float rotSpeed, float scaleSpeed, float deltaTime)
	{
		#pragma region
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
					renderer.WorldToScreen(TRS * Vector3(x,y,1.f), GetViewPort()),
					GetViewPort()
				);
			}
		}
		#pragma endregion
	}

	void Example5_DrawRectangle(const Texture2D& tex, float deltaTime)
	{
		#pragma region
		const InputManager& input         = GetInputManager();
		Renderer&			renderer      = GetRenderer();
		const float         moveSpeedSec  = (100.f * deltaTime);
		const float         rotSpeedSec   = (100.f * deltaTime);
		const float         scaleSpeedSec = (1.f   * deltaTime);

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

		const Vector2 p1 = renderer.WorldToScreen((finalMat * Vector3(-wHalf, hHalf, 1.f)), GetViewPort());
		const Vector2 p2 = renderer.WorldToScreen((finalMat * Vector3(wHalf, hHalf, 1.f)), GetViewPort());
		const Vector3 p3 = renderer.WorldToScreen((finalMat * Vector3(-wHalf, -hHalf, 1.f)), GetViewPort());
		const Vector3 p4 = renderer.WorldToScreen((finalMat * Vector3(wHalf, -hHalf, 1.f)), GetViewPort());

		const Vector2 uv1 = Vector2(0.f, 0.f);
		const Vector2 uv2 = Vector2(1.f, 0.f);
		const Vector2 uv3 = Vector2(0.f, 1.f);
		const Vector2 uv4 = Vector2(1.f, 1.f);

		Renderer::TriangleDescription triangle_desc;
		triangle_desc.MappedTexture      = &tex;
		triangle_desc.ScreenPositions[0] = p1;
		triangle_desc.ScreenPositions[1] = p2;
		triangle_desc.ScreenPositions[2] = p3;
		triangle_desc.Uvs[0]			 = uv1;
		triangle_desc.Uvs[1]			 = uv2;
		triangle_desc.Uvs[2]			 = uv3;

		renderer.DrawTriangle(triangle_desc, GetViewPort());

		triangle_desc.ScreenPositions[0] = p2;
		triangle_desc.ScreenPositions[1] = p3;
		triangle_desc.ScreenPositions[2] = p4;
		triangle_desc.Uvs[0]		     = uv2;
		triangle_desc.Uvs[1]			 = uv3;
		triangle_desc.Uvs[2]			 = uv4;
		renderer.DrawTriangle(triangle_desc, GetViewPort());

		renderer.DrawTextField(w$(L"p1: ", p1, L"\nuv: ", uv1), p1 + Vector2::Left * 200.f, GetViewPort());
		renderer.DrawTextField(w$(L"p2: ", p2, L"\nuv: ", uv2), p2, GetViewPort());
		renderer.DrawTextField(w$(L"p3: ", p3, L"\nuv: ", uv3), p3 + Vector2::Left * 200.f, GetViewPort());
		renderer.DrawTextField(w$(L"p4: ", p4, L"\nuv: ", uv4), p4, GetViewPort());
		renderer.DrawTextField(w$(L"rotMat\n", finalMat), Vector2Int(0, 500), GetViewPort());
		#pragma endregion
	}

	void Example6_DrawRodrigues(const Vector3& rotAxis, const Vector3& rotVec, float rotSpeed, float deltaTime)
	{
		#pragma region
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

		renderer.DrawLine(Color::White, renderer.WorldToScreen(Vector2::Zero, GetViewPort()), renderer.WorldToScreen(rotVec * 100.f, GetViewPort()), GetViewPort());
		renderer.DrawLine(Color::Red, renderer.WorldToScreen(Vector2::Zero, GetViewPort()), renderer.WorldToScreen(axis * 100.f, GetViewPort()), GetViewPort());
		renderer.DrawLine(Color::Pink, renderer.WorldToScreen(Vector2::Zero, GetViewPort()), renderer.WorldToScreen(final * 100.f, GetViewPort()), GetViewPort());
		renderer.DrawTextField(w$(L"<-, ->: angle - ~ + \nangle: ", angle, L"\nrotVec: ", rotVec, L"\nrotAxis: ", rotAxis), renderer.WorldToScreen(Vector2::Down * 50.f, GetViewPort()), GetViewPort());
		#pragma endregion
	}

	void Example7_DrawRectangle_WithQuaternion(const Texture2D& tex, float moveSpeed, float scaleSpeed, float rotSpeed, float deltaTime)
	{
		#pragma region
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
			Vector4(worldPos, 0.f, 1.f)
		);

		const Matrix4x4 R = quat.GetRotateMatrix();

		const Matrix4x4 S = Matrix4x4(
			(Vector4::BasisX * size.x),
			(Vector4::BasisY * size.y),
			Vector4::BasisZ,
			Vector4::BasisW
		);


		const Matrix4x4 finalMat = (T * R * S);



		/***********************************************************
		 *    주어진 위치에 회전된 사각형을 그립니다..
		 ********/
		const float wHalf = 50.f;
		const float hHalf = 50.f;

		const Vector4 objPos_Center		  = (finalMat * Vector4::BasisW);
		const Vector4 objPos_LeftTop      = Vector4(-wHalf, hHalf, 0.f, 1.f);
		const Vector4 objPos_RightTop     = Vector4(wHalf, hHalf, 0.f, 1.f);
		const Vector4 objPos_LeftBottom   = Vector4(-wHalf, -hHalf, 0.f, 1.f);
		const Vector4 objPos_RightBottom  = Vector4(wHalf, -hHalf, 0.f, 1.f);

		const Vector3 screenPos_LeftTop     = renderer.WorldToScreen(finalMat * objPos_LeftTop, GetViewPort());
		const Vector3 screenPos_RightTop    = renderer.WorldToScreen(finalMat * objPos_RightTop, GetViewPort());
		const Vector3 screenPos_LeftBottom  = renderer.WorldToScreen(finalMat * objPos_LeftBottom, GetViewPort());
		const Vector3 screenPos_RightBottom = renderer.WorldToScreen(finalMat * objPos_RightBottom, GetViewPort());

		const Vector2 uvPos_LeftTop     = Vector2(0.f, 0.f);
		const Vector2 uvPos_RightTop    = Vector2(1.f, 0.f);
		const Vector2 uvPos_LeftBottom  = Vector2(0.f, 1.f);
		const Vector2 uvPos_RightBottom = Vector2(1.f, 1.f);


		/*--------------------------------------------
		 *   첫번째 삼각형을 그린다....
		 *-----*/
		Renderer::TriangleDescription triangle_desc;
		triangle_desc.MappedTexture      = &tex;

		triangle_desc.ScreenPositions[0] = screenPos_LeftTop;
		triangle_desc.Uvs[0]			 = uvPos_LeftTop;

		triangle_desc.ScreenPositions[1] = screenPos_RightTop;
		triangle_desc.Uvs[1]			 = uvPos_RightTop;

		triangle_desc.ScreenPositions[2] = screenPos_LeftBottom;
		triangle_desc.Uvs[2]			 = uvPos_LeftBottom;

		renderer.DrawTriangle(triangle_desc, GetViewPort());



		/*--------------------------------------------
		 *   두번째 삼각형을 그린다....
		 *-----*/
		triangle_desc.ScreenPositions[0] = screenPos_RightTop;
		triangle_desc.Uvs[0]			 = uvPos_RightTop;

		triangle_desc.ScreenPositions[1] = screenPos_LeftBottom;
		triangle_desc.Uvs[1]			 = uvPos_LeftBottom;

		triangle_desc.ScreenPositions[2] = screenPos_RightBottom;
		triangle_desc.Uvs[2]			 = uvPos_RightBottom;

		renderer.DrawTriangle(triangle_desc, GetViewPort());


		/*--------------------------------------------
		 *   디버그 출력...
		 *-----*/
		renderer.DrawLine(LinearColor::Red,   renderer.WorldToScreen(objPos_Center, GetViewPort()), renderer.WorldToScreen(objPos_Center + R.BasisX * 50.f, GetViewPort()), GetViewPort());
		renderer.DrawLine(LinearColor::Green, renderer.WorldToScreen(objPos_Center, GetViewPort()), renderer.WorldToScreen(objPos_Center + R.BasisY * 50.f, GetViewPort()), GetViewPort());
		renderer.DrawLine(LinearColor::Blue,  renderer.WorldToScreen(objPos_Center, GetViewPort()), renderer.WorldToScreen(objPos_Center + R.BasisZ * 50.f, GetViewPort()), GetViewPort());

		renderer.DrawTextField(w$(L"p1: ", screenPos_LeftTop, L"\nuv: ", uvPos_LeftTop), screenPos_LeftTop + Vector2::Left * 200.f, GetViewPort());
		renderer.DrawTextField(w$(L"p2: ", screenPos_RightTop, L"\nuv: ", uvPos_RightTop), screenPos_RightTop, GetViewPort());
		renderer.DrawTextField(w$(L"p3: ", screenPos_LeftBottom, L"\nuv: ", uvPos_LeftBottom), screenPos_LeftBottom + Vector2::Left * 200.f, GetViewPort());
		renderer.DrawTextField(w$(L"p4: ", screenPos_RightBottom, L"\nuv: ", uvPos_RightBottom), screenPos_RightBottom, GetViewPort());
		renderer.DrawTextField(w$(L"Euler: ", euler, L"\n\nT\n", T, L"\n\nR\n", R, L"\n\nS\n", S, L"\n\nfinalMat\n", finalMat), Vector2Int(0, 300), GetViewPort());
		#pragma endregion
	}

	void Example8_DrawMesh(const Mesh& mesh, float moveSpeed, float scaleSpeed, float rotSpeed, float deltaTime)
	{
		#pragma region
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
			Vector4(worldPos, 0.f, 1.f)
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
		Renderer::TriangleDescription triangle_desc;

		for (uint32_t i = 0; i < mesh.Triangles.size(); i++) {
			const IndexedTriangle triangle = mesh.Triangles[i];
			
			const Vertex& vertex1 = mesh.Vertices[triangle.Indices[0]];
			const Vertex& vertex2 = mesh.Vertices[triangle.Indices[1]];
			const Vertex& vertex3 = mesh.Vertices[triangle.Indices[2]];

			const Vector4 objPos1 = Vector4(vertex1.ObjPos.x, vertex1.ObjPos.y, vertex1.ObjPos.z, 1.f);
			const Vector4 objPos2 = Vector4(vertex2.ObjPos.x, vertex2.ObjPos.y, vertex2.ObjPos.z, 1.f);
			const Vector4 objPos3 = Vector4(vertex3.ObjPos.x, vertex3.ObjPos.y, vertex3.ObjPos.z, 1.f);

			const Vector3 worldPos1 = (finalMat * objPos1);
			const Vector3 worldPos2 = (finalMat * objPos2);
			const Vector3 worldPos3 = (finalMat * objPos3);

			triangle_desc.ScreenPositions[0] = renderer.WorldToScreen(worldPos1, GetViewPort());
			triangle_desc.ScreenPositions[1] = renderer.WorldToScreen(worldPos2, GetViewPort());
			triangle_desc.ScreenPositions[2] = renderer.WorldToScreen(worldPos3, GetViewPort());

			renderer.DrawTriangle(triangle_desc, GetViewPort());
		}

		renderer.DrawTextField(w$(L"Euler: ", euler, L"\n\nT\n", T, L"\n\nR\n", R, L"\n\nS\n", S, L"\n\nfinalMat\n", finalMat), Vector2Int(0, 300), GetViewPort());
		#pragma endregion
	}

	void Example9_DrawSubMeshs(const Mesh& mesh, const std::vector<Texture2D>& texs, float deltaTime)
	{
		#pragma region
		const InputManager& input		  = GetInputManager();
		Renderer&			renderer	  = GetRenderer();
		const float         speedScale	  = (input.IsInProgress(KeyCode::Space)? .2f: 1.f);
		const float         moveSpeedSec  = (100.f * speedScale * deltaTime);
		const float         rotSpeedSec	  = (200.f * speedScale * deltaTime);
		const float         scaleSpeedSec = (1.f * speedScale * deltaTime);

		static WeakPtr<Transform> mesh_tr;
		static int32_t			  selected_boneIdx  = -1;

		static std::vector<Matrix4x4>		   skinning_mats;
		static std::vector<Vector4>			   blending_vertices;
		static std::vector<WeakPtr<Transform>> bone_trs;


		/************************************************************
		 *   본 트랜스폼들을 초기화한다....
		 ********/
		if (skinning_mats.size() != mesh.Bones.size()) 
		{
			const std::vector<Bone>& bone_list = mesh.Bones;

			skinning_mats.resize(bone_list.size());
			bone_trs.resize(bone_list.size());
			blending_vertices.resize(mesh.Vertices.size());

			mesh_tr = Transform::CreateTransform();

			for (uint32_t i = 0; i < bone_trs.size(); i++) {
				bone_trs[i] = Transform::CreateTransform();
			}

			uint64_t update_id = UniqueableObject::GetDirtyUpdateID();
			Transform* mesh_tr_rawPtr = mesh_tr.Get();

			//본들의 월드를 초기화한다...
			for (uint32_t i = 0; i < bone_trs.size(); i++) {
				Transform& bone_tr = *bone_trs[i].Get();

				bone_tr.SetLocalPositionAndScaleAndRotation(
					bone_list[i].BindingPose.Position,
					bone_list[i].BindingPose.Scale,
					bone_list[i].BindingPose.Rotation
				);

				//부모가 존재하는 경우에만...
				if (bone_list[i].Parent_BoneIdx >= 0) {
					bone_tr.SetParent(bone_trs[bone_list[i].Parent_BoneIdx].Get());
				}

				//부모가 없는 루트 트랜스폼인가?
				else {
					mesh_tr_rawPtr->AddChild(&bone_tr);
				}
			}

			mesh_tr_rawPtr->SetWorldPosition(Vector3(0.f, 0.f, 50.f));
			mesh_tr_rawPtr->SetWorldScale(Vector3::One * 1.f);
		}


		/************************************************************
		 *   조작할 트랜스폼의 회전과 이동, 크기를 조작한다...
		 ********/
		Transform& mesh_tr_ref = *mesh_tr.Get();
		Transform& control_tr  = *(selected_boneIdx>=0? bone_trs[selected_boneIdx]:mesh_tr).Get();

		control_tr.SetLocalPosition(control_tr.GetLocalPosition() +
			Vector3(
				input.GetAxis(KeyCode::Left, KeyCode::Right) * moveSpeedSec,
				input.GetAxis(KeyCode::Down, KeyCode::Up) * moveSpeedSec,
				input.GetAxis(KeyCode::NUMPAD_2, KeyCode::NUMPAD_8) * moveSpeedSec
		));

		const Quaternion eular = Quaternion::Euler(
			input.GetAxis(KeyCode::A, KeyCode::D) * rotSpeedSec,
			input.GetAxis(KeyCode::S, KeyCode::W) * rotSpeedSec,
			input.GetAxis(KeyCode::Q, KeyCode::E) * rotSpeedSec
		);

		control_tr.SetLocalRotation(eular * control_tr.GetLocalRotation());

		control_tr.SetLocalScale(control_tr.GetLocalScale() +
			Vector3(
				input.GetAxis(KeyCode::J, KeyCode::L) * scaleSpeedSec,
				input.GetAxis(KeyCode::K, KeyCode::I) * scaleSpeedSec,
				input.GetAxis(KeyCode::U, KeyCode::O) * scaleSpeedSec
			));

		if (input.WasPressedThisFrame(KeyCode::Right_Mouse) || input.WasPressedThisFrame(KeyCode::Shift)) {
			selected_boneIdx = -1;
		}

		if (input.WasPressedThisFrame(KeyCode::P)) {
			Transform::DestroyTransform(&control_tr);
			selected_boneIdx = -1;
		}

		if (input.WasPressedThisFrame(KeyCode::M)) {
			control_tr.SetParent(Transform::GetRoot());
		}


		/***********************************************************
		 *    메시의 회전과 위치를 구성하는 행렬을 만든다....
		 ********/
		const float fov = 100.f;
		const float n   = 10.f;
		const float f   = 100.f;
		const float k   = (-n-f) / (n-f);
		const float l   = -n - (k*n);
		const float a   = GetViewPort().RenderTarget.GetAspectRatio();
		const float d   = 1.f / Math::Tan(fov * Math::Angle2Rad * .5f);

		const Matrix4x4 TRS = mesh_tr_ref.GetTRS();

		const Matrix4x4 P = Matrix4x4(
			Vector4(d,   0.f,   0.f, 0.f),
			Vector4(0.f, (d*a), 0.f, 0.f),
			Vector4(0.f, 0.f,   k,   1.f),
			Vector4(0.f, 0.f,   l,   0.f)
		);

		const Matrix4x4 PTRS = (P*TRS);


		/********************************************************************
		 *   절두체 컬링을 진행한다....
		 *******/

		/*----------------------------------------------------
		 *   메시의 바운딩스피어가 절두체를 벗어났는지를 판별하고,
		 *   맞다면 삼각형 그리기를 넘어간다....
		 *-----*/
		Frustum    frustum			 = Frustum(PTRS);
		const bool useFrustumCulling = (m_useBoundingSphere? frustum.IsOverlapped(mesh.BoundSphere):frustum.IsOverlapped(mesh.BoundBox))==false;

		renderer.DrawTextField(w$(
			L"useFrustumCulling: ", (int)useFrustumCulling,
			L"\n\nBoundingSphere",
			L"\ncenter: ", mesh.BoundSphere.Center,
			L"\nradius: ", mesh.BoundSphere.Radius,
			L"\n\nBoundingBox",
			L"\nMin: ", mesh.BoundBox.Min,
			L"\nMax: ", mesh.BoundBox.Max,
			L"\nTRS: \n", TRS),
			Vector2Int(0, 100),
			GetViewPort()
		);


		if (useFrustumCulling) {
			return;
		}


		
		/*---------------------------------------------------
		 *  메시의 바운딩 스피어를 랜더링한다....
		 *-----*/
		Renderer::ClipTriangleList    clip_triangle_list;
		Renderer::TriangleDescription bounds_triangle_desc;
		bounds_triangle_desc.FillUpColor = Color::Red;

		const Mesh& drawBoundMesh = (m_useBoundingSphere? m_mesh_boundingSphere:m_mesh_boundingBox);

		static std::wstring test_str;
		static Vector3      last_pos;
		static Vector3      plane_dir;

		renderer.DrawLine(Color::Red,last_pos,last_pos + plane_dir * 50.f, GetViewPort());

		for (uint32_t i = 0; i < drawBoundMesh.Triangles.size(); i++) {
			const IndexedTriangle& triangle = drawBoundMesh.Triangles[i];
			const Vertex&   vertex1  = drawBoundMesh.Vertices[triangle.Indices[0]];
			const Vertex&   vertex2  = drawBoundMesh.Vertices[triangle.Indices[1]];
			const Vertex&   vertex3  = drawBoundMesh.Vertices[triangle.Indices[2]];

			const Vector4 clipPos1 = (PTRS * Vector4(vertex1.ObjPos, 1.f));
			const Vector4 clipPos2 = (PTRS * Vector4(vertex2.ObjPos, 1.f));
			const Vector4 clipPos3 = (PTRS * Vector4(vertex3.ObjPos, 1.f));

			const Vector2 screenPos1 = renderer.NDCToScreen(renderer.ClipToNDC(clipPos1), GetViewPort());
			const Vector2 screenPos2 = renderer.NDCToScreen(renderer.ClipToNDC(clipPos2), GetViewPort());
			const Vector2 screenPos3 = renderer.NDCToScreen(renderer.ClipToNDC(clipPos3), GetViewPort());

			clip_triangle_list.triangleCount = 1;
			clip_triangle_list.Triangles[0]  = Renderer::ClipTriangle(
				Renderer::ClipVertex(clipPos1, vertex1.UvPos),
				Renderer::ClipVertex(clipPos2, vertex2.UvPos),
				Renderer::ClipVertex(clipPos3, vertex3.UvPos)
			);

			//+Z, -Z 평면에 대한 클립핑을 적용한다...
			renderer.ClippingTriangle(clip_triangle_list, renderer.ClippingTest_Far, renderer.SolveT_Far);
			renderer.ClippingTriangle(clip_triangle_list, renderer.ClippingTest_Near, renderer.SolveT_Near);

			//+X, -X 평면에 대한 클립핑을 적용한다...
			renderer.ClippingTriangle(clip_triangle_list, renderer.ClippingTest_Right, renderer.SolveT_Right);
			renderer.ClippingTriangle(clip_triangle_list, renderer.ClippingTest_Left, renderer.SolveT_Left);

			//+Y, -Y 평면에 대한 클립핑을 적용한다...
			renderer.ClippingTriangle(clip_triangle_list, renderer.ClippingTest_Up, renderer.SolveT_Up);
			renderer.ClippingTriangle(clip_triangle_list, renderer.ClippingTest_Down, renderer.SolveT_Down);

			for (uint32_t i = 0; i < clip_triangle_list.triangleCount; i++) {
				const Renderer::ClipTriangle& clip_triangle = clip_triangle_list.Triangles[i];

				bounds_triangle_desc.SetScreenPositions(
					renderer.NDCToScreen(renderer.ClipToNDC(clip_triangle.Vertices[0].ClipPos), GetViewPort()),
					renderer.NDCToScreen(renderer.ClipToNDC(clip_triangle.Vertices[1].ClipPos), GetViewPort()),
					renderer.NDCToScreen(renderer.ClipToNDC(clip_triangle.Vertices[2].ClipPos), GetViewPort())
				);

				bounds_triangle_desc.SetDepths(
					clip_triangle.Vertices[0].ClipPos.w, 
					clip_triangle.Vertices[1].ClipPos.w, 
					clip_triangle.Vertices[2].ClipPos.w
				);

				renderer.DrawTriangle(bounds_triangle_desc, GetViewPort());
			}
		}



		/*************************************************************
		 *   본 계층구조들을 선으로 그어서 표시한다....
		 ********/
		const float wHalf = 5.f;
		const float hHalf = 5.f;

		const Vector3 p1 = Vector3(-wHalf, -hHalf, 0.f);
		const Vector3 p2 = Vector3(wHalf, -hHalf, 0.f);
		const Vector3 p3 = Vector3(-wHalf, hHalf, 0.f);
		const Vector3 p4 = Vector3(wHalf, hHalf, 0.f);

		Renderer::TriangleDescription bone_triangle_desc;
		bone_triangle_desc.FillUpColor = Color::Black;

		for (uint32_t boneIdx = 0; boneIdx < mesh.Bones.size(); boneIdx++) {

			const Bone&	bone	= mesh.Bones[boneIdx];
			Transform*  bone_tr = bone_trs[boneIdx].Get();

			if (bone_tr==nullptr) {
				continue;
			}

			const Vector3& bone_pos       = bone_tr->GetWorldPosition();
			const Vector2  bone_ScreenPos = renderer.NDCToScreen(renderer.ClipToNDC(P * Vector4(bone_pos, 1.f)), GetViewPort());

			const Vector2 sp1 = (bone_ScreenPos + p1);
			const Vector2 sp2 = (bone_ScreenPos + p2);
			const Vector2 sp3 = (bone_ScreenPos + p3);
			const Vector2 sp4 = (bone_ScreenPos + p4);

			Color rectColor = Color::Yellow;

			//부모 본이 존재할 경우에만 그린다...
			if (bone.Parent_BoneIdx >= 0 && bone_tr->GetParent()!=Transform::GetRoot())
			{
				const Bone& parent_bone    =  mesh.Bones[bone.Parent_BoneIdx];
				Transform*  parent_bone_tr = bone_trs[bone.Parent_BoneIdx].Get();

				if (parent_bone_tr!=nullptr) {
					const Vector3& parent_pos = parent_bone_tr->GetWorldPosition();
					const Vector2  parent_ScreenPos = renderer.NDCToScreen(renderer.ClipToNDC(P * Vector4(parent_pos, 1.f)), GetViewPort());

					const Vector2 bone2parent       = (bone_ScreenPos - parent_ScreenPos);
					const Vector2 bone2parent_Dir   = bone2parent.GetNormalized();
					const Vector2 bone2parent_right = Vector2(-bone2parent_Dir.y, bone2parent_Dir.x);

					const Vector2 arrow1_pos = parent_ScreenPos + (bone2parent_right * 5.f);
					const Vector2 arrow2_pos = parent_ScreenPos - (bone2parent_right * 5.f);

					Color boneColor = Color::Red;
					renderer.DrawLine(boneColor, bone_ScreenPos, parent_ScreenPos, GetViewPort());
					renderer.DrawLine(boneColor, bone_ScreenPos, arrow1_pos, GetViewPort());
					renderer.DrawLine(boneColor, bone_ScreenPos, arrow2_pos, GetViewPort());
					renderer.DrawLine(boneColor, parent_ScreenPos, arrow1_pos, GetViewPort());
					renderer.DrawLine(boneColor, parent_ScreenPos, arrow2_pos, GetViewPort());

					rectColor = Color::Blue;
				}
			}


			if (input.WasPressedThisFrame(KeyCode::Left_Mouse)) {
				const Vector2 mpos = input.GetMouseScreenPosition();

				bool checkUp = (mpos.x > sp1.x && mpos.x < sp2.x) && (mpos.y > sp1.y && mpos.y > sp2.y);
				bool checkDown = (mpos.x > sp3.x && mpos.x < sp4.x) && (mpos.y < sp3.y && mpos.y < sp4.y);

				//해당 본의 버튼을 클릭했는가?
				if (checkUp && checkDown){
					selected_boneIdx = boneIdx;
				}
			}

			if (selected_boneIdx==boneIdx) {
				rectColor = Color::Green;
			}

			renderer.DrawLine(rectColor, sp1, sp2, GetViewPort());
			renderer.DrawLine(rectColor, sp1, sp3, GetViewPort());
			renderer.DrawLine(rectColor, sp2, sp4, GetViewPort());
			renderer.DrawLine(rectColor, sp3, sp4, GetViewPort());
		}



		/***************************************************************
		 *   스키닝 행렬을 계산하고, 캐싱한다...
		 ********/
		for (uint32_t i = 0; i < bone_trs.size(); i++) {
			const Bone& bone    = mesh.Bones[i];
			Transform*  bone_tr = bone_trs[i].Get();

			if (bone_tr==nullptr) {
				continue;
			}

			skinning_mats[i] = (bone_tr->GetTRS() * bone.BindingPose.GetTRS_Inverse());
		}



		/****************************************************************
		 *   모든 버텍스마다 적절한 블랜딩 행렬을 구축해 곱해준다....
		 *********/
		for (uint32_t i = 0; i < mesh.Vertices.size(); i++) {
			const Vertex&						vertex = mesh.Vertices[i];
			const Vertex::SkinDeformDescriptor& desc   = vertex.SkinDeformDesc;

			Vector4& blending_vertex = blending_vertices[i];
			blending_vertex = Vector4::Zero;

			for (uint32_t j = 0; j < desc.Weight_Count; j++) {
				const SkinDeformWeight& weight = mesh.DeformWeights[desc.Weight_StartIdx + j];
				blending_vertex += (skinning_mats[weight.BoneTransformIdx] * Vector4(vertex.ObjPos, 1.f)) * weight.Weight;
			}
		}



		/**************************************************************
		 *    주어진 위치에 서브메시별로 삼각형들을 그립니다...
		 ********/
		Renderer::TriangleDescription triangle_desc;

		for (uint32_t subMeshIdx = 0, triangleIdx = 0; subMeshIdx < mesh.SubMeshs.size(); subMeshIdx++) {
			bool	   twoSide       = false;
			Texture2D* mappedTexture = nullptr;

			if (subMeshIdx < m_materials.size()) {
				Material& mat				= m_materials[subMeshIdx];
				triangle_desc.MappedTexture = mat.MappedTexture.Get();
			}

			
			const SubMesh& subMesh  = mesh.SubMeshs[subMeshIdx];
			const uint32_t goal_idx = (triangleIdx + subMesh.Triangle_Count);

			while(triangleIdx < goal_idx){

				const IndexedTriangle& triangle = mesh.Triangles[triangleIdx++];

				const Vertex& vertex1 = mesh.Vertices[triangle.Indices[0]];
				const Vertex& vertex2 = mesh.Vertices[triangle.Indices[1]];
				const Vertex& vertex3 = mesh.Vertices[triangle.Indices[2]];

				const Vector4& blending_ObjPos1 = blending_vertices[triangle.Indices[0]];
				const Vector4& blending_ObjPos2 = blending_vertices[triangle.Indices[1]];
				const Vector4& blending_ObjPos3 = blending_vertices[triangle.Indices[2]];

				const Vector4 clipPos1 = (P * blending_ObjPos1);
				const Vector4 clipPos2 = (P * blending_ObjPos2);
				const Vector4 clipPos3 = (P * blending_ObjPos3);

				const Vector3 ndcPos1 = renderer.ClipToNDC(clipPos1);
				const Vector3 ndcPos2 = renderer.ClipToNDC(clipPos2);
				const Vector3 ndcPos3 = renderer.ClipToNDC(clipPos3);


				/*----------------------------------------------
				 *   삼각형이 카메라와 같은 방향을 보고 있다면,
				 *   삼각형을 그리기를 넘어간다....
				 *-----*/
				if (renderer.UseBackfaceCulling && twoSide==false) 
				{
					const Vector3 u = (ndcPos1 - ndcPos3);
					const Vector3 v = (ndcPos2 - ndcPos3);
					if (Vector3::Dot(Vector3::Forward, Vector3::Cross(u, v)) >= 0.f) {
						continue;
					}
				}



				/*--------------------------------------------
				 *   삼각형 클립핑을 진행한다....
				 **********/
				clip_triangle_list.triangleCount = 1;
				clip_triangle_list.Triangles[0]  = Renderer::ClipTriangle(
					Renderer::ClipVertex( clipPos1, vertex1.UvPos ), 
					Renderer::ClipVertex( clipPos2, vertex2.UvPos ),
					Renderer::ClipVertex( clipPos3, vertex3.UvPos )
				);

				//+Z, -Z 평면에 대한 클립핑을 적용한다...
				renderer.ClippingTriangle(clip_triangle_list, renderer.ClippingTest_Far, renderer.SolveT_Far);
				renderer.ClippingTriangle(clip_triangle_list, renderer.ClippingTest_Near, renderer.SolveT_Near);

				//+X, -X 평면에 대한 클립핑을 적용한다...
				renderer.ClippingTriangle(clip_triangle_list, renderer.ClippingTest_Right, renderer.SolveT_Right);
				renderer.ClippingTriangle(clip_triangle_list, renderer.ClippingTest_Left, renderer.SolveT_Left);

				//+Y, -Y 평면에 대한 클립핑을 적용한다...
				renderer.ClippingTriangle(clip_triangle_list, renderer.ClippingTest_Up, renderer.SolveT_Up);
				renderer.ClippingTriangle(clip_triangle_list, renderer.ClippingTest_Down, renderer.SolveT_Down);



				/*------------------------------------------
				 *   분할된 삼각형들을 랜더링한다...
				 *******/
				for (uint32_t i = 0; i < clip_triangle_list.triangleCount; i++) {
					const Renderer::ClipTriangle& triangle = clip_triangle_list.Triangles[i];

					const Renderer::ClipVertex& vertex1 = triangle.Vertices[0];
					const Renderer::ClipVertex& vertex2 = triangle.Vertices[1];
					const Renderer::ClipVertex& vertex3 = triangle.Vertices[2];

					triangle_desc.SetScreenPositions(
						renderer.NDCToScreen(renderer.ClipToNDC(vertex1.ClipPos), GetViewPort()),
						renderer.NDCToScreen(renderer.ClipToNDC(vertex2.ClipPos), GetViewPort()),
						renderer.NDCToScreen(renderer.ClipToNDC(vertex3.ClipPos), GetViewPort())
					);

					triangle_desc.SetUvPositions(
						vertex1.UvPos, vertex2.UvPos, vertex3.UvPos
					);

					triangle_desc.SetDepths(
						vertex1.ClipPos.w, vertex2.ClipPos.w, vertex3.ClipPos.w
					);

					renderer.DrawTriangle(triangle_desc, GetViewPort());
				}

			}
		}


		/***********************************************************
		 *   디버그 출력...
		 *******/
		renderer.SetPixel(Color::Purple, input.GetMouseScreenPosition(), GetViewPort());
		renderer.DrawTextField(w$(
			L"selected boneIdx: (", selected_boneIdx, L"/ ", mesh.Bones.size(), L")"
			L"\nchildCount: ", control_tr.GetChildCount(),
			L"\n\nmousePos: ", input.GetMouseScreenPosition(),
			L"\ntest result: ", test_str.c_str()),
			Vector2Int(0, 500), 
			GetViewPort()
		);
		#pragma endregion
	}

	void Example10_PlaneTest2D(float moveSpeed, float rotSpeed, float deltaTime)
	{
		#pragma region
		Renderer&			renderer     = GetRenderer();
		const InputManager& input	     = GetInputManager();
		const float			moveSpeedSec = (moveSpeed * deltaTime);
		const float			rotSpeedSec  = (rotSpeed * deltaTime);


		/*************************************************************
		 *   임의의 벡터의 위치와 평면의 위치, 크기, 회전량을 조작한다....
		 ********/
		static Vector2 vector_worldPos = Vector2::Zero;
		static Vector2 plane_worldPos = Vector2::Zero;
		static Vector2 plane_scale    = Vector2::One * 300.f;
		static float   plane_angle    = 0.f;

		plane_angle += input.GetAxis(KeyCode::Q, KeyCode::E);

		plane_worldPos += Vector2(
			input.GetAxis(KeyCode::A, KeyCode::D) * moveSpeedSec,
			input.GetAxis(KeyCode::S, KeyCode::W) * moveSpeedSec
		);

		vector_worldPos += Vector3(
			input.GetAxis(KeyCode::Left, KeyCode::Right) * moveSpeedSec,
			input.GetAxis(KeyCode::Down, KeyCode::Up) * moveSpeedSec
		);



		/******************************************************************
		 *   임의의 점과 평면의 최종 위치를 계산한다....
		 *******/
		const float rad = (Math::Angle2Rad * plane_angle);
		const float c   = Math::Cos(rad);
		const float s   = Math::Sin(rad);

		const Matrix3x3 T = {
			Vector3::BasisX,
			Vector3::BasisY,
			Vector3(plane_worldPos, 1.f)
		};

		const Matrix3x3 R = {
			Vector3(c, s, 0.f),
			Vector3(-s, c, 0.f),
			Vector3::BasisZ
		};

		const Matrix3x3 S = {
			(Vector3::BasisX * plane_scale.x),
			(Vector3::BasisY * plane_scale.y),
			Vector3::BasisZ
		};

		const Matrix3x3 TRS = (T * R * S);

		const Vector3 plane_objPos1       = Vector3(-.5f, 0.f, 1.f);
		const Vector3 plane_objPos2       = Vector3(.5f, 0.f, 1.f);
		const Vector3 plane_normal_objPos = Vector3(0.f, .5f, 1.f);

		const Vector3 plane_worldPos1		= (TRS * plane_objPos1);
		const Vector3 plane_worldPos2		= (TRS * plane_objPos2);
		const Vector3 plane_normal_worldPos = (TRS * plane_normal_objPos);

		const Vector2 plane_screenPos1		 = renderer.WorldToScreen(plane_worldPos1, GetViewPort());
		const Vector2 plane_screenPos2		 = renderer.WorldToScreen(plane_worldPos2, GetViewPort());
		const Vector2 plane_normal_screenPos = renderer.WorldToScreen(plane_normal_worldPos, GetViewPort());
		const Vector2 plane_center_screenPos = plane_screenPos1 + (plane_screenPos2 - plane_screenPos1) * .5f;
		const Vector2 vector_screenPos		 = renderer.WorldToScreen(vector_worldPos, GetViewPort());
		const Vector2 origin_screenPos		 = renderer.WorldToScreen(Vector2::Zero, GetViewPort());



		/******************************************************************
		 *   벡터와 평면의 교차 관계를 계산하고 표시한다...
		 *******/

		/*+---------------------------------------------------------+
		  | 단순하게 평면의 점 p에서, 원점에서 어딘가로 향하는 임의의 벡터 |
		  | v를 뺀 벡터를, 평면의 방향벡터 n과 내적했을 때 그 결과가 0보다 |
		  | 크면 방향이 같다는 말인데, 이는 평면 밖에 있음을 의미하는 간단한|
		  | 원리다. 이를 평면의 방정식이라고 한다.					    |
		  +---------------------------------------------------------+*/

		const bool  vOutofPlane = (Vector3::Dot(R.BasisY, (vector_worldPos - plane_worldPos1)) > 0);
		const float nDotvec		= Vector3::Dot(R.BasisY, vector_worldPos);
		const float proj_size	= Math::Abs(nDotvec);
		const float plane_dst   = plane_worldPos.GetMagnitude();
		const Color planeColor  = (vOutofPlane ? Color::Black : Color::Blue);


		/*---------------------------------------------------
		 *   벡터와, 벡터가 평면의 방향간의 직교투영을 표시한다..
		 *-----*/
		renderer.DrawLine(Color::Black, origin_screenPos, vector_screenPos, GetViewPort());
		renderer.DrawLine(Color::Red, origin_screenPos, renderer.WorldToScreen(R.BasisY * 50.f, GetViewPort()), GetViewPort());
		renderer.DrawLine(Color::Green, origin_screenPos, renderer.WorldToScreen(R.BasisY* nDotvec, GetViewPort()), GetViewPort());


		/*------------------------------------------------
		 *   평면과 평면의 방향을 표시한다...
		 *------*/
		renderer.DrawLine(planeColor, plane_screenPos1, plane_screenPos2, GetViewPort());
		renderer.DrawLine(Color::Red, plane_center_screenPos, plane_normal_screenPos, GetViewPort());


		/******************************************************
		 *  디버그 출력...
		 ******/
		renderer.DrawTextField(w$(
			L"vector_worldPos: ", vector_worldPos,
			L"\nplane_worldPos: ", plane_worldPos, 
			L"\nplane_scale: ", plane_scale, 
			L"\nplane_angle: ", plane_angle,
			L"\n\n\nT\n", T,
			L"\n\nR\n", R,
			L"\n\nS\n", S,
			L"\n\nTRS\n", TRS,
			L"\n\n\nplane_objnPos1: ", plane_objPos1, L"\nplane_screenPos1: ", plane_screenPos1,
			L"\n\nplane_objnPos2: ", plane_objPos2, L"\nplane_screenPos2: ", plane_screenPos2,
			L"\n\nproj_size: ", proj_size, L"\nplane_dst: ", plane_dst),
			Vector2Int(0, 100),
			GetViewPort()
		);
		#pragma endregion
	}
	void Example11_ProjectionTest2D(float moveSpeed, float rotSpeed, float deltaTime)
	{
		#pragma region
		Renderer& renderer = GetRenderer();
		const InputManager& input = GetInputManager();
		const float			moveSpeedSec = (moveSpeed * deltaTime);
		const float			rotSpeedSec = (rotSpeed * deltaTime);


		/*************************************************************
		 *   두 벡터의 위치를 조작한다....
		 ********/
		static Vector2 v1_worldPos = Vector2::Zero;
		static Vector2 v2_worldPos = Vector2::Zero;

		v1_worldPos += Vector3(
			input.GetAxis(KeyCode::Left, KeyCode::Right) * moveSpeedSec,
			input.GetAxis(KeyCode::Down, KeyCode::Up) * moveSpeedSec
		);

		v2_worldPos += Vector3(
			input.GetAxis(KeyCode::A, KeyCode::D) * moveSpeedSec,
			input.GetAxis(KeyCode::S, KeyCode::W) * moveSpeedSec
		);



		/******************************************************************
		 *   두 벡터를 그린다...
		 *******/
		const Vector2 v2_worldPos_normal = v2_worldPos.GetNormalized();
		const Vector2 v1_worldPos_normal = v1_worldPos.GetNormalized();

		const float oneProjTwo = Vector2::Dot(v1_worldPos, v2_worldPos_normal);
		const float c		   = Vector2::Dot(v2_worldPos_normal, v1_worldPos_normal);

		const Vector2 origin_screenPos     = renderer.WorldToScreen(Vector2::Zero, GetViewPort());
		const Vector2 v1_screenPos		   = renderer.WorldToScreen(v1_worldPos, GetViewPort());
		const Vector2 v2_screenPos		   = renderer.WorldToScreen(v2_worldPos, GetViewPort());
		const Vector2 oneProjTwo_screenPos = renderer.WorldToScreen(v2_worldPos.GetNormalized() * oneProjTwo, GetViewPort());


		/*-------------------------------------------------
		 *   한 벡터의 크기가, 두 벡터 사이의 cos값에 의해서
		 *   스케일링되면서 단위벡터 쪽의 그림자가 되는 것.
		 *----*/
		renderer.DrawLine(Color::Red, origin_screenPos, v1_screenPos, GetViewPort());
		renderer.DrawLine(Color::Blue, origin_screenPos, v2_screenPos, GetViewPort());
		renderer.DrawLine(Color::Green, origin_screenPos, oneProjTwo_screenPos, GetViewPort());

		

		/******************************************************
		 *  디버그 출력...
		 ******/
		renderer.DrawTextField(w$(
			L"vector1_screenPos: ", v1_screenPos,
			L"\nvector2_screenPos: ", v2_screenPos,
			L"\n\noneProjTwo: ", oneProjTwo,
			L"\ncos: ", c),
			Vector2Int(0, 200), 
			GetViewPort()
		);
		#pragma endregion
	}
	void Example12_TriangleClippingTest2D(float moveSpeed, float deltaTime)
	{
		#pragma region
		Renderer&			renderer	 = GetRenderer();
		const InputManager& input		 = GetInputManager();
		const float			moveSpeedSec = (moveSpeed * deltaTime);


		/****************************************************************************************
		 *   삼각형을 구성하는 세 점의 위치를 조작한다...
		 *******/
		static float fov = 60.f;
		static float f   = 500.f;
		static float n   = 100.f;

		static Vector2 p1  = Vector2(-66.f, 320.f);
		static Vector2 p2  = Vector2(48.f, 302.f);
		static Vector2 p3  = Vector2(-1.f, 233.f);


		p1 += Vector2(
			input.GetAxis(KeyCode::A, KeyCode::D) * moveSpeedSec,
			input.GetAxis(KeyCode::S, KeyCode::W) * moveSpeedSec
		);

		p2 += Vector2(
			input.GetAxis(KeyCode::J, KeyCode::L) * moveSpeedSec,
			input.GetAxis(KeyCode::K, KeyCode::I) * moveSpeedSec
		);

		p3 += Vector2(
			input.GetAxis(KeyCode::Left, KeyCode::Right) * moveSpeedSec,
			input.GetAxis(KeyCode::Down, KeyCode::Up) * moveSpeedSec
		);

		n   += input.GetAxis(KeyCode::NUMPAD_5, KeyCode::NUMPAD_8);
		f   += input.GetAxis(KeyCode::NUMPAD_6, KeyCode::NUMPAD_9);
		fov += input.GetAxis(KeyCode::NUMPAD_1, KeyCode::NUMPAD_3);



		/*************************************************************************************
		 *   세 점을 사영공간으로 변환시킨다....
		 *******/
		const float rad = (Math::Angle2Rad * fov * .5f);
		const float d   = (1.f / Math::Tan(rad));
		const float a   = GetViewPort().RenderTarget.GetAspectRatio();
		const float k   = (-n-f) / (n-f);
		const float l   = -n-(k*n);

		const Matrix4x4 P = Matrix4x4(
			Vector4(d, 0.f, 0.f, 0.f),
			Vector4(0.f, (d*a), 0.f, 0.f),
			Vector4(0.f, 0.f, k, 1.f),
			Vector4(0.f, 0.f, l, 0.f)
		);

		const Vector4 clip1 = (P * Vector4(p1.x, 0.f, p1.y, 1.f));
		const Vector4 clip2 = (P * Vector4(p2.x, 0.f, p2.y, 1.f));
		const Vector4 clip3 = (P * Vector4(p3.x, 0.f, p3.y, 1.f));

		const Vector3 ndc1 = renderer.ClipToNDC(clip1);
		const Vector3 ndc2 = renderer.ClipToNDC(clip2);
		const Vector3 ndc3 = renderer.ClipToNDC(clip3);

		const Vector2 screenPos1 = renderer.NDCToScreen(ndc1, GetViewPort());
		const Vector2 screenPos2 = renderer.NDCToScreen(ndc1, GetViewPort());
		const Vector2 screenPos3 = renderer.NDCToScreen(ndc1, GetViewPort());



		/*********************************************************************************
		 *   클립핑에 필요한 람다 함수들을 정의한다....
		 *******/
		struct ClipVertex
		{
			Vector4 ClipPos;
			Vector3 WorldPos;
		};

		struct ClipTriangle
		{
			ClipVertex Vertices[3];
		};

		struct ClippingDescription
		{
			uint32_t from, to1, to2;
		};

		struct ClipTriangleList
		{
			uint32_t     triangleCount          = 0;
			Vector3      green_highlight_point  = Vector3(3000.f);
			Vector3      red_highlight_point2   = Vector3(3000.f);
			Vector3      purple_highlight_point = Vector3(3000.f);
			ClipTriangle Triangles[20];
		};

		/*---------------------------------------
		 *   삼각형을 절단하는 람다 함수의 정의...
		 *******/
		static const auto ClippingTriangle = [&](ClipTriangleList& outTriangleList, std::function<bool(const Vector4& clipPos)> checkClipping, std::function<float(const Vector4& fromClipPos, const Vector4& toClipPos)> SolveT)
		{
			uint32_t		    clipping_desc_count = 0;
			ClippingDescription clipping_descs[3];

			const uint32_t clipping_test_count = outTriangleList.triangleCount;
			for (uint32_t i = 0; i < clipping_test_count; i++) 
			{
				ClipTriangle& cur_triangle = outTriangleList.Triangles[i];

				/*-------------------------------------------------------
				 *   클립핑이 적용되어야할 점에 대한 서술자를 작성한다....
				 *******/
				clipping_desc_count = 0;

				//첫번째 점에 대한 클립핑 서술자를 작성한다...
				if (checkClipping(cur_triangle.Vertices[0].ClipPos) == true) {
					clipping_descs[clipping_desc_count++] = ClippingDescription{ 0, 1, 2 };
				}

				//두번째 점에 대한 클립핑 서술자를 작성한다...
				if (checkClipping(cur_triangle.Vertices[1].ClipPos) == true) {
					clipping_descs[clipping_desc_count++] = ClippingDescription{ 1, 0, 2 };
				}

				//세번째 점에 대한 클립핑 서술자를 작성한다...
				if (checkClipping(cur_triangle.Vertices[2].ClipPos) == true) {
					clipping_descs[clipping_desc_count++] = ClippingDescription{ 2, 0, 1 };
				}


				/*--------------------------------------------------------
				 *   클립핑할 점이 하나일 경우의 처리...
				 *******/
				if (clipping_desc_count == 1) {
					const ClippingDescription& desc = clipping_descs[0];

					const ClipVertex  fromVertex = cur_triangle.Vertices[desc.from];
					const ClipVertex& toVertex1  = cur_triangle.Vertices[desc.to1];
					const ClipVertex& toVertex2  = cur_triangle.Vertices[desc.to2];

					const float t1 = SolveT(fromVertex.ClipPos, toVertex1.ClipPos);
					const float t2 = SolveT(fromVertex.ClipPos, toVertex2.ClipPos);

					//첫번째 삼각형의 버텍스를 갱신한다...
					cur_triangle.Vertices[desc.from].ClipPos  = (fromVertex.ClipPos * t1) + (toVertex1.ClipPos * (1.f-t1));
					cur_triangle.Vertices[desc.from].WorldPos = (fromVertex.WorldPos * t1) + (toVertex1.WorldPos * (1.f-t1));

					//두번째 삼각형의 버텍스를 갱신한다...
					ClipTriangle& nxt_triangle = outTriangleList.Triangles[outTriangleList.triangleCount++];
					nxt_triangle.Vertices[0]		  = cur_triangle.Vertices[desc.from];
					nxt_triangle.Vertices[1]		  = toVertex2;
					nxt_triangle.Vertices[2].ClipPos  = (fromVertex.ClipPos * t2) + (toVertex2.ClipPos * (1.f-t2));
					nxt_triangle.Vertices[2].WorldPos = (fromVertex.WorldPos * t2) + (toVertex2.WorldPos * (1.f-t2));
				}



				/*---------------------------------------------------------
				 *   클립핑할 점이 두 개일 경우의 처리...
				 *******/
				else if(clipping_desc_count==2){
					const ClippingDescription& desc1 = clipping_descs[0];
					const ClippingDescription& desc2 = clipping_descs[1];

					const ClipVertex  fromVertex1 = cur_triangle.Vertices[desc1.from];
					const ClipVertex  fromVertex2 = cur_triangle.Vertices[desc2.from];
					const ClipVertex& toVertex    = cur_triangle.Vertices[(desc1.to1 == desc2.to1 ? desc1.to1 : desc2.to2)];

					const float t1 = SolveT(fromVertex1.ClipPos, toVertex.ClipPos);
					const float t2 = SolveT(fromVertex2.ClipPos, toVertex.ClipPos);

					//기존 삼각형의 버텍스를 갱신한다...
					cur_triangle.Vertices[desc1.from].ClipPos  = (toVertex.ClipPos * (1.f - t1)) + (fromVertex1.ClipPos * t1);
					cur_triangle.Vertices[desc1.from].WorldPos = (toVertex.WorldPos * (1.f - t1)) + (fromVertex1.WorldPos * t1);

					cur_triangle.Vertices[desc2.from].ClipPos = (toVertex.ClipPos * (1.f - t2)) + (fromVertex2.ClipPos * t2);
					cur_triangle.Vertices[desc2.from].WorldPos = (toVertex.WorldPos * (1.f - t2)) + (fromVertex2.WorldPos * t2);
				}


				/*------------------------------------------------------
				 *  클립핑할 점이 세 개일 경우의 처리...
				 *******/
				else if (clipping_desc_count==3) {
					cur_triangle = outTriangleList.Triangles[outTriangleList.triangleCount-1];
					outTriangleList.triangleCount--;
				}

			}
		};


		/*--------------------------------------------
		 *   +Z, -Z에 대한 람다함수를 정의한다...
		 *******/
		static const auto Condition_far = [](const Vector4& clipPos)->float {
			return (clipPos.z > clipPos.w);
		};

		static const auto SolveT_far = [](const Vector4& fromClipPos, const Vector4& toClipPos)->float {
			return (toClipPos.w - toClipPos.z) / (-toClipPos.z + fromClipPos.z + toClipPos.w - fromClipPos.w);
		};

		static const auto Condition_near = [](const Vector4& clipPos)->float {
			return (clipPos.z < -clipPos.w);
		};

		static const auto SolveT_near = [](const Vector4& fromClipPos, const Vector4& toClipPos)->float {
			return (-toClipPos.w - toClipPos.z) / (-toClipPos.z + fromClipPos.z - toClipPos.w + fromClipPos.w);
		};


		/*--------------------------------------------
		 *   +X, -X에 대한 람다함수를 정의한다...
		 *******/
		static const auto Condition_right = [](const Vector4& clipPos)->float {
			return (clipPos.x > clipPos.w);
		};

		static const auto SolveT_right = [](const Vector4& fromClipPos, const Vector4& toClipPos)->float {
			return (toClipPos.w - toClipPos.x) / (-toClipPos.x + fromClipPos.x + toClipPos.w - fromClipPos.w);
		};

		static const auto Condition_left = [](const Vector4& clipPos)->float {
			return (clipPos.x < -clipPos.w);
		};

		static const auto SolveT_left = [](const Vector4& fromClipPos, const Vector4& toClipPos)->float {
			return (-toClipPos.w - toClipPos.x) / (-toClipPos.x + fromClipPos.x - toClipPos.w + fromClipPos.w);
		};


		/*--------------------------------------------
		 *   +Y, -Y에 대한 람다함수를 정의한다...
		 *******/
		static const auto Condition_up = [](const Vector4& clipPos)->float {
			return (clipPos.y > clipPos.w);
		};

		static const auto SolveT_up = [](const Vector4& fromClipPos, const Vector4& toClipPos)->float {
			return (toClipPos.w - toClipPos.y) / (-toClipPos.y + fromClipPos.y + toClipPos.w - fromClipPos.w);
		};

		static const auto Condition_down = [](const Vector4& clipPos)->float {
			return (clipPos.y < -clipPos.w);
		};

		static const auto SolveT_down = [](const Vector4& fromClipPos, const Vector4& toClipPos)->float {
			return (-toClipPos.w - toClipPos.y) / (-toClipPos.y + fromClipPos.y - toClipPos.w + fromClipPos.w);
		};






		/********************************************************************************
		 *   삼각형 클립핑을 적용한다....
		 ******/
		ClipTriangleList triangle_list;
		triangle_list.triangleCount = 1;
		triangle_list.Triangles[0]  = ClipTriangle{ ClipVertex{ clip1, p1 },ClipVertex{ clip2, p2 },ClipVertex{ clip3, p3 } };

		//+Z, -Z에 대한 클립핑을 적용한다...
		ClippingTriangle(triangle_list, Condition_far, SolveT_far);
		ClippingTriangle(triangle_list, Condition_near, SolveT_near);

		//+X, -X에 대한 클립핑을 적용한다...
		ClippingTriangle(triangle_list, Condition_right, SolveT_right);
		ClippingTriangle(triangle_list, Condition_left, SolveT_left);




		/*********************************************************************************
		 *  화면에 각 요소들을 출력한다....
		 *******/
		const Vector2 yOffset = (Vector2::Down * GetViewPort().RenderTarget.GetBackBufferHeightf() * .4f);
		const auto    DrawRect = [&](const Vector2& rectCenterScreenPos, const Color& color, const float size = 5.f)
		{
			for (float y = -size; y <= size; y += 1.f) {
				for (float x = -size; x <= size; x += 1.f) {
					renderer.SetPixel(color, (rectCenterScreenPos + Vector2(x, y)), GetViewPort());
				}
			}
		};



		/*-------------------------------
		 *   near/far 영역을 표시한다....
		 ******/
		const Vector2 n_startScreenPos = renderer.WorldToScreen(Vector2(GetViewPort().RenderTarget.GetBackBufferWidthf() * -.5f, n) + yOffset, GetViewPort());
		const Vector2 n_endScreenPos   = renderer.WorldToScreen(Vector2(GetViewPort().RenderTarget.GetBackBufferWidthf() * .5f, n) + yOffset, GetViewPort());

		const Vector2 f_startScreenPos = renderer.WorldToScreen(Vector2(GetViewPort().RenderTarget.GetBackBufferWidthf() * -.5f, f) + yOffset, GetViewPort());
		const Vector2 f_endScreenPos   = renderer.WorldToScreen(Vector2(GetViewPort().RenderTarget.GetBackBufferWidthf() * .5f, f) + yOffset, GetViewPort());

		const Vector2 form_startScreenPos = renderer.WorldToScreen(Vector2(GetViewPort().RenderTarget.GetBackBufferWidthf() * -1.f, d) + yOffset, GetViewPort());
		const Vector2 form_endScreenPos   = renderer.WorldToScreen(Vector2(GetViewPort().RenderTarget.GetBackBufferWidthf() * 1.f, d) + yOffset, GetViewPort());

		renderer.DrawLine(Color::Black, n_startScreenPos, n_endScreenPos, GetViewPort());
		renderer.DrawTextField(w$(L"near: ", n), n_endScreenPos+(Vector2::Left * 200.f), GetViewPort());

		renderer.DrawLine(Color::Black, f_startScreenPos, f_endScreenPos, GetViewPort());
		renderer.DrawTextField(w$(L"far: ", f), f_endScreenPos+(Vector2::Left * 200.f), GetViewPort());

		renderer.DrawLine(Color::Blue, form_startScreenPos, form_endScreenPos, GetViewPort());



		/*------------------------
		 *   시야각을 표시한다....
		 ********/
		const float rad_left  = (Math::Angle2Rad * (90.f + fov * .5f));
		const float rad_right = (Math::Angle2Rad * (90.f - fov * .5f));

		const Vector2 originPos       = renderer.WorldToScreen(yOffset, GetViewPort());
		const Vector2 insight_left    = renderer.WorldToScreen(Vector2(Math::Cos(rad_left), Math::Sin(rad_left)) * 1000.f + yOffset, GetViewPort());
		const Vector2 insight_right   = renderer.WorldToScreen(Vector2(Math::Cos(rad_right), Math::Sin(rad_right)) * 1000.f + yOffset, GetViewPort());
		const Vector2 insight_forward = renderer.WorldToScreen(Vector2::Up * 1000.f, GetViewPort());

		renderer.DrawLine(Color::Black, originPos, insight_left, GetViewPort());
		renderer.DrawLine(Color::Black, originPos, insight_right, GetViewPort());
		renderer.DrawLine(Color::Green, originPos, insight_forward, GetViewPort());



		/*------------------------------
		 *   클립핑 된 점들을 표시한다...
		 ********/
		const Vector2 p1_screenPos = renderer.WorldToScreen(p1 + yOffset, GetViewPort());
		const Vector2 p2_screenPos = renderer.WorldToScreen(p2 + yOffset, GetViewPort());
		const Vector2 p3_screenPos = renderer.WorldToScreen(p3 + yOffset, GetViewPort());

		renderer.DrawLine(Color::Black, p1_screenPos, p2_screenPos, GetViewPort());
		renderer.DrawLine(Color::Black, p1_screenPos, p3_screenPos, GetViewPort());
		renderer.DrawLine(Color::Black, p2_screenPos, p3_screenPos, GetViewPort());

		for (uint32_t i = 0; i < triangle_list.triangleCount; i++) {
			const ClipTriangle& triangle = triangle_list.Triangles[i];

			const Vector2 screenPos1 = renderer.WorldToScreen(triangle.Vertices[0].WorldPos + yOffset, GetViewPort());
			const Vector2 screenPos2 = renderer.WorldToScreen(triangle.Vertices[1].WorldPos + yOffset, GetViewPort());
			const Vector2 screenPos3 = renderer.WorldToScreen(triangle.Vertices[2].WorldPos + yOffset, GetViewPort());

			renderer.DrawLine(Color::Red, screenPos1, screenPos2, GetViewPort());
			renderer.DrawLine(Color::Red, screenPos1, screenPos3, GetViewPort());
			renderer.DrawLine(Color::Red, screenPos2, screenPos3, GetViewPort());

			DrawRect(screenPos1, Color::Blue);
			DrawRect(screenPos2, Color::Blue);
			DrawRect(screenPos3, Color::Blue);
		}



		/*-------------------------------
		 *  세 점들을 표시한다....
		 ********/
		DrawRect(p1_screenPos, Color::Black);
		DrawRect(p2_screenPos, Color::Black);
		DrawRect(p3_screenPos, Color::Black);

		DrawRect(renderer.WorldToScreen(triangle_list.green_highlight_point + yOffset, GetViewPort()), Color::Green);
		DrawRect(renderer.WorldToScreen(triangle_list.red_highlight_point2 + yOffset, GetViewPort()), Color::Red);
		DrawRect(renderer.WorldToScreen(triangle_list.purple_highlight_point + yOffset, GetViewPort()), Color::Purple);



		/*--------------------------
		 *  디버그 출력...
		 *******/
		renderer.DrawTextField(w$(
			L"p1: ", p1,
			L"\np2: ", p2,
			L"\np3: ", p3,
			L"\n\nfov: ", fov,
			L"\nn: ", n,
			L"\nf: ", f,
			L"\nd: ", d,
			L"\n\nP\n", P,
			L"\n\nclip1: ", clip1,
			L"\nndc1: ", ndc1,
			L"\n\nclip2: ", clip2,
			L"\nndc2: ", ndc2,
			L"\n\nclip3: ", clip3,
			L"\nndc3: ", ndc3,
			L"\n\ntriangle_count: (", triangle_list.triangleCount, L"/", sizeof(triangle_list.Triangles)/sizeof(ClipTriangle), L")"),
			Vector2Int(0, 200), 
			GetViewPort()
		);

		#pragma endregion
	}
	void Example13_TestUniqueObject()
	{
		#pragma region
		//class TestObject : public UniqueableObject
		//{
		//public:
		//	uint32_t Value;

		//	TestObject();
		//	TestObject(uint32_t value) :Value(value){};
		//};

		//static std::vector<TestObject>			obj_list;
		//static std::vector<WeakPtr<TestObject>> ptr_list;

		//

		///**************************************************
		// *  초기화를 진행한다....
		// ******/
		//if (obj_list.size()==0) {

		//	for (uint32_t i = 0; i < 30; i++) {
		//		obj_list.push_back(TestObject(i));
		//	}

		//	for (uint32_t i = 0; i < 30; i++) {
		//		ptr_list.push_back(WeakPtr<TestObject>(&obj_list[i]));
		//	}
		//}


		///***************************************************
		// *   원본 데이터 목록을 출력한다...
		// *******/
		//static std::wstring output;

		//const InputManager& input = GetInputManager();

		//static uint32_t i = 0;
		//if (input.WasPressedThisFrame(KeyCode::Space)) {
		//	obj_list[i++].Make_UnUnique();
		//}

		//static uint32_t i2 = 0;
		//if (input.WasPressedThisFrame(KeyCode::Right)) {
		//	obj_list[i2 + 1] = std::move(obj_list[i2]);

		//	i2++;
		//}

		//static uint32_t i3 = 29;
		//if (input.WasPressedThisFrame(KeyCode::Left)) {
		//	obj_list[i3-1] = std::move(obj_list[i3]);

		//	i3--;
		//}

		//if (input.WasPressedThisFrame(KeyCode::Shift)) {
		//	TestObject& obj = obj_list[i];
		//	obj.Value++;
		//}


		//output.clear();
		//output += L"object_list\n";
		//for (uint32_t i = 0; i < obj_list.size(); i++) {
		//	output += (const std::wstring&)w$(
		//		L"obj[", i, L"].value: ", obj_list[i].Value, L"\n"
		//	);
		//}
		//
		//GetRenderer().DrawTextField(output, Vector2Int(400));


		//output.clear();
		//output += L"ptr_list\n";
		//for (uint32_t i = 0; i < ptr_list.size(); i++) {
		//	TestObject* raw_ptr = ptr_list[i].Get();

		//	if (raw_ptr!=nullptr) {
		//		output += (const std::wstring&)w$(
		//			L"ptr[", i, L"]->value: ", raw_ptr->Value, L"\n"
		//		);
		//		continue;
		//	}

		//	output += (const std::wstring&)w$(
		//		L"ptr[", i, L"]: nullptr\n"
		//	);
		//}

		//GetRenderer().DrawTextField(output, Vector2Int(800));
		#pragma endregion
	}

	void Example14_TestTransform(std::vector<Texture2D>& textures, float deltaTime)
	{
		#pragma region
		Renderer&			renderer	 = GetRenderer();
		const InputManager& input		 = GetInputManager();
		const float         slowScale	 = (input.IsInProgress(KeyCode::Space) ? .2f : 1.f);
		const float			moveSpeedSec = (deltaTime * 200.f * slowScale);
		const float			rotSpeedSec  = (deltaTime * 100.f * slowScale);



		/*********************************************************
		 *   객체들을 초기화한다....
		 *********/
		static std::vector<WeakPtr<Transform>> tr_list;

		if (tr_list.size()==0) 
		{
			tr_list.resize(6);

			PngImporter::ImportResult pngRet = PngImporter::Imports(m_textures, {
					L"Resources/zero.png",
					L"Resources/one.png",
					L"Resources/two.png",
					L"Resources/three.png",
					L"Resources/four.png",
					L"Resources/five.png",
					L"Resources/six.png"
				});

			if (pngRet.Success == false) {
				throw "png import failed!!!";
			}

			for (uint32_t i = 0; i < tr_list.size(); i++) {
				tr_list[i] = Transform::CreateTransform();
			}
			
			Transform& tr_0 = *tr_list[0].Get();
			tr_0.SetLocalPositionAndScaleAndRotation(
				Vector3(0.f, 0.f, 0.f),
				Vector3::One * 100.f,
				Quaternion::Identity
			);

			Transform& tr_1 = *tr_list[1].Get();
			tr_1.SetLocalPositionAndScaleAndRotation(
				Vector3(100.f, 0.f, 0.f),
				Vector3::One * 100.f,
				Quaternion::Identity
			);

			Transform& tr_2 = *tr_list[2].Get();
			tr_2.SetLocalPositionAndScaleAndRotation(
				Vector3(30.f, 100.f, 0.f),
				Vector3::One * 100.f,
				Quaternion::Identity
			);

			Transform& tr_3 = *tr_list[3].Get();
			tr_3.SetLocalPositionAndScaleAndRotation(
				Vector3(100.f, 60.f, 0.f),
				Vector3::One * 100.f,
				Quaternion::Identity
			);

			Transform& tr_4 = *tr_list[4].Get();
			tr_4.SetLocalPositionAndScaleAndRotation(
				Vector3(200.f, 40.f, 0.f),
				Vector3::One * 100.f,
				Quaternion::Identity
			);

			Transform& tr_5 = *tr_list[5].Get();
			tr_5.SetLocalPositionAndScaleAndRotation(
				Vector3(300.f, 200.f, 0.f),
				Vector3::One * 100.f,
				Quaternion::Identity
			);
		}
		



		/*********************************************************
		 *    선택한 객체를 조작한다....
		 *********/
		static uint32_t tr_idx = 0;

		for (uint32_t i = 0; i < tr_list.size(); i++) 
		{
			//현재 조작중인 대상이 부모를 제거한다...
			if (input.WasPressedThisFrame(KeyCode::X)) {
				tr_list[tr_idx]->SetParent(Transform::GetRoot());
			}

			//현재 조작중인 대상을 삭제한다...
			if (input.WasPressedThisFrame(KeyCode::P)) {
				Transform::DestroyTransform(tr_list[tr_idx].Get());
			}

			if (input.WasPressedThisFrame(KeyCode(uint32_t(KeyCode::NUMPAD_0) + i)))
			{
				//현재 조작중인 대상을, 선택한 대상의 자식으로 넣는다....
				if (input.IsInProgress(KeyCode::Z)) {
					tr_list[tr_idx]->SetParent(tr_list[i].Get());
				}

				//조작 대상을, 선택한 대상으로 변경한다....
				else tr_idx = i;
			}
		}

		Transform& tr = *tr_list[tr_idx].Get();

		tr.SetWorldPositionAndScaleAndRotation(
			tr.GetWorldPosition() + Vector3(input.GetAxis(KeyCode::Left, KeyCode::Right), input.GetAxis(KeyCode::Down, KeyCode::Up), 0.f) * moveSpeedSec,
			tr.GetWorldScale()    + Vector3(input.GetAxis(KeyCode::A, KeyCode::D), input.GetAxis(KeyCode::S, KeyCode::W)) + (Vector3::One * input.GetAxis(KeyCode::F, KeyCode::R)),
			Quaternion::AngleAxis(input.GetAxis(KeyCode::Q, KeyCode::E) * rotSpeedSec, Vector3::Back) * tr.GetWorldRotation()
		);




		/*********************************************************
		 *    모든 사각형들을 랜더링한다....
		 ********/
		const float wHalf = .5f;
		const float hHalf = .5f;

		Vector4 objPos1 = Vector4(-wHalf, hHalf, 0.f, 1.f);
		Vector4 objPos2 = Vector4(wHalf, hHalf, 0.f, 1.f);
		Vector4 objPos3 = Vector4(-wHalf, -hHalf, 0.f, 1.f);
		Vector4 objPos4 = Vector4(wHalf, -hHalf, 0.f, 1.f);

		Vector2 uvPos1 = Vector2(0.f, 0.f);
		Vector2 uvPos2 = Vector2(1.f, 0.f);
		Vector2 uvPos3 = Vector2(0.f, 1.f);
		Vector2 uvPos4 = Vector2(1.f, 1.f);

		static std::wstring debugTxt;
		debugTxt.clear();
		debugTxt += (const std::wstring&)w$(L"selected idx: ", tr_idx, L"\n(add parent mode: z~ / remove parent: x)\n-----------------------------------------------\n\n");
		
		Renderer::TriangleDescription triangle_desc;
		for (uint32_t i = 0; i < tr_list.size(); i++) 
		{
			Transform& cur = *tr_list[i].Get();
			Matrix4x4  TRS = cur.GetTRS();

			Vector2 screenPos1 = renderer.WorldToScreen(TRS * objPos1, GetViewPort());
			Vector2 screenPos2 = renderer.WorldToScreen(TRS * objPos2, GetViewPort());
			Vector2 screenPos3 = renderer.WorldToScreen(TRS * objPos3, GetViewPort());
			Vector2 screenPos4 = renderer.WorldToScreen(TRS * objPos4, GetViewPort());

			triangle_desc.MappedTexture = &textures[i];
			triangle_desc.SetDepths(i, i, i);


			/*------------------------------------------
			 *   첫번째 삼각형을 그린다...
			 ********/
			triangle_desc.SetUvPositions(uvPos1, uvPos2, uvPos3);
			triangle_desc.SetScreenPositions(screenPos1, screenPos2, screenPos3);
			renderer.DrawTriangle(triangle_desc, GetViewPort());


			/*------------------------------------------
			 *   두번째 삼각형을 그린다...
			 ********/
			triangle_desc.SetUvPositions(uvPos2, uvPos3, uvPos4);
			triangle_desc.SetScreenPositions(screenPos2, screenPos3, screenPos4);
			renderer.DrawTriangle(triangle_desc, GetViewPort());


			/*----------------------------------------
			 *   조작중인 객체라면, 테두리를 그린다...
			 ********/
			if (tr_idx==i) {
				renderer.DrawLine(Color::Green, screenPos1, screenPos2, GetViewPort());
				renderer.DrawLine(Color::Green, screenPos1, screenPos3, GetViewPort());
				renderer.DrawLine(Color::Green, screenPos2, screenPos4, GetViewPort());
				renderer.DrawLine(Color::Green, screenPos2, screenPos4, GetViewPort());
			}


			/*---------------------------------------
			 *   해당 객체에 대한 디버그 출력...
			 *******/
			debugTxt += (const std::wstring&)w$(
				L"transform[", i, L"].parent: ", (cur.GetParent() - Transform::GetRoot()),
				L"\nlocal_position", cur.GetLocalPosition(), L"\nlocal_scale: ", cur.GetLocalScale(), L"\nlocal_rotation: ", cur.GetLocalRotation(), L"\n-----------------------------------------------\n\n"
			);
		}




		/****************************************************
		 *  디버그 출력...
		 *******/
		renderer.DrawTextField(debugTxt, Vector2Int(0, 100), GetViewPort());

		#pragma endregion
	}

	#pragma endregion

	void Example15_DrawRenderMesh(float deltaTime)
	{
		#pragma region
		const InputManager& input	 = GetInputManager();
		Renderer&			renderer = GetRenderer();

		const float speedScale    = (input.IsInProgress(KeyCode::Space) ? .2f : 1.f);
		const float moveSpeedSec  = (100.f * speedScale * deltaTime);
		const float rotSpeedSec   = (200.f * speedScale * deltaTime);
		const float scaleSpeedSec = (1.f * speedScale * deltaTime);

		static bool	isInit = false;

		static Camera	  cam;
		static RenderMesh renderMesh;

		static const wchar_t*     control_name = L"none";
		static WeakPtr<Transform> control_tr;

		static WeakPtr<Transform> cam_tr;
		static WeakPtr<Transform> mesh_tr;

		static Mesh					  mesh;
		static std::vector<Material>  mats;
		static std::vector<Texture2D> texs;


		/********************************************************************************
		 *   초기화가 되지 않았다면, 초기화를 진행한다...
		 *******/
		if (isInit==false) 
		{
			isInit = true;

			/*---------------------------------------------------------
			 *   리소스를 로드한다....
			 ********/
			PmxImporter::StorageDescription storage_desc;
			storage_desc.OutMesh      = &mesh;
			storage_desc.OutMaterials = &mats;
			storage_desc.OutTextures  = &texs;

			PmxImporter::ImportResult pmx_ret;
			if ((pmx_ret = PmxImporter::Import(storage_desc, L"Resources/steve/steve.pmx")).Success == false) {
				throw "Pmx import failed!!";
			}

			mesh.RecalculateBoundingBox();
			

			/*-------------------------------------------------------
			 *   트랜스폼을 생성한다......
			 ********/
			mesh_tr    = Transform::CreateTransform();
			cam_tr     = Transform::CreateTransform();

			control_tr   = mesh_tr;
			control_name = L"renderMesh component";


			/*-----------------------------------------------------------
			 *   트랜스폼 컴포넌트들을 초기화하고, 적절한 트랜스폼에 부착한다..
			 ********/
			renderMesh.SetMesh(&mesh);
			
			for (uint32_t i = 0; i < mats.size(); i++) {
				Material& mat = mats[i];
				mat.Shaders.VertexShader   = Shader::VertexShader_MulFinalMat;
				mat.Shaders.FragmentShader = Shader::FragmentShader_Tex0Mapping;
				renderMesh.AddMaterial(&mats[i]);
			}

			mesh_tr->AttachTransformComponent(&renderMesh);
			cam_tr->AttachTransformComponent(&cam);


			/*-------------------------------------------------------
			 *   뷰포트를 초기화한다...
			 ********/
			GetViewPort().RenderCamera = &cam;
		}


		/********************************************************************************
		 *   선택한 트랜스폼을 제어한다....
		 *******/

		//랜더메시를 조작 대상으로 선택한다...
		if (input.WasPressedThisFrame(KeyCode::Num_0)) {
			control_tr = mesh_tr;
			control_name = L"renderMesh component";
		}

		//카메라를 조작 대상으로 선택한다...
		if (input.WasPressedThisFrame(KeyCode::Num_9)) {
			control_tr = cam_tr;
			control_name = L"camera component";
		}



		/********************************************************************************
		 *   선택한 트랜스폼을 제어한다....
		 *******/
		Transform* control_tr_raw = control_tr.Get();

		const Vector3 add_move = Vector3(
			input.GetAxis(KeyCode::Left, KeyCode::Right) * moveSpeedSec,
			input.GetAxis(KeyCode::Down, KeyCode::Up) * moveSpeedSec,
			input.GetAxis(KeyCode::NUMPAD_2, KeyCode::NUMPAD_8) * moveSpeedSec
		);

		const Quaternion add_rot = Quaternion::Euler(
			input.GetAxis(KeyCode::A, KeyCode::D) * rotSpeedSec,
			input.GetAxis(KeyCode::W, KeyCode::S) * rotSpeedSec,
			input.GetAxis(KeyCode::E, KeyCode::Q) * rotSpeedSec
		);

		const Vector3 add_scale = Vector3::One * input.GetAxis(KeyCode::F, KeyCode::R) * scaleSpeedSec;

		control_tr_raw->SetLocalPositionAndScaleAndRotation(
			(control_tr_raw->GetLocalPosition() + add_move),
			(control_tr_raw->GetLocalScale() + add_scale),
			(add_rot * control_tr_raw->GetLocalRotation())
		);



		/********************************************************************************
		 *   랜더메시를 뷰포트의 백버퍼에 그린다....
		 *******/
		renderer.DrawRenderMesh(renderMesh, GetViewPort());
		


		/********************************************************************************
		 *   디버그 출력...
		 *******/
		renderer.DrawTextField(w$(
			L"control transform name: ", control_name,
			L"\npos: ", control_tr_raw->GetWorldPosition(),
			L"\nrot: ", control_tr_raw->GetWorldRotation(),
			L"\nscale: ", control_tr_raw->GetWorldScale()),
			Vector2Int(0, 300), 
			GetViewPort()
		);

		#pragma endregion
	}

	void Example16_ShowBoneTransforms(const RenderMesh& renderMesh, WeakPtr<Transform>& control_tr)
	{
		#pragma region
		/********************************************************************
		 *   표시에 필요한 값들을 캐싱하고, 계산한다....
		 ********/
		Renderer&			renderer = GetRenderer();
		const InputManager& input    = GetInputManager();

		const float wHalf = 5.f;
		const float hHalf = 5.f;

		const Vector3 p1 = Vector3(-wHalf, -hHalf, 0.f);
		const Vector3 p2 = Vector3(wHalf, -hHalf, 0.f);
		const Vector3 p3 = Vector3(-wHalf, hHalf, 0.f);
		const Vector3 p4 = Vector3(wHalf, hHalf, 0.f);

		Mesh* mesh = renderMesh.GetMesh().Get();

		if (mesh==nullptr) {
			return;
		}




		/********************************************************************
		 *   표시에 필요한 값들을 캐싱하고, 계산한다....
		 ********/
		const uint32_t bone_count = mesh->Bones.size();

		for (uint32_t boneIdx = 0; boneIdx < bone_count; boneIdx++) {

			const Bone& bone   = mesh->Bones[boneIdx];
			Transform* bone_tr = renderMesh.GetBoneTransformAt(boneIdx).Get();

			if (bone_tr == nullptr) {
				continue;
			}

			const Vector3& bone_pos = bone_tr->GetWorldPosition();
			const Vector2  bone_ScreenPos = renderer.NDCToScreen(renderer.ClipToNDC(P * Vector4(bone_pos, 1.f)), GetViewPort());

			const Vector2 sp1 = (bone_ScreenPos + p1);
			const Vector2 sp2 = (bone_ScreenPos + p2);
			const Vector2 sp3 = (bone_ScreenPos + p3);
			const Vector2 sp4 = (bone_ScreenPos + p4);

			Color rectColor = Color::Yellow;

			//부모 본이 존재할 경우에만 그린다...
			if (bone.Parent_BoneIdx >= 0 && bone_tr->GetParent() != Transform::GetRoot())
			{
				const Bone& parent_bone = mesh.Bones[bone.Parent_BoneIdx];
				Transform* parent_bone_tr = bone_trs[bone.Parent_BoneIdx].Get();

				if (parent_bone_tr != nullptr) {
					const Vector3& parent_pos = parent_bone_tr->GetWorldPosition();
					const Vector2  parent_ScreenPos = renderer.NDCToScreen(renderer.ClipToNDC(P * Vector4(parent_pos, 1.f)), GetViewPort());

					const Vector2 bone2parent = (bone_ScreenPos - parent_ScreenPos);
					const Vector2 bone2parent_Dir = bone2parent.GetNormalized();
					const Vector2 bone2parent_right = Vector2(-bone2parent_Dir.y, bone2parent_Dir.x);

					const Vector2 arrow1_pos = parent_ScreenPos + (bone2parent_right * 5.f);
					const Vector2 arrow2_pos = parent_ScreenPos - (bone2parent_right * 5.f);

					Color boneColor = Color::Red;
					renderer.DrawLine(boneColor, bone_ScreenPos, parent_ScreenPos, GetViewPort());
					renderer.DrawLine(boneColor, bone_ScreenPos, arrow1_pos, GetViewPort());
					renderer.DrawLine(boneColor, bone_ScreenPos, arrow2_pos, GetViewPort());
					renderer.DrawLine(boneColor, parent_ScreenPos, arrow1_pos, GetViewPort());
					renderer.DrawLine(boneColor, parent_ScreenPos, arrow2_pos, GetViewPort());

					rectColor = Color::Blue;
				}
			}


			if (input.WasPressedThisFrame(KeyCode::Left_Mouse)) {
				const Vector2 mpos = input.GetMouseScreenPosition();

				bool checkUp = (mpos.x > sp1.x && mpos.x < sp2.x) && (mpos.y > sp1.y && mpos.y > sp2.y);
				bool checkDown = (mpos.x > sp3.x && mpos.x < sp4.x) && (mpos.y < sp3.y && mpos.y < sp4.y);

				//해당 본의 버튼을 클릭했는가?
				if (checkUp && checkDown) {
					selected_boneIdx = boneIdx;
				}
			}

			if (selected_boneIdx == boneIdx) {
				rectColor = Color::Green;
			}

			renderer.DrawLine(rectColor, sp1, sp2, GetViewPort());
			renderer.DrawLine(rectColor, sp1, sp3, GetViewPort());
			renderer.DrawLine(rectColor, sp2, sp4, GetViewPort());
			renderer.DrawLine(rectColor, sp3, sp4, GetViewPort());
		}


		#pragma endregion
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