#pragma once
#include <climits>
#include "../MathModule/Math.h"
#include "../MathModule/Quaternion.h"
#include "../MathModule/Matrix.h"

namespace hyunwoo {
	class Transform;
}


/*=====================================================================================================================================================
 *    이동/크기/회전에 대한 변환을 기록하는 구조체입니다. 자식을 가진 부모 Transform이 비균등할 경우, 자식 Transform이 비정상적으로 작동할 수 있습니다.
 *************/
class hyunwoo::Transform
{
	//=====================================================================================================
	////////////////							 Define...								  /////////////////
	//=====================================================================================================
private:
	/*********************************************
	 *  자식들의 포인터를 담는 로컬 버퍼의 크기를
	 *  나타내는 상수...
	 *****/
	constexpr static inline uint32_t m_localBuf_count = 2; 


	/*****************************************
	 *   메모리를 압축하기 위한 구조체입니다...
	 ******/
	struct CompactPackage
	{
		uint16_t ChildCount    : 15 = 0;
		uint16_t IsDirty       : 1  = false;
	};




	//=====================================================================================================
	////////////////							 Fields...								  /////////////////
	//=====================================================================================================
private:
	Vector3 m_local_position = Vector3::Zero;
	Vector3	m_world_position = Vector3::Zero;

	Vector3 m_local_scale = Vector3::One;
	Vector3	m_world_scale = Vector3::One;

	Quaternion m_local_rotation  = Quaternion::Identity;
	Quaternion m_world_rotation  = Quaternion::Identity;


	Transform*     m_parent     = nullptr;
	Transform**    m_child_list = m_localBuf;
	CompactPackage m_package;

	union {
		Transform* m_localBuf[m_localBuf_count];
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
	/*******************************************
	 *   월드/로컬 트랜스폼을 재계산하는 메소드들..
	 ******/
	void UpdateWorldTransform();
	void UpdateLocalTransform();


	/*******************************************
	 *   재계산이 필요하다는 isDirty플래그를
	 *   체크하거나, 처리하는 메소드들...
	 ******/
	void UpdateChildDirties();
	void UpdateDirtyParentWorldTransforms();
};