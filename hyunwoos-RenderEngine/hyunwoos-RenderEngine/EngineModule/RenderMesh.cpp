#include "RenderMesh.h"


/*=======================================================================================================================================================
 *    해당 RenderMesh가 Transform에 부착되거나/떼어졌을 때 호출되는 메소드들입니다....
 ***********/
void hyunwoo::RenderMesh::OnAttachment()
{
	CreateBoneTransforms();
}

void hyunwoo::RenderMesh::OnDetachment()
{
	DestroyBoneTransforms();
}











/*=======================================================================================================================================================
 *    해당 RenderMesh가 랜더링 될 때 사용할 Bone들의 Transform개수를 얻습니다....
 ***********/
uint32_t hyunwoo::RenderMesh::GetBoneTransformCount() const
{
	return (m_boneTransforms.size() - m_rootBoneCount);
}











/*=======================================================================================================================================================
 *    해당 RenderMesh가 랜더링 될 때 사용할 Bone Transform을 얻습니다.....
 ***********/
hyunwoo::WeakPtr<hyunwoo::Transform> hyunwoo::RenderMesh::GetBoneTransformAt(uint32_t index) const
{
	if (index >= 0 && index < m_boneTransforms.size()) {
		return m_boneTransforms[index];
	}

	return WeakPtr<Transform>();
}










/*=======================================================================================================================================================
 *    해당 RenderMesh가 랜더링 될 때 사용할 Mesh를 설정하거나 얻습니다.....
 ***********/
hyunwoo::WeakPtr<hyunwoo::Mesh> hyunwoo::RenderMesh::GetMesh() const
{
	return m_mesh;
}

void hyunwoo::RenderMesh::SetMesh(Mesh* mesh)
{
	Mesh* prev_mesh = m_mesh.Get();
	m_mesh = mesh;

	/********************************************
	 *   설정한 메시가 유효하지 않다면, 생성한
	 *   본 트랜스폼을 파괴한다...
	 ******/
	if (mesh==nullptr) {
		DestroyBoneTransforms();
		return;
	}


	/********************************************
	 *   Transform에 부착된 상태에서 메시 리소스가
	 *   교체되었을 경우, 본 트랜스폼을 재구축 한다..
	 ******/
	Transform* attached_to = GetAttachedTransform().Get();
	if (prev_mesh!=mesh && attached_to!=nullptr) {
		DestroyBoneTransforms();
		CreateBoneTransforms();
	}
}










/*=======================================================================================================================================================
 *    해당 RenderMesh가 랜더링 될 때 사용할 Material의 참조를 추가합니다...
 ***********/
void hyunwoo::RenderMesh::AddMaterial(Material* new_mat)
{
	m_materials.push_back(new_mat);
}













/*=======================================================================================================================================================
 *    해당 RenderMesh가 랜더링 될 때 사용할 Material의 개수/참조를 얻습니다...
 ***********/
uint32_t hyunwoo::RenderMesh::GetMaterialCount() const
{
	return m_materials.size();
}

hyunwoo::WeakPtr<hyunwoo::Material> hyunwoo::RenderMesh::GetMaterialAt(uint32_t index) const
{
	if (index >=0 && index < m_materials.size()) {
		return m_materials[index];
	}

	return WeakPtr<Material>();
}











/*=======================================================================================================================================================
 *    해당 RenderMesh에 설정된 메시가 유효하고, 본을 가지고 있다면 본에 대한 Transform을 생성해 자식 Transform으로 추가합니다...
 ***********/
void hyunwoo::RenderMesh::CreateBoneTransforms()
{
	Mesh* mesh = m_mesh.Get();

	/*****************************************************************
	 *  설정된 메시가 유효하지 않다면, 함수를 종료한다....
	 ******/
	if (mesh==nullptr) {
		return;
	}


	/*****************************************************************
	 *   해당 메시가 본을 가지고 있다면, 본 Transform을 생성한다... 
	 *******/
	const std::vector<Bone>& bone_list = mesh->Bones;
	if (bone_list.size() > 0) 
	{
		/*-----------------------------------------
		 *   본 트랜스폼들을 생성한다...
		 ******/
		m_rootBoneCount = 0;
		m_boneTransforms.resize(bone_list.size());

		for (uint32_t i = 0; i < m_boneTransforms.size(); i++) {
			if (m_boneTransforms[i].Get()==nullptr) {
				Transform* raw_ptr = Transform::CreateTransform();
			    m_boneTransforms[i] = raw_ptr;
			}
		}


		/*-----------------------------------------
		 *   본 트랜스폼들을 초기화하고, 계층구조를
		 *   구축한다...
		 ******/
		Transform* attached_to = GetAttachedTransform().Get();

		for (uint32_t i = 0; i < bone_list.size(); i++) {
			const Bone&	bone      = bone_list[i];
			Transform*  bone_tr   = m_boneTransforms[i].Get();

			//본 트랜스폼의 월드 데이터를, 바인딩 포즈로 초기화한다...
			bone_tr->SetWorldPositionAndScaleAndRotation(
				bone.BindingPose.Position,
				bone.BindingPose.Scale,
				bone.BindingPose.Rotation
			);

			//부모 본이 존재하는가? 맞다면 부모의 자식으로 추가한다..
			if (bone.Parent_BoneIdx >=0) {
				bone_tr->SetParent(m_boneTransforms[bone.Parent_BoneIdx].Get(), true);
			}

			//부모 본이 존재하지 않는가? 맞다면 RenderMesh의 자식으로 추가한다..
			//해당 본은 이후 빠른 삭제를 위해서 본 트랜스폼 목록의 맨 뒤에 삽입된다...
			else {
				m_rootBoneCount++;
				m_boneTransforms.push_back(m_boneTransforms[i]);
				attached_to->AddChild(bone_tr, true);
			}
		}
	}

}









/*=======================================================================================================================================================
 *    본에 대한 Transform들을 모두 파괴합니다....
 ***********/
void hyunwoo::RenderMesh::DestroyBoneTransforms()
{
	/*******************************************************
	 *   본 트랜스폼들의 맨 좌측에 있는 m_rootBoneCount개의
	 *   루트 본들을 모두 파괴하여, 모든 본들을 연쇄파괴한다...
	 ******/
	const uint32_t start_idx = (m_boneTransforms.size() - m_rootBoneCount);
	const uint32_t goal_idx  = (m_boneTransforms.size() - 1);

	for (uint32_t i = start_idx; i <= goal_idx; i++) {
		Transform::DestroyTransform(m_boneTransforms[i].Get());
	}

	m_rootBoneCount = 0;
	m_boneTransforms.clear();
}
