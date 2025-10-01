#pragma once
#include <Windows.h>
#include <vector>
#include <functional>
#include "../MathModule/Vector.h"
#include "../MathModule/Color.h"
#include "../GeometryModule/Geometry.h"

namespace hyunwoo {
	struct Texture2D;
	struct Vertex;
	struct Triangle;
	struct Mesh;
	struct Shader;
	struct Material;
	struct Plane;
	struct DisplayObject;
	struct BoundingBox;
	struct BoundingSphere;
}



/*========================================================================================================
 *    2D 텍스쳐가 정의된 구조체입니다. 텍스쳐의 uv좌표계는 스크린좌표계를 따릅니다...
 *=========*/
struct hyunwoo::Texture2D final
{
	uint32_t				 Width  = 0;
	uint32_t				 Height = 0;
	std::vector<Color>		 Pixels;

	const Color GetPixel(const Vector2Int& pos) const;
};






/*===========================================================================================================
 *   오브젝트 위치와, UV좌표로 구성된 버텍스가 정의된 구조체입니다...
 *=========*/
struct hyunwoo::Vertex final
{
	Vector3 ObjPos;
	Vector2 UvPos;
};






/*===========================================================================================================
 *   버텍스 3개의 위치로 구성된 삼각형을 나타내는 구조체입니다...
 *=========*/
struct hyunwoo::Triangle final
{
	uint32_t indices[3];
};






/*===========================================================================================================
 *   모델링 데이터를 담을 수 있는 구조체입니다...
 *=========*/
struct hyunwoo::Mesh final
{
	struct SubMesh 
	{
		uint32_t Triangle_Count;
	};

	std::vector<Vertex>	  Vertices;
	std::vector<Triangle> Triangles;
	std::vector<SubMesh>  SubMeshs;
	BoundingBox			  BoundBox;
	BoundingSphere		  BoundSphere;

	void RecalculateBoundingSphere();
	void RecalculateBoundingBox();

	void CreateBoundingBoxMesh(Mesh& outMesh);
	void CreateBoundingSphereMesh(Mesh& outMesh);
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