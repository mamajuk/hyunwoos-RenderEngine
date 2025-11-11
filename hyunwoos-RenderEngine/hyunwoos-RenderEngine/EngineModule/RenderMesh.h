#pragma once
#include "../RenderModule/Mesh.h"
#include "../RenderModule/Material.h"
#include "../EngineModule/Transform.h"
#include <vector>

namespace hyunwoo {
	class RenderMesh;
}


/*=============================================================================================================================================================================
 *    주어진 메시를 랜더링하는데 필요한 정보를 가진 TransformComponent입니다. 스켈레탈 메시라면 본들의 Transform을 별도로 소유합니다...
 *********/
class hyunwoo::RenderMesh : public TransformComponent
{
	//==========================================================================================
	////////////							Fields...								////////////
	//==========================================================================================
private:
	uint32_t						m_rootBoneCount = 0;
	WeakPtr<Mesh>					m_mesh;
	std::vector<WeakPtr<Material>>  m_materials;
	std::vector<WeakPtr<Transform>> m_boneTransforms;




	//===============================================================================================
	////////////						  Protected methods..						 ////////////////
	//===============================================================================================
protected:
	virtual void OnAttachment() override;
	virtual void OnDetachment() override;



	//===========================================================================================
	////////////						  Public methods..						 ////////////////
	//===========================================================================================
public:
	/*******************************************
	 *   본 트랜스폼을 얻어오는 메소드...
	 ******/
	uint32_t		   GetBoneTransformCount() const;
	WeakPtr<Transform> GetBoneTransformAt(uint32_t index) const;


	/*******************************************
	 *   메시를 얻어오거나, 설정하는 메소드...
	 ******/
	WeakPtr<Mesh> GetMesh() const;
	void		  SetMesh(Mesh* mesh);


	/*******************************************
	 *   머터리얼를 얻어오는 메소드...
	 ******/
	void			  AddMaterial(Material* new_mat);
	uint32_t		  GetMaterialCount() const;
	WeakPtr<Material> GetMaterialAt(uint32_t index) const;




	//===========================================================================================
	////////////						  Private methods..						 ////////////////
	//===========================================================================================
private:
	/*******************************************
	 *   본 트랜스폼을 생성하거나/파괴하는 메소드..
	 ******/
	void CreateBoneTransforms();
	void DestroyBoneTransforms();
};