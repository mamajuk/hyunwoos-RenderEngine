#pragma once
#include "../MathModule/Vector.h"
#include "../GeometryModule/Curve.h"
#include "../EngineModule/UniqueableObject.h"
#include "../UtilityModule/StringKey.h"
#include <vector>

namespace hyunwoo {
	class  AnimationClip;
}


/*==================================================================================================================================================================
 *    애니메이션 정보가 담겨있는 구조체입니다....
 ************/
class hyunwoo::AnimationClip final : public UniqueableObject
{
	//========================================================================================
	////////////////						  Defines..						//////////////////
	//========================================================================================
public:
	/*****************************************
	 *  프로퍼티의 타입을 나타내는 열거형입니다..
	 ******/
	enum class PropertyType
	{
		LocalScale,
		LocalRotation,
		LocalPosition
	};

	/*********************************************
	 *  프로퍼티의 키프레임을 나타내는 열거형입니다..
	 ******/
	struct KeyFrame final
	{
		float		 Time;
		float		 Value;
		CurveVariant Curve;
	};

	/*****************************************
	 *  애니메이션에 사용되는 프로퍼티를 나타내는
	 *  구조체입니다...
	 ******/
	struct Property final
	{
		PropertyType		  Type;
		WStringKey			  Name;
		std::vector<KeyFrame> KeyFrames;
	};



	//============================================================================================
	////////////////						  Properties...						//////////////////
	//============================================================================================
public:
	std::vector<Property> Properties;
};