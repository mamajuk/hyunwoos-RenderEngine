#pragma once
#include <fstream>
#include <string>
#include <cstdint>
#include "../MathModule/Vector.h"
#include "../MathModule/Quaternion.h"
#include "../AnimationModule/AnimationClip.h"
#include "../UtilityModule/StringKey.h"
#include "../ImportModule/PmxImporter.h"

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
		bool InPmxMeshIsNotValid	   : 1;
	};



	/*******************************************
	 *   본의 보간 방식을 나타내는 구조체입니다...
	 *********/
	struct InterpolationData
	{
		uint8_t ControlPointX[4];
		uint8_t ControlPointY[4];
		uint8_t ControlPointZ[4];
		uint8_t ControlPointR[4];
		uint8_t disabled[48];
	};


	/*******************************************
	 *   IK의 키프레임 데이터를 나타내는 구조체..
	 *********/
	struct IK_KeyFrame
	{
		enum class DisplayMode : uint8_t { Off = 0, On  = 1 };
		enum class IKMode	   : uint8_t { Off = 0, On  = 1 };

		struct BoneDesc
		{
			WStringKey Name;
			IKMode     Mode;
		};

		uint32_t    FrameCount;
		DisplayMode DisplayMode;
		uint32_t    IKBoneCount;
		uint32_t    IKBoneStartIdx;
	};




	//=============================================================================================
	/////////////////					   Public methods..						  /////////////////
	//=============================================================================================
public:
	static ImportResult Import(AnimationClip& outAnimClip, const Mesh& inPmxMesh, const wchar_t* path);




	//=================================================================================================
	/////////////////					   Private methods..						  /////////////////
	//=================================================================================================
private:
	static void AddKeyFrame(AnimationClip::Property& prop, const AnimationClip::KeyFrame& keyFrame);
	static void ReadString(std::ifstream& in, uint32_t shiftJIS_StrSize, std::wstring& outUTF16Str);

};