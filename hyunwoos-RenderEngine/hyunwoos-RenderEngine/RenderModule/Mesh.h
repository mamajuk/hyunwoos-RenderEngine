#pragma once
#include <vector>
#include "../MathModule/Quaternion.h"
#include "../GeometryModule/Geometry.h"
#include "../EngineModule/UniqueableObject.h"
#include "../UtilityModule/StringKey.h"

namespace hyunwoo {
	struct Vertex;
	struct Triangle;
	struct IndexedTriangle;
	struct SkinDeformWeight;
	struct SubMesh;
	struct Bone;
	struct Mesh;

	enum class SkinDeformType : uint8_t;
}


/*===================================================================================================================================
 *   본 스키닝 블랜딩 타입을 나타내는 열거형입니다...
 *=========*/
enum class hyunwoo::SkinDeformType : uint8_t
{
	BDEF, SDEF, QDEF
};














/*===================================================================================================================================
 *   오브젝트 위치와, UV좌표, 스키닝 애니메이션에 필요한 정보들로 구성된 버텍스가 정의된 구조체입니다...
 *=========*/
struct hyunwoo::Vertex final
{
	struct SkinDeformDescriptor
	{
		uint32_t	   Weight_StartIdx;
		uint32_t	   Weight_Count;
		SkinDeformType DeformType;
	};

	Vector3				 ObjPos;
	Vector2				 UvPos;
	SkinDeformDescriptor SkinDeformDesc;
};













/*===================================================================================================================================
 *   본 스키닝 애니메이션에서, 다른 본에 얼마나 영향을 받는지를 나타내는 구조체입니다...
 *=========*/
struct hyunwoo::SkinDeformWeight final
{
	float   Weight;
	int32_t BoneTransformIdx;
};













/*==================================================================================================================================
 *   버텍스 3개의 위치로 구성된 삼각형을 나타내는 구조체입니다...
 *=========*/
struct hyunwoo::IndexedTriangle final
{
	uint32_t Indices[3];
};












/*==================================================================================================================================
 *   정점 데이터 3개로 구성된 삼각형을 나타내는 구조체입니다...
 *=========*/
struct hyunwoo::Triangle final
{
	Vertex Vertices[3];
};










/*==================================================================================================================================
 *   해당 메시의 서브메시 정보를 담은 구조체입니다....
 *=========*/
struct hyunwoo::SubMesh
{
	uint32_t Triangle_Count;
};











/*===================================================================================================================================
 *    본의 바인딩 포즈와, 부모 본의 인덱스를 가지고 있는 구조체입니다....
 *=========*/
struct hyunwoo::Bone
{
	struct Pose
	{
		Vector3    LocalPosition = Vector3::Zero;
		Vector3    LocalScale    = Vector3::One;
		Quaternion LocalRotation = Quaternion::Identity;

		Vector3    WorldPosition = Vector3::Zero;
		Vector3    WorldScale    = Vector3::One;
		Quaternion WorldRotation = Quaternion::Identity;

		Matrix4x4 GetTRS()         const;
		Matrix4x4 GetTRS_Inverse() const;
	};

	int32_t    Parent_BoneIdx;
	WStringKey Name;
	Pose	   BindingPose;
};











 /*================================================================================================================================
  *   모델링 데이터를 담을 수 있는 구조체입니다...
  *=========*/
struct hyunwoo::Mesh final : public hyunwoo::UniqueableObject
{
	//=========================================================================================
	///////////							  Properties...							///////////////
	//=========================================================================================
	std::vector<Vertex>			  Vertices;
	std::vector<IndexedTriangle>  Triangles;
	std::vector<SubMesh>		  SubMeshs;
	std::vector<SkinDeformWeight> DeformWeights;
	std::vector<Bone>			  Bones;

	BoundingBox	   BoundBox;
	BoundingSphere BoundSphere;




	//=========================================================================================
	///////////							 Public methods..						///////////////
	//=========================================================================================
	void RecalculateBoundingSphere();
	void RecalculateBoundingBox();

	void CreateBoundingBoxMesh(Mesh& outMesh);
	void CreateBoundingSphereMesh(Mesh& outMesh);
};