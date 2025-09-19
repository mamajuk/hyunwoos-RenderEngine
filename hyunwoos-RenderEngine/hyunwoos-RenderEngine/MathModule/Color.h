#pragma once
#include <windows.h>
#include "Vector.h"

namespace hyunwoo {
	struct Color;
	struct LinearColor;
}



/*================================================================================================
 *   정수형 색상이 구현된 구조체입니다...
 *=========*/
struct hyunwoo::Color final
{
	//===============================================
	////////		   Property...			 ////////
	//===============================================
	union{
		struct
		{
			DWORD B : 8;
			DWORD G : 8;
			DWORD R : 8;
			DWORD A : 8;
		};

		DWORD ARGB;
	};

	static const Color White;
	static const Color Black;
	static const Color Red;
	static const Color Blue;
	static const Color Yellow;
	static const Color Pink;
	static const Color Purple;
	static const Color Green;





	//======================================================
	////////		   Public methods..				////////
	//======================================================
	constexpr Color(DWORD R = 0, DWORD G = 0, DWORD B = 0, DWORD A = 1) :R(R), G(G), B(B), A(A) {};
	constexpr Color(const Color&) = default;
	constexpr ~Color()			  = default;


	void AddString(std::string& sharedStr) const {
		sharedStr += "(";
		sharedStr += std::to_string(R);
		sharedStr += ", ";
		sharedStr += std::to_string(G);
		sharedStr += ", ";
		sharedStr += std::to_string(B);
		sharedStr += ", ";
		sharedStr += std::to_string(A);
		sharedStr += ")";
	}

	void AddWString(std::wstring& sharedStr) const {
		sharedStr += L"(";
		sharedStr += std::to_wstring(R);
		sharedStr += L", ";
		sharedStr += std::to_wstring(G);
		sharedStr += L", ";
		sharedStr += std::to_wstring(B);
		sharedStr += L", ";
		sharedStr += std::to_wstring(A);
		sharedStr += L")";
	}



	//=========================================================
	/////////			Operator methods...			  /////////
	//=========================================================
	constexpr const bool operator==(const Color& rhs) const {
		return ((R == rhs.R) && (G == rhs.G) && (B == rhs.B) && (A == rhs.A));
	}

	constexpr const bool operator!=(const Color& rhs) const {
		return !(*this == rhs);
	}

	constexpr const Color operator+(const Color& rhs) const {
		return Color((R + rhs.R), (G + rhs.G), (B + rhs.B), (A + rhs.A));
	}

	constexpr const Color operator-(const Color& rhs) const {
		return Color((R - rhs.R), (G - rhs.G), (B - rhs.B), (A - rhs.A));
	}

	constexpr const Color operator*(const Color& rhs) const {
		return Color((R * rhs.R), (G * rhs.G), (B * rhs.B), (A * rhs.A));
	}

	constexpr const Color operator*(const DWORD& scalar) const {
		return Color((R*scalar), (G*scalar), (B*scalar), (A * scalar));
	}

	operator LinearColor() const;

	Color& operator+=(const Color& rhs) {
		R += rhs.R;
		G += rhs.G;
		B += rhs.B;
		A += rhs.A;
		return *this;
	}

	Color& operator-=(const Color& rhs) {
		R -= rhs.R;
		G -= rhs.G;
		B -= rhs.B;
		A -= rhs.A;
		return *this;
	}

	Color& operator*=(const Color& rhs) {
		R *= rhs.R;
		G *= rhs.G;
		B *= rhs.B;
		A *= rhs.A;
		return *this;
	}

	Color& operator+=(const DWORD scalar) {
		R += scalar;
		G += scalar;
		B += scalar;
		A += scalar;
		return *this;
	}
};











/*================================================================================================
 *   정규화된 선형 색상이 구현된 구조체입니다...
 *=========*/
struct hyunwoo::LinearColor final
{
	//===============================================
	////////		   Property...			 ////////
	//===============================================
	float R, G, B, A;


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
	constexpr LinearColor(DWORD ARGB) :A((ARGB >> 24)/255.f), R(((ARGB << 8) >> 24)/255), G(((ARGB << 16) >> 24)/255), B(((ARGB << 24) >> 24)/255) {}
	constexpr LinearColor(float r = 0.f, float g = 0.f, float b = 0.f, float a = 1.f): R(r), G(g), B(b), A(a) {}
	constexpr LinearColor(const LinearColor&) = default;
	constexpr ~LinearColor()			      = default;

	void AddString(std::string& sharedStr) const {
		sharedStr += "(";
		sharedStr += std::to_string(R);
		sharedStr += ", ";
		sharedStr += std::to_string(G);
		sharedStr += ", ";
		sharedStr += std::to_string(B);
		sharedStr += ", ";
		sharedStr += std::to_string(A);
		sharedStr += ")";
	}

	void AddWString(std::wstring& sharedStr) const {
		sharedStr += L"(";
		sharedStr += std::to_wstring(R);
		sharedStr += L", ";
		sharedStr += std::to_wstring(G);
		sharedStr += L", ";
		sharedStr += std::to_wstring(B);
		sharedStr += L", ";
		sharedStr += std::to_wstring(A);
		sharedStr += L")";
	}



	//=========================================================
	/////////			Operator methods...			  /////////
	//=========================================================
	constexpr const bool operator==(const LinearColor& rhs) const {
		return ((R==rhs.R) && (G==rhs.G) && (B==rhs.B) && (A==rhs.A));
	}

	constexpr const bool operator!=(const LinearColor& rhs) const {
		return !(*this==rhs);
	}

	constexpr const LinearColor operator+(const LinearColor& rhs) const {
		return LinearColor((R+rhs.R), (G+rhs.G), (B+rhs.B), (A+rhs.A));
	}

	const LinearColor operator-(const LinearColor& rhs) const {
		return LinearColor((R-rhs.R), (G-rhs.G), (B-rhs.B), (A-rhs.A));
	}

	const LinearColor operator*(const LinearColor& rhs) const {
		return LinearColor((R*rhs.R), (G*rhs.G), (B*rhs.B), (A*rhs.A));
	}

	const LinearColor operator*(const float scalar) const {
		return LinearColor((R*scalar), (G*scalar), (B*scalar), (A*scalar));
	}

	operator Color() const;

	LinearColor& operator+=(const LinearColor& rhs){
		R += rhs.R;
		G += rhs.G;
		B += rhs.B;
		A += rhs.A;
		return *this;
	}

	LinearColor& operator-=(const LinearColor& rhs) {
		R -= rhs.R;
		G -= rhs.G;
		B -= rhs.B;
		A -= rhs.A;
		return *this;
	}

	LinearColor& operator*=(const LinearColor& rhs) {
		R *= rhs.R;
		G *= rhs.G;
		B *= rhs.B;
		A *= rhs.A;
		return *this;
	}

	LinearColor& operator+=(const float scalar) {
		R += scalar;
		G += scalar;
		B += scalar;
		A += scalar;
		return *this;
	}
};