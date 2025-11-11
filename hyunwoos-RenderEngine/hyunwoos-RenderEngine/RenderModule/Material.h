#pragma once
#include <Windows.h>
#include <vector>
#include <functional>
#include "../MathModule/Vector.h"
#include "../MathModule/Color.h"
#include "../GeometryModule/Geometry.h"
#include "../RenderModule/Mesh.h"

namespace hyunwoo {
	struct Texture2D;
	struct Shader;
	struct MaterialProperties;
	struct Material;
}



/*====================================================================================================================================================================
 *    2D 텍스쳐가 정의된 구조체입니다. 텍스쳐의 uv좌표계는 스크린좌표계를 따릅니다...
 *=========*/
struct hyunwoo::Texture2D final : public UniqueableObject
{
	uint32_t		   Width  = 0;
	uint32_t		   Height = 0;
	std::vector<Color> Pixels;

	const Color GetPixel(const Vector2& uvPos) const;
};













/**======================================================================================================================================================================
 *   쉐이더의 각 메소드들이 정의된 구조체입니다....
 *==========*/
struct hyunwoo::Shader final
{
	//===================================================================================
	///////////						     Defines...							 ////////////
	//===================================================================================
public:
	/***************************************
	 *   쉐이더 함수들의 별칭....
	 *****/
	using VertexShaderFunc   = hyunwoo::Vector4(const hyunwoo::Vertex& inVertex, const Matrix4x4& inFinalMatrix);
	using FragmentShaderFunc = hyunwoo::Color(const hyunwoo::Vector2& inUv, const hyunwoo::Vector3& inNormal, const hyunwoo::Texture2D& inTex);



	//===========================================================================================
	///////////						     Public methods...							 ////////////
	//===========================================================================================
public:
	/************************************
	 *   기본 제공 버텍스 쉐이더 메소드...
	 ******/
	static Vector4 VertexShader_MulFinalMat(const Vertex& inVertex, const Matrix4x4& inFinalMat);


	 /************************************
	  *   기본 제공 프래그먼트 쉐이더 메소드...
	  ******/
	static Color FragmentShader_InvalidTex(const Vector2& inUv, const hyunwoo::Vector3& inNormal, const hyunwoo::Texture2D& inTex);
	static Color FragmentShader_Tex0Mapping(const Vector2& inUv, const hyunwoo::Vector3& inNormal, const hyunwoo::Texture2D& inTex);


	//=======================================================================================
	///////////						     Properties...							 ////////////
	//=======================================================================================
	VertexShaderFunc*	VertexShader   = nullptr;
	FragmentShaderFunc* FragmentShader = nullptr;
};










/*========================================================================================================================================================================
 *   머터리얼을 나타내는 구조체입니다...
 *==========*/
struct hyunwoo::Material final : public UniqueableObject
{
	Shader			   Shaders;
	WeakPtr<Texture2D> MappedTexture;
};