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
 *   해당 Transform이 Unique하지 않게 되기 전에, 부모 Transform이 유효하다면 부모의 자식 목록에서 자신을 제거한다...
 ***********/
void hyunwoo::Transform::OnUnUniqued()
{
    SetParent(nullptr);
}














/*=========================================================================================================================================
 *   로컬 위치를 얻거나/설정합니다..... 
 ***********/
const hyunwoo::Vector3 hyunwoo::Transform::GetLocalPosition() const
{
    return m_local_position;
}

void hyunwoo::Transform::SetLocalPosition(const Vector3& newPosition)
{
    m_local_position = newPosition;
    m_world_position = 
}














/*=========================================================================================================================================
 *   월드 위치를 얻거나/설정합니다.....
 ***********/
const hyunwoo::Vector3 hyunwoo::Transform::GetWorldPosition() const
{
    return m_world_position;
}

void hyunwoo::Transform::SetWorldPosition(const Vector3& newPosition)
{
    m_world_position = newPosition;
}














/*=========================================================================================================================================
 *   로컬 스케일을 얻거나/설정합니다.....
 ***********/
const hyunwoo::Vector3 hyunwoo::Transform::GetLocalScale() const
{
    return m_local_scale;
}

void hyunwoo::Transform::SetLocalScale(const Vector3& newScale)
{
    m_local_scale = newScale;
}










/*=========================================================================================================================================
 *   월드 스케일을 얻거나/설정합니다.....
 ***********/
const hyunwoo::Vector3 hyunwoo::Transform::GetWorldScale() const
{
    return m_world_scale;
}

void hyunwoo::Transform::SetWorldScale(const Vector3& newScale)
{
    m_world_scale = newScale;
}













/*=========================================================================================================================================
 *   로컬 회전량을 얻거나/설정합니다.....
 ***********/
const hyunwoo::Quaternion hyunwoo::Transform::GetLocalRotation() const
{
    return m_local_rotation;
}

void hyunwoo::Transform::SetLocalRotation(const Quaternion& newRotation)
{
    m_local_rotation = newRotation;
}













/*=========================================================================================================================================
 *   월드 회전량을 얻거나/설정합니다.....
 ***********/
const hyunwoo::Quaternion hyunwoo::Transform::GetWorldRotation() const
{
    return m_world_rotation;
}

void hyunwoo::Transform::SetWorldRotation(const Quaternion& newRotation)
{
    m_world_rotation = newRotation;
}











/*=========================================================================================================================================
 *   부모 Transform을 설정하거나/약참조를 얻습니다..
 ***********/
hyunwoo::WeakPtr<hyunwoo::Transform> hyunwoo::Transform::GetParent() const
{
    return m_parent;
}

void hyunwoo::Transform::SetParent(WeakPtr<Transform> newParent)
{
    /*********************************************************
     *   기존 부모가 설정되어 있었다면, 부모->자식관계를 해제한다..
     *******/
    Transform* const prev_parent_rawPtr = m_parent.Get();
    Transform* const new_parent_rawPtr  = newParent.Get();

    if (prev_parent_rawPtr!=nullptr) {
        prev_parent_rawPtr->RemoveChild(*this);
    }



    /***********************************************************
     *  새로운 부모가 유효하다면, 자신을 자식으로 추가한다...   
     ********/
    m_parent = newParent;

    if (new_parent_rawPtr!=nullptr) {
        new_parent_rawPtr->AddChild(*this);
    }
}











/*=========================================================================================================================================
 *    자식 Transform의 개수와, 특정 인덱스에 있는 자식 Transform의 약참조를 얻습니다....
 ***********/
const uint32_t hyunwoo::Transform::GetChildCount() const
{
    return m_childCount;
}

hyunwoo::WeakPtr<hyunwoo::Transform> hyunwoo::Transform::GetChildAt(uint32_t index) const
{
    //유효한 인덱스 범위를 벗어났다면, null 약참조 객체를 반환한다...
    if (index < 0 || index >= m_childCount) {
        return WeakPtr<Transform>();
    }

    return m_child_list[index];
}














/*=========================================================================================================================================
 *    새로운 자식 Transform을 추가합니다....
 ***********/
void hyunwoo::Transform::AddChild(Transform& newChild)
{
    /********************************************************
     *  이미 추가되어 있는 자식 Transform이라면, 무시한다....
     *******/
    if (newChild.m_parent.Get()==this) {
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
        if (m_childCount > 1) {
            m_child_list    = new WeakPtr<Transform>[3]();
            m_child_list[0] = m_localBuf[0];
            m_capacity  = 3;
        }
    }


    
    /************************************************************
     *  동적 버퍼의 공간이 부족하면, 기존 크기의 배인 버퍼를 새로 
     *  할당하고, 기존 리소스를 옮긴다....
     *******/
    else if (m_capacity < (m_childCount + 1))
    {
        WeakPtr<Transform>* prev_list = m_child_list;

        m_child_list = new WeakPtr<Transform>[(m_capacity *= 2)]();
        memcpy((void*)m_child_list, (void*)prev_list, (sizeof(WeakPtr<Transform>) * m_childCount));
        
        delete[] prev_list;
    }


    /*************************************************
     *  해당 객체를 자식으로 추가한다....
     *******/
    m_child_list[m_childCount++] = &newChild;
    newChild.m_parent            = this;
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
        Transform* cur_raw_ptr = m_child_list[i].Get();

        //제거할 대상과 동일한 객체를 제거한다...
        if (&removeChild == cur_raw_ptr) {
            removeChild.m_parent.Reset();
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


    /***********************************************
     *  
     *******/
    Transform* raw_ptr = m_child_list[index].Get();
    if (raw_ptr!=nullptr) {
        raw_ptr->m_parent.Reset();
    }

    m_child_list[index] = m_child_list[m_childCount - 1];
    m_childCount--;
}













/*=============================================================================================================================================
 *   해당 Transform의 모델링 행렬 (T*R*S)를 얻습니다....
 ***********/
const hyunwoo::Matrix4x4 hyunwoo::Transform::GetTRS() const
{
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
