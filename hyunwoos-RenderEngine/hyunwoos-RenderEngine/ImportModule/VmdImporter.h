#pragma once
#include <fstream>
#include <string>
#include "../MathModule/Vector.h"
#include "../MathModule/Quaternion.h"
#include "../AnimationModule/AnimationClip.h"
#include "../UtilityModule/StringKey.h"

namespace hyunwoo {
	class VmdImporter;
}



/*=================================================================================================================================================
 *    Vmd 파일을 로드하는 기능이 구현된 클래스입니다...
 *=================*/
class hyunwoo::VmdImporter final
{
	//===========================================================================================
	/////////////////						 Defines...							/////////////////
	//===========================================================================================
public:
	/*************************************
	 *   Vmd 파일의 시그니처를 나타내는
	 *   30bytes 이하의 문자열입니다...
	 ********/
	constexpr inline static const char* const Signature_v1_30       = "Vocaloid Motion Data file";
	constexpr inline static const char* const Signature_v1_30_later = "Vocaloid Motion Data 0002";


	/*************************************
	 *   Vmd 파일의 임포트 결과를 보관하는
	 *   구조체입니다..
	 ********/
	struct ImportResult final
	{
		bool Success		           : 1;
		bool Failed_OpenFile           : 1;
		bool IsNotVmdFile              : 1;
		bool CreatedWithMMD_1_30       : 1;
		bool CreatedWithMMD_1_30_later : 1;
	};


	/*******************************************
	 *   본의 프레임 구조를 나타내는 구조체입니다..
	 *********/
	struct BoneKeyFrame final
	{
		struct InterpolationData
		{
			Vector2 ControlPointX[2];
			Vector2 ControlPointY[2];
			Vector2 ControlPointZ[2];
			Vector2 ControlPointR[2];
		};

		uint32_t		  FrameNumber;
		Vector3			  BonePosition;
		Quaternion		  BoneRotation;
		InterpolationData Interpolations;
	};


	/********************************************
	 *   얼굴의 프레임 구조를 나타내는 구조체입니다..
	 *********/
	struct FaceKeyFrame final
	{
		WStringKey   Name;
		uint32_t     FrameNumber;
		float        Weight;
	};


	/**********************************************
	 *   카메라의 프레임 구조를 나타내는 구조체입니다..
	 *********/
	struct CameraKeyFrame final
	{
		uint32_t FrameNumber;
		float    CamToTargetDistance;
		float	 TargetPosition[3];
		float    CameraRotation[3];
		uint8_t  InterpolationData[24];
		uint32_t CameraFOVAngle;
		uint8_t  CameraPerspective;
	};




	//=============================================================================================
	/////////////////					   Public methods..						  /////////////////
	//=============================================================================================
public:
	static ImportResult Import(AnimationClip& outAnimClip, const wchar_t* path);




	//=================================================================================================
	/////////////////					   Private methods..						  /////////////////
	//=================================================================================================
private:
	static void ReadString(std::ifstream& in, uint32_t shiftJIS_StrSize, wchar_t* outUTF16Str);

};