#include "Transform.h"

/*=========================================================================================================================================
 *   정적 메소드들의 정의....
 *********/
hyunwoo::Transform::TransformArray hyunwoo::Transform::m_transforms = hyunwoo::Transform::TransformArray{ new Transform[10], 10, 1};
std::vector<uint16_t>              hyunwoo::Transform::m_freeList;













/*=========================================================================================================================================
 *   해당 트랜스폼을 초기화합니다....
 *********/
void hyunwoo::Transform::Clear()
{
    SetParent(GetRoot());

    /********************************************
     *  자식 목록을 순회해서, 관계를 끊는다...
     ******/
    for (uint32_t i = 0; i < m_package.ChildCount; i++) {
        Transform* child = GetRawptr(m_child_list[i]);
        DestroyTransform(child);
    }


    /********************************************
     *  동적할당된 자식 목록을 사용한다면,
     *  해당 버퍼를 삭제한다...
     ******/
    if (m_child_list != m_localBuf) 
    {
        delete[] m_child_list;
        m_child_list = m_localBuf;
    }

    m_package.ChildCount = 0;
}













/*=========================================================================================================================================
 *   인자로 주어진 Transform의 인덱스값을 얻어옵니다...
 *********/
uint16_t hyunwoo::Transform::GetIndex(Transform* target)
{
    return uint16_t(target - GetRoot());
}












/*=========================================================================================================================================
 *   인자로 주어진 인덱스에 위치한 Transform의 주소값을 얻어옵니다...
 *********/
hyunwoo::Transform* hyunwoo::Transform::GetRawptr(uint16_t idx)
{
    return (GetRoot() + idx);
}














/*=========================================================================================================================================
 *   해당 Transform의 복사와 이동 동작이 정의된 대입 연산자입니다....
 ***********/
void hyunwoo::Transform::operator=(const Transform& prev)
{
    UniqueableObject::operator=(prev);
}

void hyunwoo::Transform::operator=(Transform&& prev) noexcept
{
    /**********************************************
     *   동일한 객체에 대한 이동은 허용되지 않는다..
     *******/
    if (this == &prev) {
        return;
    }

    //해당 Transform을 초기화한다..
    Clear();


    /**********************************************
     *   월드/로컬 데이터를 이동한다...
     ******/
    m_local_position = prev.m_local_position;
    m_world_position = prev.m_world_position;

    m_local_scale = prev.m_local_scale;
    m_world_scale = prev.m_world_scale;

    m_local_rotation = prev.m_local_rotation;
    m_world_rotation = prev.m_world_rotation;


    /**********************************************
     *   부모 데이터와, 자식 개수를 이동한다...
     ******/
    m_parent      = prev.m_parent;
    prev.m_parent = m_root_idx;

    m_package = prev.m_package;
    prev.m_package.ChildCount = 0;


    /***********************************************
     *  자식 리스트를 이동한다....
     *****/

    //prev가 동적할당된 버퍼를 사용하는가?
    if (prev.m_child_list != prev.m_localBuf) {
        m_child_list = prev.m_child_list;
        prev.m_child_list = prev.m_localBuf;
    }

    //prev가 로컬버퍼를 사용중이였는가?
    else memcpy(m_localBuf, prev.m_localBuf, sizeof(WeakPtr<Transform>) * (m_package.ChildCount));

    UniqueableObject::operator=(std::move(prev));
}











/*=========================================================================================================================================
 *   해당 Transform이 파괴되기 전, 정리를 진행합니다...
 ***********/
hyunwoo::Transform::~Transform()
{
    Clear();
}














/*=========================================================================================================================================
 *   로컬 위치를 얻거나/설정합니다..... 
 ***********/
const hyunwoo::Vector3 hyunwoo::Transform::GetLocalPosition()
{
    return m_local_position;
}

void hyunwoo::Transform::SetLocalPosition(const Vector3& newPosition)
{
    if (m_package.IsDirty) {
        UpdateDirtyParentWorldTransforms();
    }

    m_local_position = newPosition;
    UpdateWorldTransform();
    UpdateChildDirties();
}














/*=========================================================================================================================================
 *   월드 위치를 얻거나/설정합니다.....
 ***********/
const hyunwoo::Vector3 hyunwoo::Transform::GetWorldPosition()
{
    if (m_package.IsDirty) {
        UpdateDirtyParentWorldTransforms();
        UpdateWorldTransform();
    }

    return m_world_position;
}

void hyunwoo::Transform::SetWorldPosition(const Vector3& newPosition)
{
    if (m_package.IsDirty) {
        UpdateDirtyParentWorldTransforms();
    }

    m_world_position = newPosition;
    UpdateLocalTransform();
    UpdateChildDirties();
}














/*=========================================================================================================================================
 *   로컬 스케일을 얻거나/설정합니다.....
 ***********/
const hyunwoo::Vector3 hyunwoo::Transform::GetLocalScale()
{
    return m_local_scale;
}

void hyunwoo::Transform::SetLocalScale(const Vector3& newScale)
{
    if (m_package.IsDirty) {
        UpdateDirtyParentWorldTransforms();
    }

    m_local_scale     = newScale;
    UpdateWorldTransform();
    UpdateChildDirties();
}










/*=========================================================================================================================================
 *   월드 스케일을 얻거나/설정합니다.....
 ***********/
const hyunwoo::Vector3 hyunwoo::Transform::GetWorldScale()
{
    if (m_package.IsDirty) {
        UpdateDirtyParentWorldTransforms();
        UpdateWorldTransform();
    }

    return m_world_scale;
}

void hyunwoo::Transform::SetWorldScale(const Vector3& newScale)
{
    if (m_package.IsDirty) {
        UpdateDirtyParentWorldTransforms();
    }

    m_world_scale = newScale;
    UpdateLocalTransform();
    UpdateChildDirties();
}













/*=========================================================================================================================================
 *   로컬 회전량을 얻거나/설정합니다.....
 ***********/
const hyunwoo::Quaternion hyunwoo::Transform::GetLocalRotation()
{
    return m_local_rotation;
}

void hyunwoo::Transform::SetLocalRotation(const Quaternion& newRotation)
{
    if (m_package.IsDirty) {
        UpdateDirtyParentWorldTransforms();
    }

    m_local_rotation  = newRotation;
    UpdateWorldTransform();
    UpdateChildDirties();
}













/*=========================================================================================================================================
 *   월드 회전량을 얻거나/설정합니다.....
 ***********/
const hyunwoo::Quaternion hyunwoo::Transform::GetWorldRotation()
{
    if (m_package.IsDirty) {
        UpdateDirtyParentWorldTransforms();
        UpdateWorldTransform();
    }

    return m_world_rotation;
}

void hyunwoo::Transform::SetWorldRotation(const Quaternion& newRotation)
{
    if (m_package.IsDirty) {
        UpdateDirtyParentWorldTransforms();
    }

    m_world_rotation = newRotation;
    UpdateLocalTransform();
    UpdateChildDirties();
}













/*=========================================================================================================================================
 *   여러 변환을 동시에 적용한 후, 해당 Transform을 재계산하는 메소드들....
 ***********/
void hyunwoo::Transform::SetLocalPositionAndRotation(const Vector3& newPosition, const Quaternion& newRotation)
{
    if (m_package.IsDirty) {
        UpdateDirtyParentWorldTransforms();
    }

    m_local_position = newPosition;
    m_local_rotation = newRotation;
    UpdateWorldTransform();
    UpdateChildDirties();
}

void hyunwoo::Transform::SetWorldPositionAndRotation(const Vector3& newPosition, const Quaternion& newRotation)
{
    if (m_package.IsDirty) {
        UpdateDirtyParentWorldTransforms();
    }

    m_world_position = newPosition;
    m_world_rotation = newRotation;
    UpdateLocalTransform();
    UpdateChildDirties();
}



void hyunwoo::Transform::SetLocalPositionAndScaleAndRotation(const Vector3& newPosition, const Vector3& newScale, const Quaternion& newRotation)
{
    if (m_package.IsDirty) {
        UpdateDirtyParentWorldTransforms();
    }

    m_local_position  = newPosition;
    m_local_scale     = newScale;
    m_local_rotation  = newRotation;
    UpdateWorldTransform();
    UpdateChildDirties();
}

void hyunwoo::Transform::SetWorldPositionAndScaleAndRotation(const Vector3& newPosition, const Vector3& newScale, const Quaternion& newRotation)
{
    if (m_package.IsDirty) {
        UpdateDirtyParentWorldTransforms();
    }

    m_world_position = newPosition;
    m_world_scale    = newScale;
    m_world_rotation = newRotation;
    UpdateLocalTransform();
    UpdateChildDirties();
}



















/*=========================================================================================================================================
 *   부모 Transform을 설정하거나/주소값을 얻습니다..
 ***********/
hyunwoo::Transform* hyunwoo::Transform::GetParent() const
{
    return GetRawptr(m_parent);
}

void hyunwoo::Transform::SetParent(Transform* newParent)
{
    /*********************************************************
     *   기존 부모가 설정되어 있었다면, 부모->자식관계를 해제한다..
     *******/
    if (m_parent != m_root_idx) {
        Transform* parent_ptr = GetRawptr(m_parent);
        parent_ptr->RemoveChild(this);
    }


    /***********************************************************
     *  새로운 부모가 유효하다면, 자신을 자식으로 추가한다...   
     ********/
    if (newParent!=nullptr) {
        newParent->AddChild(this);
    }
}











/*=========================================================================================================================================
 *    자식 Transform의 개수와, 특정 인덱스에 있는 자식 Transform의 약참조를 얻습니다....
 ***********/
uint32_t hyunwoo::Transform::GetChildCount() const
{
    return m_package.ChildCount;
}

hyunwoo::Transform* hyunwoo::Transform::GetChildAt(uint32_t index) const
{
    //유효한 인덱스 범위를 벗어났다면, nullptr을 반환한다...
    if (index < 0 || index >= m_package.ChildCount) {
        return nullptr;
    }

    return GetRawptr(m_child_list[index]);
}














/*=========================================================================================================================================
 *    새로운 자식 Transform을 추가합니다....
 ***********/
void hyunwoo::Transform::AddChild(Transform* newChild)
{
    /**********************************************************
     *  이미 추가되어 있는 자식이거나, 추가될 자식이 자기 자신 또는
     *  루트 트랜스폼이거나, 자식 개수 한도를 초과했다면, 
     *  함수를 종료한다....
     *******/
    const uint16_t my_idx = GetIndex(this);
    if (newChild==nullptr || newChild->m_parent==my_idx || newChild == this || newChild == GetRoot() || m_package.ChildCount >= m_max_child_count) {
        return;
    }



    /********************************************************
     *  추가할 자식이 기존 부모를 가지고 있다면 해제한다...
     *******/
    if (newChild->m_parent != m_root_idx) {
        Transform* newChild_parentPtr = GetRawptr(newChild->m_parent);
        newChild_parentPtr->RemoveChild(newChild);
    }



    /***********************************************************
     *  현재 로컬 버퍼를 사용하고 있는 경우...
     *******/
    if (m_child_list==m_localBuf) 
    {
        /*---------------------------------------
         *  자식개수가 로컬 버퍼를 초과한 경우,
         *  동적할당된 버퍼로 교체한다...
         *****/
        if ((m_package.ChildCount+1) > m_localBuf_count) {
            uint16_t* new_list = new uint16_t[(m_localBuf_count+1)]();
            memcpy(new_list, m_localBuf, sizeof(uint16_t) * m_localBuf_count);
            m_child_list = new_list;
            m_capacity   = (m_localBuf_count + 1);
        }
    }


    
    /************************************************************
     *  동적 버퍼의 공간이 부족하면, 기존 크기의 배인 버퍼를 새로 
     *  할당하고, 기존 리소스를 옮긴다....
     *******/
    else if (m_capacity == m_package.ChildCount)
    {
        uint16_t* prev_list = m_child_list;

        m_child_list = new uint16_t[(m_capacity *= 2)]();
        memcpy(m_child_list, prev_list, (sizeof(uint16_t) * m_package.ChildCount));
        
        delete[] prev_list;
    }


    /*************************************************
     *  해당 객체를 자식으로 추가한다....
     *******/
    m_child_list[m_package.ChildCount++] = GetIndex(newChild);
    newChild->m_parent                    = GetIndex(this);
    newChild->UpdateLocalTransform();
}












/*=========================================================================================================================================
 *    특정 자식 Transform을 제거합니다....
 ***********/
void hyunwoo::Transform::RemoveChild(Transform* removeChild)
{
    /*************************************************
     *  자식 목록을 순회해서, 자식 객체라면 제거한다...
     ******/
    if (removeChild==nullptr) {
        return;
    }

    const uint32_t remove_child_idx = GetIndex(removeChild);
    for (uint32_t i = 0; i < m_package.ChildCount; i++)
    {
        //제거할 대상과 동일한 객체를 제거한다...
        if (m_child_list[i] == remove_child_idx) {
            removeChild->m_parent = m_root_idx;
            removeChild->UpdateLocalTransform();
            m_child_list[i] = m_child_list[--m_package.ChildCount];
            return;
        }
    }
}

void hyunwoo::Transform::RemoveChildAt(uint32_t index)
{
    /**********************************************
     *  유효하지 않은 인덱스라면, 함수를 종료한다...
     *****/
    if (index < 0 || index >= m_package.ChildCount) {
        return;
    }

    Transform* child = GetRawptr(m_child_list[index]);
    child->m_parent  = m_root_idx;
    child->UpdateLocalTransform();
    m_child_list[index] = m_child_list[--m_package.ChildCount];
}
















/*=============================================================================================================================================
 *   해당 Transform의 모델링 행렬 (T*R*S)를 얻습니다....
 ***********/
const hyunwoo::Matrix4x4 hyunwoo::Transform::GetTRS()
{
    if (m_package.IsDirty) {
        UpdateDirtyParentWorldTransforms();
        UpdateWorldTransform();
    }

    const Matrix4x4 T = Matrix4x4(
        Vector4::BasisX,
        Vector4::BasisY,
        Vector4::BasisZ,
        Vector4(m_world_position, 1.f)
    );

    const Matrix4x4 R = m_world_rotation.GetRotateMatrix();

    const Matrix4x4 S = Matrix4x4(
        (Vector4::BasisX * m_world_scale.x),
        (Vector4::BasisY * m_world_scale.y),
        (Vector4::BasisZ * m_world_scale.z),
        Vector4::BasisW
    );

    return (T*R*S);
}














/*=============================================================================================================================================
 *   루트 트랜스폼의 주소값을 얻습니다....
 ***********/
hyunwoo::Transform* hyunwoo::Transform::GetRoot()
{
    return &m_transforms.Transforms[0];
}

















/*=============================================================================================================================================
 *   Transform을 생성하거나, 삭제합니다....
 ***********/
hyunwoo::Transform* hyunwoo::Transform::CreateTransform()
{
    /**********************************************
     *   Transform 개수가 한도에 도달했다면 nullptr을
     *   반환하고 함수를 종료한다...
     *****/
    if (m_transforms.Count > m_max_tr_count) {
        return nullptr;
    }


    /**************************************************
     *   사용하지 않는 Transform이 있다면, 해당 공간에
     *   새로운 Transform을 placement new로 할당한다..
     *****/
    if (m_freeList.size() > 0)
    {
        const uint16_t free_idx = m_freeList[0];

        Transform* new_tr = new(&m_transforms.Transforms[free_idx]) Transform();
        new_tr->SetParent(GetRoot());
        m_freeList[0] = m_freeList[m_freeList.size() - 1];
        m_freeList.pop_back();

        return new_tr;
    }


    /********************************************************
      *   여유 Transform 공간이 없다면, 새로운 Transform을
      *   할당한다...
      *****/
    else
    {
        /*--------------------------------------------
         *   현재 버퍼의 메모리가, 새 Transform을
         *   담기에 부족하다면 배로 할당하고 기존 내용을
         *   이동시킨다...
         ******/
        if (m_transforms.Capacity < (m_transforms.Count+1)) {
            Transform* prev_buf     = m_transforms.Transforms;
            m_transforms.Transforms = new Transform[m_transforms.Capacity*=2];
            
            //기존 Transform들을 새 버퍼로 이동시킨다...
            for (uint32_t i = 0; i < m_transforms.Count; i++) {
                m_transforms.Transforms[i] = std::move(prev_buf[i]);
            }

            delete[] prev_buf;
        }

        Transform* new_tr = new(&m_transforms.Transforms[m_transforms.Count++]) Transform();
        new_tr->SetParent(GetRoot());
        return new_tr;
    }

    return nullptr;
}

void hyunwoo::Transform::DestroyTransform(Transform* target)
{
    target->~Transform();
    m_freeList.push_back(GetIndex(target));
}














/*==============================================================================================================================================
 *   로컬 트랜스폼으로부터 월드 트랜스폼을 구해 설정합니다...
 *************/
void hyunwoo::Transform::UpdateWorldTransform()
{
    /*-------------------------------------------
     *  부모가 없다면, 로컬과 똑같은 값을 가진다...
     *****/
    if (m_parent==m_root_idx) {
        m_world_position  = m_local_position;
        m_world_rotation  = m_local_rotation;
        m_world_scale     = m_local_scale;
        m_package.IsDirty = false;
        return;
    }

    Transform* parent_ptr = GetRawptr(m_parent);
    m_world_position  = ((parent_ptr->m_world_rotation * m_local_position) * parent_ptr->m_world_scale) + parent_ptr->m_world_position;
    m_world_rotation  = (parent_ptr->m_world_rotation * m_local_rotation);
    m_world_scale     = (parent_ptr->m_world_scale * m_local_scale);
    m_package.IsDirty = false;
}

















/*==============================================================================================================================================
 *   월드 트랜스폼으로부터 로컬 트랜스폼을 구해 설정합니다...
 *************/
void hyunwoo::Transform::UpdateLocalTransform()
{
    /*-------------------------------------------
     *  부모가 없다면, 월드와 똑같은 값을 가진다...
     *****/
    if (m_parent==m_root_idx) {
        m_local_position  = m_world_position;
        m_local_rotation  = m_world_rotation;
        m_local_scale     = m_world_scale;
        m_package.IsDirty = false;
        return;
    }

    Transform* parent_ptr       = GetRawptr(m_parent);
    const float parentScaleDiv  = (1.f / parent_ptr->m_world_scale.x);
    Quaternion  parentConjugate = parent_ptr->m_world_rotation.GetConjugate();

    m_local_position  = parentConjugate * ((m_world_position - parent_ptr->m_world_position) * parentScaleDiv);
    m_local_rotation  = (parentConjugate * m_world_rotation);
    m_local_scale     = (m_world_scale * parentScaleDiv);
    m_package.IsDirty = false;
}















/*=============================================================================================================================================
 *   해당 Transform의 모든 하위 Transform들이 재계산할 필요가있음을 나타내는 isDirty 플래그를 설정합니다....
 ***********/
void hyunwoo::Transform::UpdateChildDirties()
{
    for (uint32_t i = 0; i < m_package.ChildCount; i++) {
        Transform* childTr = GetRawptr(m_child_list[i]);

        /*****************************************************
         *   이미 Dirty상태라면, 그 자식객체들도 모두 dirty
         *   상태일 것이기 때문에, 무시한다...
         *****/
        if (childTr->m_package.IsDirty == false) {
            childTr->m_package.IsDirty = true;
            childTr->UpdateChildDirties();
        }
    }
}

















/*=============================================================================================================================================
 *   Transform 재계산이 필요한 부모들을 거슬러올라가서 차례대로 월드 Transform의 재계산을 적용한다....
 ***********/
void hyunwoo::Transform::UpdateDirtyParentWorldTransforms()
{
    /**************************************************************
     *   해당 Transform의 부모가 재계산이 필요한가?
     ********/
    Transform* parent_ptr = GetRawptr(m_parent);

    if (parent_ptr->m_package.IsDirty) {
        parent_ptr->UpdateDirtyParentWorldTransforms();
        parent_ptr->UpdateWorldTransform();
        parent_ptr->m_package.IsDirty = false;
    }
}
