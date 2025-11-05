#pragma once
#include "../RenderModule/Mesh.h"
#include "../RenderModule/Material.h"
#include "../EngineModule/Transform.h"
#include "UniqueableObject.h"
#include <vector>

namespace hyunwoo {
	class DisplayObject;
}


/*=============================================================================================================================================================================
 *    화면에 주어진 메시에 대한 랜더링을 진행할 수 있는 객체입니다. 스켈레탈 메시라면 본들의 Transform을 별도로 소유합니다...
 *********/
class hyunwoo::DisplayObject : public UniqueableObject
{
//	//==========================================================================================
//	////////////							Fields...								////////////
//	//==========================================================================================
//private:
//	WeakPtr<Mesh>		   m_mesh;
//	std::vector<Material>  m_materials;
//
//	Transform			   m_transform;
//	std::vector<Transform> m_boneTransforms;
//
//
//
//	//===========================================================================================
//	////////////						  Public methods..						 ////////////////
//	//===========================================================================================
//public:
//	/***************************************
//	 *   메시에 대한 get/set 메소드들...
//	 *****/
//	WeakPtr<Mesh> GetMesh() const {
//		return m_mesh;
//	}
//
//	void SetMesh(Mesh* mesh);
//
//
//	/****************************************
//	 *   
//	 ******/

};