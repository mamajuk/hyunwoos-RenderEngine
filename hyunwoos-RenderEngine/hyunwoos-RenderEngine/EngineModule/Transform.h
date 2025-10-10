#pragma once
#include "../MathModule/Math.h"
#include "../MathModule/Quaternion.h"
#include "../MathModule/Matrix.h"
#include "../EngineModule/UniqueableObject.h"

namespace hyunwoo {
	class Transform;
}


/*=====================================================================================================================================================
 *    이동/크기/회전에 대한 변환을 기록하는 구조체입니다...
 *************/
class hyunwoo::Transform : public UniqueableObject
{
private:
	//=====================================================================================================
	////////////////							 Fields...								  /////////////////
	//=====================================================================================================
	Vector3    m_local_position;
	Vector3	   m_world_position;

	Vector3    m_local_scale;
	Vector3	   m_world_scale;

	Quaternion m_local_rotation;
	Quaternion m_world_rotation;


	WeakPtr<Transform>  m_parent;
	WeakPtr<Transform>* m_child_list = m_localBuf;
	uint32_t		    m_childCount = 0;

	union {
		WeakPtr<Transform> m_localBuf[1];
		uint32_t		   m_capacity = 0;
	};




	//===================================================================================================
	////////////////						 Public methods..						    /////////////////
	//===================================================================================================
public:
	virtual ~Transform() override;
	const Matrix4x4 GetTRS() const;


	/*****************************************
	 *   위치 관련 메소드....
	 ******/
	const Vector3 GetLocalPosition() const;
	void		  SetLocalPosition(const Vector3& newPosition);

	const Vector3 GetWorldPosition() const;
	void		  SetWorldPosition(const Vector3& newPosition);


	/*****************************************
	 *   스케일 관련 메소드....
	 ******/
	const Vector3 GetLocalScale() const;
	void		  SetLocalScale(const Vector3& newScale);

	const Vector3 GetWorldScale() const;
	void		  SetWorldScale(const Vector3& newScale);


	/*****************************************
	 *   회전 관련 메소드....
	 ******/
	const Quaternion GetLocalRotation() const;
	void			 SetLocalRotation(const Quaternion& newRotation);

	const Quaternion GetWorldRotation() const;
	void			 SetWorldRotation(const Quaternion& newRotation);


	/*****************************************
	 *   계층구조 관련 메소드....
	 ******/
	WeakPtr<Transform> GetParent() const;
	void			   SetParent(WeakPtr<Transform> newParent);

	const uint32_t     GetChildCount() const;
	WeakPtr<Transform> GetChildAt(uint32_t index) const;

	void AddChild(Transform& newChild);
	void RemoveChild(Transform& removeChild);
	void RemoveChildAt(uint32_t index);






	//===================================================================================================
	////////////////						 Override methods..						    /////////////////
	//===================================================================================================
private:
	virtual void OnUnUniqued() override;





	//===================================================================================================
	////////////////						 Private methods..						    /////////////////
	//===================================================================================================
private:

};