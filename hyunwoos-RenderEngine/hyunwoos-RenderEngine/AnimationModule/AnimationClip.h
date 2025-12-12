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
	enum class PropertyType : uint32_t
	{
		Local_Position,
		Local_Scale,
		Local_Rotation_Quat,
		Local_Rotation_Euler
	};

	/*********************************************
	 *  프로퍼티의 키프레임을 나타내는 열거형입니다..
	 ******/
	struct KeyFrame final
	{
		float	 Time;
		uint32_t CurveStartIdx;

		union 
		{
		   Vector3    Vec3;
		   Quaternion Quat = Quaternion::Identity;
		};
	};

	/*****************************************
	 *  애니메이션에 사용되는 프로퍼티를 나타내는
	 *  구조체입니다...
	 ******/
	struct Property final
	{
		PropertyType		      Type;
		WStringKey			      Name;
		std::vector<KeyFrame>     KeyFrames;
		std::vector<CurveVariant> Curves;

		Property(PropertyType type, WStringKey name) :Type(type), Name(name) {};
	};



	//============================================================================================
	////////////////						  Properties...						//////////////////
	//============================================================================================
public:
	float				  TotalTime = 0.f;
	std::vector<Property> Properties;
};