#pragma once
#include "../MathModule/Math.h"
#include "../MathModule/Quaternion.h"
#include "../MathModule/Matrix.h"

namespace hyunwoo {
	class Transform;
}


/*=====================================================================================================================================================
 *    이동/크기/회전에 대한 변환을 기록하는 구조체입니다...
 *************/
class hyunwoo::Transform
{
private:
	//=====================================================================================================
	////////////////							 Fields...								  /////////////////
	//=====================================================================================================
	bool m_isDirty = false;

	Vector3 m_local_position = Vector3::Zero;
	Vector3	m_world_position = Vector3::Zero;

	Vector3 m_local_scale = Vector3::One;
	Vector3	m_world_scale = Vector3::One;

	Quaternion m_local_rotation  = Quaternion::Identity;
	Quaternion m_world_rotation  = Quaternion::Identity;


	Transform*  m_parent     = nullptr;
	Transform** m_child_list = m_localBuf;
	uint32_t	m_childCount = 0;

	union {
		Transform* m_localBuf[1];
		uint32_t   m_capacity = 0;
	};




	//===================================================================================================
	////////////////						 Public methods..						    /////////////////
	//===================================================================================================
public:
	~Transform();
	const Matrix4x4 GetTRS();


	/*****************************************
	 *   위치 관련 메소드....
	 ******/
	const Vector3 GetLocalPosition();
	void		  SetLocalPosition(const Vector3& newPosition);

	const Vector3 GetWorldPosition();
	void		  SetWorldPosition(const Vector3& newPosition);


	/*****************************************
	 *   스케일 관련 메소드....
	 ******/
	const Vector3 GetLocalScale();
	void		  SetLocalScale(const Vector3& newScale);

	const Vector3 GetWorldScale();
	void		  SetWorldScale(const Vector3& newScale);


	/*****************************************
	 *   회전 관련 메소드....
	 ******/
	const Quaternion GetLocalRotation();
	void			 SetLocalRotation(const Quaternion& newRotation);

	const Quaternion GetWorldRotation();
	void			 SetWorldRotation(const Quaternion& newRotation);


	/*****************************************
	 *   동시 변환 메소드...
	 ******/
	void SetLocalPositionAndRotation(const Vector3& newPosition, const Quaternion& newRotation);
	void SetWorldPositionAndRotation(const Vector3& newPosition, const Quaternion& newRotation);
	
	void SetLocalPositionAndScaleAndRotation(const Vector3& newPosition, const Vector3& newScale, const Quaternion& newRotation);
	void SetWorldPositionAndScaleAndRotation(const Vector3& newPosition, const Vector3& newScale, const Quaternion& newRotation);


	/*****************************************
	 *   계층구조 관련 메소드....
	 ******/
	Transform* GetParent() const;
	void	   SetParent(Transform* newParent);

	uint32_t   GetChildCount() const;
	Transform* GetChildAt(uint32_t index) const;

	void AddChild(Transform& newChild);
	void RemoveChild(Transform& removeChild);
	void RemoveChildAt(uint32_t index);




	//===================================================================================================
	////////////////						 Private methods..						    /////////////////
	//===================================================================================================
private:
	void UpdateWorldTransform();
	void UpdateLocalTransform();

	void UpdateChildDirties();
	void UpdateDirtyParentWorldTransforms();
};