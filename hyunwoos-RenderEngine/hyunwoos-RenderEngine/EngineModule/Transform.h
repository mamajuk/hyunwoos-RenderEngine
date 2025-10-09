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
	Vector3    m_local_scale;
	Quaternion m_local_rotation;

	Vector3	   m_world_position;
	Vector3	   m_world_scale;
	Quaternion m_world_rotation;

	WeakPtr<Transform> m_parent     = nullptr;
	WeakPtr<Transform> m_childs     = nullptr;
	uint32_t		   m_childCount = 0;

	union {
		Transform* m_localBuf[1] = { 0, };
		uint32_t   m_capacity;
	};




	//===================================================================================================
	////////////////						 Public methods..						    /////////////////
	//===================================================================================================
public:
	const Vector3 GetLocalPosition() const;
	void		  SetLocalPosition(const Vector3& newPosition);

	const Vector3 GetWorldPosition() const;
	void		  SetWorldPosition(const Vector3& newPosition);



	const Vector3 GetLocalScale() const;
	void		  SetLocalScale(const Vector3& newScale);

	const Vector3 GetWorldScale() const;
	void		  SetWorldScale(const Vector3& newScale);



	const Quaternion GetLocalRotation() const;
	void			 SetLocalRotation(const Quaternion& newRotate);

	const Quaternion GetWorldRotation() const;
	void			 SetWorldRotation(const Quaternion& newRotate);


	WeakPtr<Transform> GetParent() const;

	const uint32_t     GetChildCount() const;
	WeakPtr<Transform> GetChildAt(uint32_t index) const;
};