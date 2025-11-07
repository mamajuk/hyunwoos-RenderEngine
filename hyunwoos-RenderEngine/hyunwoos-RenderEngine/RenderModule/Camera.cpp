#include "Camera.h"

/*===============================================================================================================================
 *   해당 카메라를 기준으로 공간 변환을 할 수 있는 뷰 행렬을 얻습니다....
 **********/
const hyunwoo::Matrix4x4 hyunwoo::Camera::GetViewMatrix() const
{
    if (Transform* attached_to = GetAttachedTransform().Get(); attached_to!=nullptr) {
        return attached_to->GetTRS_Inverse();
    }

    return Matrix4x4::Identity;
}



/*===============================================================================================================================
 *   해당 카메라에 설정된 정보를 바탕으로 원근 투영 변환( 클립 좌표계로의 변환 )을 할 수 있는 원근 투영 행렬을 얻습니다...
 **********/
const hyunwoo::Matrix4x4 hyunwoo::Camera::GetPerspectiveMatrix(float ascpectRatio) const
{
    const float d = (1.f / Math::Tan(Fov * .5f * Math::Angle2Rad));
    const float k = (-Near - Far) / (Near - Far);
    const float l = -Near - (k * Near);

    return Matrix4x4(
        Vector4(d, 0.f, 0.f, 0.f),
        Vector4(0.f, (d*ascpectRatio), 0.f, 0.f),
        Vector4(0.f, 0.f, k, 1.f),
        Vector4(0.f, 0.f, l, 0.f)
    );
}
