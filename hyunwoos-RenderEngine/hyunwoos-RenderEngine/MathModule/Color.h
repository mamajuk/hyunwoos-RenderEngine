#pragma once
#include <windows.h>
#include "Vector.h"

namespace hyunwoo {
	struct LinearColor;
}

/**************************************************
 *   정규화된 선형 색상이 구현된 구조체입니다...
 *********/
struct hyunwoo::LinearColor final
{
	//===============================================
	////////		   Property...			 ////////
	//===============================================
	float r, g, b, a;


	static const LinearColor White;
	static const LinearColor Black;
	static const LinearColor Red;
	static const LinearColor Blue;
	static const LinearColor Yellow;
	static const LinearColor Pink;
	static const LinearColor Purple;
	static const LinearColor Green;




	//======================================================
	////////		   Public methods..				////////
	//======================================================
	constexpr LinearColor(float r = 0.f, float g = 0.f, float b = 0.f, float a = 1.f): r(r), g(g),b(b), a(a) {}
	constexpr LinearColor(const LinearColor&) = default;
	constexpr ~LinearColor()			      = default;

	DWORD ToDWORD_rgba(int rOrder, int gOrder, int bOrder, int aOrder) const;
	DWORD ToDWORD_rgb(int rOrder, int gOrder, int bOrder) const;

	void AddString(std::string& sharedStr) const {
		sharedStr += "(";
		sharedStr += std::to_string(r);
		sharedStr += ", ";
		sharedStr += std::to_string(g);
		sharedStr += ", ";
		sharedStr += std::to_string(b);
		sharedStr += ", ";
		sharedStr += std::to_string(a);
		sharedStr += ")";
	}

	void AddWString(std::wstring& sharedStr) const {
		sharedStr += L"(";
		sharedStr += std::to_wstring(r);
		sharedStr += L", ";
		sharedStr += std::to_wstring(g);
		sharedStr += L", ";
		sharedStr += std::to_wstring(b);
		sharedStr += L", ";
		sharedStr += std::to_wstring(a);
		sharedStr += L")";
	}



	//=========================================================
	/////////			Operator methods...			  /////////
	//=========================================================
	constexpr const LinearColor operator+(const LinearColor& rhs) const {
		return LinearColor((r+rhs.r), (g+rhs.g), (b+rhs.b), (a+rhs.a));
	}

	const LinearColor operator-(const LinearColor& rhs) const {
		return LinearColor((r-rhs.r), (g-rhs.g), (b-rhs.b), (a-rhs.a));
	}

	const LinearColor operator*(const LinearColor& rhs) const {
		return LinearColor((r*rhs.r), (g*rhs.g), (b*rhs.b), (a*rhs.a));
	}

	const LinearColor operator*(const float scalar) const {
		return LinearColor((r*scalar), (g*scalar), (b*scalar), (a*scalar));
	}

	LinearColor& operator+=(const LinearColor& rhs){
		r += rhs.r;
		g += rhs.g;
		b += rhs.b;
		a += rhs.a;
		return *this;
	}

	LinearColor& operator-=(const LinearColor& rhs) {
		r -= rhs.r;
		g -= rhs.g;
		b -= rhs.b;
		a -= rhs.a;
		return *this;
	}

	LinearColor& operator*=(const LinearColor& rhs) {
		r *= rhs.r;
		g *= rhs.g;
		b *= rhs.b;
		a *= rhs.a;
		return *this;
	}

	LinearColor& operator+=(const float scalar) {
		r += scalar;
		g += scalar;
		b += scalar;
		a += scalar;
		return *this;
	}
};