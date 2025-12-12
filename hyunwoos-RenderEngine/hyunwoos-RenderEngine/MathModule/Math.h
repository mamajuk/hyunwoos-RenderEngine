#pragma once
#include <cmath>
#include "Vector.h"

namespace hyunwoo {
	class Math;
	class Vector2;
}


/*==========================================================================================================================================================
 *   수학 관련 정적 메소드들이 구현된 클래스입니다....
 ********/
class hyunwoo::Math final
{
	//========================================================================================================
	////////									Property..											//////////
	//========================================================================================================
public:
	static constexpr float PI        = 3.14159265359f;
	static constexpr float Rad2Angle = (180.f / PI);
	static constexpr float Angle2Rad = (PI / 180.f);



private:
	//========================================================================================================
	/////////								Constructor	and Destructor								//////////
	//========================================================================================================
	Math() = delete;
	Math(const Math&) = delete;
	~Math() = delete;



	//=========================================================================================================
	///////////								    Public methods..								    ///////////
	//=========================================================================================================
public:
	static const float Cos(const float radian) {
		return cosf(radian);
	}

	static const float Sin(const float radian) {
		return sinf(radian);
	}

	static const float Tan(const float radian) {
		return tanf(radian);
	}

	static const float Acos(const float cos_value) {
		return acosf(cos_value);
	}

	static const float ASin(const float sin_value) {
		return asinf(sin_value);
	}

	static const float Atan(const float tan_value) {
		return atanf(tan_value);
	}

	static const float Atan2(const float y, const float x) {
		return atan2f(y, x);
	}

	static const float Atan2(const hyunwoo::Vector2& rhs);

	static const float Sqrt(const float rhs) {
		return sqrtf(rhs);
	}

	static const int Sqrt(const int rhs) {
		return sqrt(rhs);
	}

	static const int Round(const float rhs) {
		return lround(rhs);
	}

	static const float Abs(const float rhs) {
		return fabsf(rhs);
	}

	static const int Abs(const int rhs) {
		return labs(rhs);
	}

	static const int Floor(const int rhs) {
		return floor(rhs);
	}

	static const int Floor(const float rhs) {
		return floorf(rhs);
	}

	static const float Clamp(const int Symbol, const int min, const int max) {
		if (Symbol < min) return min;
		if (Symbol > max) return max;
		return Symbol;
	}

	static const float Clamp(const float Symbol, const float min, const float max) {
		if (Symbol < min) return min;
		if (Symbol > max) return max;
		return Symbol;
	}

	static const float Clamp01(const float Symbol) {
		if (Symbol < 0.f) return 0.f;
		if (Symbol > 1.f) return 1.f;
		return Symbol;
	}

	static const float Repeat(const float symbol, const float min, const float max) {
		if (symbol > max) return (symbol - max);
		if (symbol < min) return symbol;

		return symbol;
	}

	template<typename T, typename...Tn>
	static const T Max(const T lhs, Tn... args) {
		float max = lhs;
		(((args>max) && (max=args)),...);
		return max;
	}

	template<typename T, typename...Tn>
	static const T Min(const T lhs, Tn... args) {
		float min = lhs;
		(((args < min) && (min = args)), ...);
		return min;
	}
};