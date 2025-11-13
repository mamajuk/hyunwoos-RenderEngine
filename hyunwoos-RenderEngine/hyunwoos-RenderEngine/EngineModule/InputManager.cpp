#include "InputManager.h"

/*=============================================================
 *   인자로 준 키가 현재 눌리고 있는 중인지를 확인합니다....
 *==========*/
bool hyunwoo::InputManager::IsInProgress(KeyCode keyCode) const
{
    const KeyState& state = m_keyStates[(int)keyCode];
    return (state.IsInProgress==true || state.WasPressedThisFrame==true);
}




/*=============================================================
 *   인자로 준 키가 현재 프레임에서 눌렸는지를 확인합니다...
 *==========*/
bool hyunwoo::InputManager::WasPressedThisFrame(KeyCode keyCode) const
{
    return (m_keyStates[(int)keyCode].WasPressedThisFrame== true);
}




/*=============================================================
 *   인자로 준 키가 현재 프레임에서 떼어졌는지를 확인합니다...
 *==========*/
bool hyunwoo::InputManager::WasReleasedThisFrame(KeyCode keyCode) const
{
    return (m_keyStates[(int)keyCode].WasReleasedThisFrame== true);
}



/*=============================================================
 *   인자로 준 키들로 축값을 얻습니다....
 *==========*/
float hyunwoo::InputManager::GetAxis(KeyCode minusKey, KeyCode plusKey) const
{
    float ret = 0.f;
    if (IsInProgress(minusKey)) ret += -1.f;
    if (IsInProgress(plusKey)) ret += 1.f;

    return ret;
}



/*=============================================================
 *   마우스 좌표를, 인자로 준 뷰포트의 해상도에 맞게 변경합니다...
 *==========*/
hyunwoo::Vector2 hyunwoo::InputManager::GetMouseScreenPosition(const Renderer::ViewPort& viewPort) const
{
    return Vector2();
}



/*=============================================================
 *   클라이언트 창 해상도에 알맞는 마우스 좌표를 얻습니다...
 *==========*/
hyunwoo::Vector2 hyunwoo::InputManager::GetMouseScreenPosition() const {
    return m_mouse_screenPos;
}



/*=============================================================
 *   마우스 좌표를, 인자로 준 뷰포트의 해상도에 맞게 변경합니다...
 *==========*/
void hyunwoo::InputManager::SetMouseScreenPosition(Vector2 newPosition) {
    m_mouse_screenPos = newPosition;
}