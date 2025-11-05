#pragma once
#include <climits>
#include <vector>
#include "../MathModule/Math.h"
#include "../MathModule/Quaternion.h"
#include "../MathModule/Matrix.h"
#include "../EngineModule/UniqueableObject.h"

namespace hyunwoo {
	class Transform;
	class TransformComponent;
}



/*=====================================================================================================================================================
 *    Transform에게 부착될 수 있는 클래스들의 기반 클래스입니다...
 *************/
class hyunwoo::TransformComponent : public UniqueableObject
{

};















/*=====================================================================================================================================================
 *    이동/크기/회전에 대한 변환을 기록하는 구조체입니다. 자식을 가진 부모 Transform이 비균등할 경우, 자식 Transform이 비정상적으로 작동할 수 있습니다.
 *************/
class hyunwoo::Transform final : public hyunwoo::UniqueableObject
{
	//=====================================================================================================
	////////////////							 Define...								  /////////////////
	//=====================================================================================================
private:
	/*********************************************
	 *  각종 상수들의 정의입니다...
	 *****/
	constexpr static inline uint32_t m_localBuf_count  = 6; 
	constexpr static inline uint16_t m_root_idx        = 0;
	constexpr static inline uint16_t m_max_tr_count    = UINT16_MAX;
	constexpr static inline uint16_t m_max_child_count = 32767;


	/*****************************************
	 *   메모리를 압축하기 위한 구조체입니다...
	 ******/
	struct CompactPackage
	{
		uint16_t ChildCount   : 15 = 0;
		uint16_t IsDirty      : 1  = false;
	};


	/************************************************
	 *   Transform을 담는 freeList방식 컨테이너입니다..
	 ******/
	struct TransformArray final
	{
		Transform* Transforms;
		uint32_t   Capacity;
		uint32_t   Count;
	};




	//=====================================================================================================
	////////////////							 Fields...								  /////////////////
	//=====================================================================================================
private:
	static TransformArray	     m_transforms;
	static std::vector<uint16_t> m_freeList;

	Vector3 m_local_position = Vector3::Zero;
	Vector3	m_world_position = Vector3::Zero;

	Vector3 m_local_scale = Vector3::One;
	Vector3	m_world_scale = Vector3::One;

	Quaternion m_local_rotation  = Quaternion::Identity;
	Quaternion m_world_rotation  = Quaternion::Identity;

	uint16_t*				    m_child_list = m_localBuf;
	WeakPtr<TransformComponent> m_comp;


	uint16_t	   m_parent = m_root_idx;
	CompactPackage m_package;

	union {
		uint16_t m_localBuf[m_localBuf_count];
		uint32_t m_capacity = 0;
	};




	//===================================================================================================
	////////////////						 Private methods..						    /////////////////
	//===================================================================================================
private:
	Transform() = default;
	Transform(const Transform& prev)	 { operator=(prev); }
	Transform(Transform&& prev) noexcept { operator=(std::move(prev)); }
	virtual ~Transform();

	void operator=(const Transform& prev);
	void operator=(Transform&& prev) noexcept;



	//===================================================================================================
	////////////////						 Public methods..						    /////////////////
	//===================================================================================================
public:
	const Matrix4x4 GetTRS();


	/*****************************************
	 *   정적 메소드...
	 ******/
	static Transform* GetRoot();
	static Transform* CreateTransform();
	static void		  DestroyTransform(Transform* target);


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

	void AddChild(Transform* newChild);
	void RemoveChild(Transform* removeChild);
	void RemoveChildAt(uint32_t index);



	//===================================================================================================
	////////////////						 Private methods..						    /////////////////
	//===================================================================================================
private:
	void Clear();

	/*******************************************
	 *   인덱스 관련 메소드들...
	 ******/
	static inline uint16_t   GetIndex(Transform* idx);
	static inline Transform* GetRawptr(uint16_t idx);


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