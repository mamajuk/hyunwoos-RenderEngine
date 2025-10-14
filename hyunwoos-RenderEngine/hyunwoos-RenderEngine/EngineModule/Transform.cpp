#include "Transform.h"

/*=========================================================================================================================================
 *   해당 Transform이 파괴되기 전, 부모 Transform이 유효하다면 부모의 자식으로부터 자신을 제거합니다...
 ***********/
hyunwoo::Transform::~Transform()
{
    SetParent(nullptr);

    /********************************************
     *  동적할당된 자식 목록을 제거한다....
     ******/
    if (m_child_list!=m_localBuf) {
        delete[] m_child_list;
        m_child_list = m_localBuf;
    }

    m_childCount = 0;
}














/*=========================================================================================================================================
 *   로컬 위치를 얻거나/설정합니다..... 
 ***********/
const hyunwoo::Vector3 hyunwoo::Transform::GetLocalPosition()
{
    if (m_isDirty) {
        UpdateDirtyParentWorldTransforms();
        UpdateWorldTransform();
    }

    return m_local_position;
}

void hyunwoo::Transform::SetLocalPosition(const Vector3& newPosition)
{
    if (m_isDirty) {
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
    if (m_isDirty) {
        UpdateDirtyParentWorldTransforms();
        UpdateWorldTransform();
    }

    return m_world_position;
}

void hyunwoo::Transform::SetWorldPosition(const Vector3& newPosition)
{
    if (m_isDirty) {
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
    if (m_isDirty) {
        UpdateDirtyParentWorldTransforms();
        UpdateWorldTransform();
    }

    return m_local_scale;
}

void hyunwoo::Transform::SetLocalScale(const Vector3& newScale)
{
    if (m_isDirty) {
        UpdateDirtyParentWorldTransforms();
    }

    m_local_scale = newScale;
    UpdateWorldTransform();
    UpdateChildDirties();
}










/*=========================================================================================================================================
 *   월드 스케일을 얻거나/설정합니다.....
 ***********/
const hyunwoo::Vector3 hyunwoo::Transform::GetWorldScale()
{
    if (m_isDirty) {
        UpdateDirtyParentWorldTransforms();
        UpdateWorldTransform();
    }

    return m_world_scale;
}

void hyunwoo::Transform::SetWorldScale(const Vector3& newScale)
{
    if (m_isDirty) {
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
    if (m_isDirty) {
        UpdateDirtyParentWorldTransforms();
        UpdateWorldTransform();
    }

    return m_local_rotation;
}

void hyunwoo::Transform::SetLocalRotation(const Quaternion& newRotation)
{
    if (m_isDirty) {
        UpdateDirtyParentWorldTransforms();
    }

    m_local_rotation = newRotation;
    UpdateWorldTransform();
    UpdateChildDirties();
}













/*=========================================================================================================================================
 *   월드 회전량을 얻거나/설정합니다.....
 ***********/
const hyunwoo::Quaternion hyunwoo::Transform::GetWorldRotation()
{
    if (m_isDirty) {
        UpdateDirtyParentWorldTransforms();
        UpdateWorldTransform();
    }

    return m_world_rotation;
}

void hyunwoo::Transform::SetWorldRotation(const Quaternion& newRotation)
{
    if (m_isDirty) {
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
    if (m_isDirty) {
        UpdateDirtyParentWorldTransforms();
    }

    m_local_position = newPosition;
    m_local_rotation = newRotation;
    UpdateWorldTransform();
    UpdateChildDirties();
}

void hyunwoo::Transform::SetWorldPositionAndRotation(const Vector3& newPosition, const Quaternion& newRotation)
{
    if (m_isDirty) {
        UpdateDirtyParentWorldTransforms();
    }

    m_world_position = newPosition;
    m_world_rotation = newRotation;
    UpdateLocalTransform();
    UpdateChildDirties();
}



void hyunwoo::Transform::SetLocalPositionAndScaleAndRotation(const Vector3& newPosition, const Vector3& newScale, const Quaternion& newRotation)
{
    if (m_isDirty) {
        UpdateDirtyParentWorldTransforms();
    }

    m_local_position = newPosition;
    m_local_scale    = newScale;
    m_local_rotation = newRotation;
    UpdateWorldTransform();
    UpdateChildDirties();
}

void hyunwoo::Transform::SetWorldPositionAndScaleAndRotation(const Vector3& newPosition, const Vector3& newScale, const Quaternion& newRotation)
{
    if (m_isDirty) {
        UpdateDirtyParentWorldTransforms();
    }

    m_world_position = newPosition;
    m_world_scale    = newScale;
    m_world_rotation = newRotation;
    UpdateLocalTransform();
    UpdateChildDirties();
}



















/*=========================================================================================================================================
 *   부모 Transform을 설정하거나/약참조를 얻습니다..
 ***********/
hyunwoo::Transform* hyunwoo::Transform::GetParent() const
{
    return m_parent;
}

void hyunwoo::Transform::SetParent(Transform* newParent)
{
    /*********************************************************
     *   기존 부모가 설정되어 있었다면, 부모->자식관계를 해제한다..
     *******/
    if (m_parent!=nullptr) {
         m_parent->RemoveChild(*this);
    }


    /***********************************************************
     *  새로운 부모가 유효하다면, 자신을 자식으로 추가한다...   
     ********/
    if (newParent!=nullptr) {
        newParent->AddChild(*this);
    }
}











/*=========================================================================================================================================
 *    자식 Transform의 개수와, 특정 인덱스에 있는 자식 Transform의 약참조를 얻습니다....
 ***********/
uint32_t hyunwoo::Transform::GetChildCount() const
{
    return m_childCount;
}

hyunwoo::Transform* hyunwoo::Transform::GetChildAt(uint32_t index) const
{
    //유효한 인덱스 범위를 벗어났다면, nullptr을 반환한다...
    if (index < 0 || index >= m_childCount) {
        return nullptr;
    }

    return m_child_list[index];
}














/*=========================================================================================================================================
 *    새로운 자식 Transform을 추가합니다....
 ***********/
void hyunwoo::Transform::AddChild(Transform& newChild)
{
    /********************************************************
     *  이미 추가되어 있는 자식이거나, 추가될 자식이 자기 자신이면
     *  함수를 종료한다.....
     *******/
    if (newChild.m_parent==this || &newChild==this) {
        return;
    }


    /***********************************************************
     *  자식 개수가 1개라면 로컬버퍼를, 2개 이상부터는 동적 버퍼를
     *  사용한다....
     *******/
    if (m_child_list==m_localBuf) 
    {
        /*---------------------------------------
         *  자식개수가 로컬 버퍼를 초과한 경우,
         *  동적할당된 버퍼로 교체한다...
         *****/
        if ((m_childCount+1) > 1) {
            m_child_list    = new Transform*[3]();
            m_child_list[0] = m_localBuf[0];
            m_capacity      = 3;
        }
    }


    
    /************************************************************
     *  동적 버퍼의 공간이 부족하면, 기존 크기의 배인 버퍼를 새로 
     *  할당하고, 기존 리소스를 옮긴다....
     *******/
    else if (m_capacity < (m_childCount + 1))
    {
        Transform** prev_list = m_child_list;

        m_child_list = new Transform*[(m_capacity *= 2)]();
        memcpy((void*)m_child_list, (void*)prev_list, (sizeof(Transform*) * m_childCount));
        
        delete[] prev_list;
    }


    /*************************************************
     *  해당 객체를 자식으로 추가한다....
     *******/
    m_child_list[m_childCount++] = &newChild;
    newChild.m_parent            = this;
    newChild.UpdateLocalTransform();
}












/*=========================================================================================================================================
 *    특정 자식 Transform을 제거합니다....
 ***********/
void hyunwoo::Transform::RemoveChild(Transform& removeChild)
{
    /*************************************************
     *  자식 목록을 순회해서, 자식 객체라면 제거한다...
     ******/
    for (uint32_t i = 0; i < m_childCount; i++)
    {
        //제거할 대상과 동일한 객체를 제거한다...
        if (m_child_list[i]==&removeChild) {
            removeChild.m_parent = nullptr;
            removeChild.UpdateLocalTransform();
            m_child_list[i] = m_child_list[--m_childCount];
            return;
        }
    }
}

void hyunwoo::Transform::RemoveChildAt(uint32_t index)
{
    /**********************************************
     *  유효하지 않은 인덱스라면, 함수를 종료한다...
     *****/
    if (index < 0 || index >= m_childCount) {
        return;
    }

    m_child_list[index]->m_parent = nullptr;
    m_child_list[index]->UpdateLocalTransform();
    m_child_list[index] = m_child_list[m_childCount - 1];
    m_childCount--;
}
















/*=============================================================================================================================================
 *   해당 Transform의 모델링 행렬 (T*R*S)를 얻습니다....
 ***********/
const hyunwoo::Matrix4x4 hyunwoo::Transform::GetTRS()
{
    if (m_isDirty) {
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












/*==============================================================================================================================================
 *   로컬 트랜스폼으로부터 월드 트랜스폼을 구해 설정합니다...
 *************/
void hyunwoo::Transform::UpdateWorldTransform()
{
    /*-------------------------------------------
     *  부모가 없다면, 로컬과 똑같은 값을 가진다...
     *****/
    if (m_parent==nullptr) {
        m_world_position = m_local_position;
        m_world_rotation = m_local_rotation;
        m_world_scale    = m_local_scale;
        m_isDirty        = false;
        return;
    }


    m_world_position = ((m_parent->m_world_rotation * m_local_position) * m_parent->m_world_scale) + m_parent->m_world_position;
    m_world_rotation = (m_parent->m_world_rotation * m_local_rotation);
    m_world_scale    = (m_parent->m_world_scale * m_local_scale);
    m_isDirty        = false;
}

















/*==============================================================================================================================================
 *   월드 트랜스폼으로부터 로컬 트랜스폼을 구해 설정합니다...
 *************/
void hyunwoo::Transform::UpdateLocalTransform()
{
    /*-------------------------------------------
     *  부모가 없다면, 월드와 똑같은 값을 가진다...
     *****/
    if (m_parent==nullptr) {
        m_local_position = m_world_position;
        m_local_rotation = m_world_rotation;
        m_local_scale    = m_world_scale;
        m_isDirty        = false;
        return;
    }


    const float parentScaleDiv  = (1.f / m_parent->m_world_scale.x);
    Quaternion  parentConjugate = m_parent->m_world_rotation.GetConjugate();

    m_local_position = parentConjugate * ((m_world_position - m_parent->m_world_position) * parentScaleDiv);
    m_local_rotation = (parentConjugate * m_world_rotation);
    m_local_scale    = (m_world_scale * parentScaleDiv);
    m_isDirty        = false;
}















/*=============================================================================================================================================
 *   해당 Transform의 자식들의 Transform들이 재계산할 필요가있다고 체크한다....
 ***********/
void hyunwoo::Transform::UpdateChildDirties()
{
    for (uint32_t i = 0; i < m_childCount; i++) {
        Transform* childTr = m_child_list[i];

        //이전에 isDirty상태가 되었다면, 무시한다...
        if (childTr->m_isDirty == false) {
            childTr->m_isDirty = true;
            childTr->UpdateChildDirties();
        }
        else uint8_t a = 0;
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
    if (m_parent!=nullptr && m_parent->m_isDirty) 
    {
        m_parent->UpdateDirtyParentWorldTransforms();
        m_parent->UpdateWorldTransform();
        m_parent->m_isDirty = false;
    }
}
