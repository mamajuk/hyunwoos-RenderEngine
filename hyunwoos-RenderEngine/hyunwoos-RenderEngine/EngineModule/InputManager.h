#pragma once
#include "../MathModule/Vector.h"

namespace hyunwoo {
	class InputManager;
}



/*======================================================================================================================================
 *    사용자의 키보드/마우스 입력에 대한 정보를 보관하는 클래스입니다...
 *=============*/
class hyunwoo::InputManager final
{
	//======================================================================================
	////////									Defines..							////////
	//=====================================================================================
public:

	/****************************************
	 *    입력된 키를 나타내는 열거형입니다...
	 *******/
	enum class KeyCode
	{
		Left_Mouse = 0x01,
		Right_Mouse = 0x02,
		Crtl_Break = 0x03,
		Mid_Mouse = 0x05,
		X1_Mouse = 0x05,
		X2_Mouse = 0x06,
		Back_Space = 0x08,
		Tab = 0x09,
		Clear = 0x0C,
		Enturn = 0x0D,
		Shift = 0x10,
		Control = 0x11,
		Alt = 0x12,
		Pause = 0x13,
		Caps_Lock = 0x14,
		IME_KANA = 0x14,
		IME_HANGUL = 0x15,
		IME_JUNJA = 0x17,
		IME_FINAL = 0x18,
		IME_KANJI = 0x19,
		IME_OFF = 0x1A,
		Esc = 0x1B,
		IME_CONVERT = 0x1A,
		IME_ACCEPT = 0x1E,
		IME_MODECHANGE = 0x1F,
		Space = 0x20,
		Page_Up = 0x21,
		Page_Down = 0x22,
		End = 0x23,
		Home = 0x24,
		Left = 0x25,
		Up = 0x26,
		Right = 0x27,
		Down = 0x28,
		Select = 0x29,
		Print = 0x2A,
		Execute = 0x2B,
		Print_Screen = 0x2C,
		Insert = 0x2D,
		Del = 0x2E,
		Help = 0x2F,
		Num_0 = 0x30,
		Num_1 = 0x31,
		Num_2 = 0x32,
		Num_3 = 0x33,
		Num_4 = 0x34,
		Num_5 = 0x35,
		Num_6 = 0x36,
		Num_7 = 0x37,
		Num_8 = 0x38,
		Num_9 = 0x39,
		A = 0x41,
		B = 0x42,
		C = 0x43,
		D = 0x44,
		E = 0x45,
		F = 0x46,
		G = 0x47,
		H = 0x48,
		I = 0x49,
		J = 0x4A,
		K = 0x4B,
		L = 0x4C,
		M = 0x4D,
		N = 0x4E,
		O = 0x4F,
		P = 0x50,
		Q = 0x51,
		R = 0x52,
		S = 0x53,
		T = 0x54,
		U = 0x55,
		V = 0x56,
		W = 0x57,
		X = 0x58,
		Y = 0x59,
		Z = 0x5A,
		Left_Window = 0x5B,
		Right_Window = 0x5C,
		Application = 0x5D,
		Sleep = 0x5F,
		NUMPAD_0 = 0x60,
		NUMPAD_1 = 0x61,
		NUMPAD_2 = 0x62,
		NUMPAD_3 = 0x63,
		NUMPAD_4 = 0x64,
		NUMPAD_5 = 0x65,
		NUMPAD_6 = 0x66,
		NUMPAD_7 = 0x67,
		NUMPAD_8 = 0x68,
		NUMPAD_9 = 0x69,
		Multiply = 0x6A,
		Add = 0x6B,
		Separator = 0x6C,
		Subtract = 0x6D,
		Decimal = 0x6E,
		Divide = 0x6F,
		F1 = 0x70,
		F2 = 0x71,
		F3 = 0x72,
		F4 = 0x73,
		F5 = 0x74,
		F6 = 0x75,
		F7 = 0x76,
		F8 = 0x77,
		F9 = 0x78,
		F10 = 0x79,
		F11 = 0x7A,
		F12 = 0x7B,
		Left_Shift = 0xA0,
		Right_Shift = 0xA1,
		Left_Ctrl = 0xA2,
		Right_Ctrl = 0xA3,
		Left_Alt = 0xA4,
		Right_Alt = 0xA5
	};
	

	/*****************************************
	 *  각 키들의 상태를 보관하는 구조체입니다...
	 ******/
	struct KeyState
	{
		bool IsInProgress         : 1;
		bool WasPressedThisFrame  : 1;
		bool WasReleasedThisFrame : 1;
	};



	//=====================================================================================
	///////									 Fields..								///////
	//=====================================================================================
private:
	Vector2  m_mouse_screenPos;
	KeyState m_keyStates[255] = { 0, };




	//========================================================================================
	////////								Public methods..						/////////
	//======================================================================================
public:

	/*******************************************
	 *
	 ******/
	bool IsInProgress(KeyCode keyCode)         const;
	bool WasPressedThisFrame(KeyCode keyCode)  const;
	bool WasReleasedThisFrame(KeyCode keyCode) const;

	float GetAxis(KeyCode minusKey, KeyCode plusKey) const;

	KeyState& GetKeyState(KeyCode keyCode) {
		return m_keyStates[(int)keyCode];
	}

	Vector2 GetMouseScreenPosition() const {
		return m_mouse_screenPos;
	}

	void SetMouseScreenPosition(Vector2 newPosition) {
		m_mouse_screenPos = newPosition;
	}
};