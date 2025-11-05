#include "DisplayObject.h"

/*=======================================================================================================================================================
 *    인자로 주어진 메시를 랜더링 리소스로 설정합니다. 해당 메시가 본 정보를 가지고 있다면, 자동으로 본에 대한 Transform을 생성해 자식 Transform으로 추가합니다...
 ***********/
//void hyunwoo::DisplayObject::SetMesh(Mesh* mesh)
//{
//	m_mesh = mesh;
//	m_boneTransforms.clear();
//
//
//	/*****************************************************************
//	 *   해당 메시가 본을 가지고 있다면, 본 Transform을 생성한다... 
//	 *******/
//	const std::vector<Bone>& bone_list = mesh->Bones;
//	if (bone_list.size() > 0) 
//	{
//		//본 트랜스폼을 담을 버퍼를 확장한다....
//		m_boneTransforms.resize(bone_list.size());
//
//		for (uint32_t i = 0; i < bone_list.size(); i++) {
//			const Bone&	bone    = bone_list[i];
//			Transform&  bone_tr = m_boneTransforms[i];
//
//			/*------------------------------------
//			 *   본 트랜스폼의 로컬 데이터를
//			 *   바인딩 포즈로 초기화한다...
//			 *****/
//			bone_tr.SetLocalPositionAndScaleAndRotation(
//				bone.BindingPose.Position,
//				bone.BindingPose.Scale,
//				bone.BindingPose.Rotation
//			);
//
//
//			/*------------------------------------
//			 *   부모 본이 존재하는가? 맞다면 부모의 
//			 *   자식으로 추가한다..
//			 *****/
//			if (bone.Parent_BoneIdx >=0) {
//				bone_tr.SetParent(&m_boneTransforms[bone.Parent_BoneIdx]);
//			}
//
//
//			/*------------------------------------
//			 *   부모 본이 존재하지않는가? 맞다면 
//			 *   DisplayObject의 자식으로 추가한다..
//			 *****/
//			else {
//				m_transform.AddChild(bone_tr);
//			}
//		}
//	}
//
//}
