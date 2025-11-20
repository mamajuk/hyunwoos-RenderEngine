#pragma once
#include "../MathModule/Vector.h"

namespace hyunwoo {
	struct CurveVariant;

	struct LinearBezierCurve;
	struct QuadraticBezierCurve;
	struct CubicBezierCurve;
}

/*===========================================================================================================================================
 *    1차 베지어 곡선이 구현된 커브입니다...
 ************/
struct hyunwoo::LinearBezierCurve final
{
	float Yvalues[2];
	float Evaluate(float normalizedTime) const;
};



/*===========================================================================================================================================
 *    2차 베지어 곡선이 구현된 커브입니다...
 ************/
struct hyunwoo::QuadraticBezierCurve final
{
	float Yvalues[3];
	float Evaluate(float normalizedTime) const;
};



/*===========================================================================================================================================
 *    3차 베지어 곡선이 구현된 커브입니다...
 ************/
struct hyunwoo::CubicBezierCurve final
{
	float Yvalues[4];
	float Evaluate(float normalizedTime) const;
};





/*===========================================================================================================================================
 *     다양한 커브들을 담을 수 있는 구조체입니다....
 ************/
struct hyunwoo::CurveVariant final
{
	//==========================================================================================
	//////////////							Defines..							////////////////
	//==========================================================================================
public:
	/********************************************
	 *   커브의 종류를 나타내는 열거형입니다...
	 *******/
	enum class VariantType : uint32_t
	{
		Linear_Bazier,
		Constant_Bazier,
		Cubic_Bezier
	};


	//==============================================================================================
	//////////////							Properties..							////////////////
	//==============================================================================================
public:
	VariantType Type = VariantType::Linear_Bazier;

	union {
		LinearBezierCurve	 LinearBezier;
		QuadraticBezierCurve QuadraticBezier;
		CubicBezierCurve	 CubicBezier;
	};
};