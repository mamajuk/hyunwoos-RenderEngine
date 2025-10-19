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
	struct Material;
}



/*===================================================================================================================================
 *    2D 텍스쳐가 정의된 구조체입니다. 텍스쳐의 uv좌표계는 스크린좌표계를 따릅니다...
 *=========*/
struct hyunwoo::Texture2D final
{
	uint32_t		   Width  = 0;
	uint32_t		   Height = 0;
	std::vector<Color> Pixels;

	const Color GetPixel(const Vector2Int& pos) const;
};











/**==============================================================================================================
 *   쉐이더의 각 메소드들이 정의된 구조체입니다....
 *==========*/
struct hyunwoo::Shader final
{
	std::function<Vertex(const Vertex& in)>			  VertexShader;
	std::function<LinearColor(const LinearColor& in)> FragmentShader;
};










/*==============================================================================================================
 *   머터리얼을 나타내는 구조체입니다...
 *==========*/
struct hyunwoo::Material final
{
	bool	   TwoSide : 1;
	Shader	   Shader;
	Texture2D* MappedTexture = nullptr;
};