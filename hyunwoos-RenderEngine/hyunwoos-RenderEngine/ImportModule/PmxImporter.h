#pragma once
#include <string>
#include <vector>
#include "../MathModule/Vector.h"
#include "../RenderModule/RenderResources.h"
#include "PngImporter.h"

namespace hyunwoo {
	class PmxImporter;
}


/*=================================================================================================================================================
 *    Pmx 파일을 로드하는 기능이 구현된 클래스입니다...
 *=================*/
class hyunwoo::PmxImporter final
{
	//============================================================================================
	/////////////////							Defines..							//////////////
	//============================================================================================
public:

	/***************************************
	 *   Pmx 파일의 시그니처를 나타내는 
	 *   4bytes 정수값입니다...
	 *******/
	constexpr static const uint32_t Signature = ' XMP';



	/********************************************
	 *   텍스트 인코딩 값을 나타내는 열거형입니다..
	 ******/
	enum class TextEncodingType : uint8_t
	{
		UTF16LE = 0,
		UTF8    = 1
	};



	/********************************************
	 *   스키닝 애니메이션에서, 각 버텍스를를 본의
	 *   가중치로 어떻게 변형할지를 나타내는 열거형
	 *   입니다...
	 ******/
	enum class WeightDeformType : uint8_t
	{
		BDEF1 = 0,
		BDEF2 = 1,
		BDEF4 = 2,
		SDEF  = 3,
		QDEF  = 4
	};



	/****************************************************
	 *   각 데이터들의 인덱스의 타입을 정의한 열거형입니다..
	 ******/
	enum class VertexIndexType : uint8_t
	{
		Type1_ubyte  = 1,
		Type2_ushort = 2,
		Type4_int    = 4,
	};

	enum class DefaultIndexType : uint8_t
	{
		Type1_byte  = 1,
		Type2_short = 2,
		Type4_int   = 4,
		Null		= -1
	};


	/**********************************************
	 *   머터리얼 플래그를 나타내는 열거형입니다...
	 ******/
	enum class MaterialFlag
	{
		No_Cull        = 1,
		Ground_Shadow  = 2,
		Draw_Shadow    = 4,
		Receive_Shadow = 8,
		Has_Edge	   = 16,
		Vertex_Colour  = 32,
		Point_Drawing  = 64,
		Line_Drawing   = 128
	};


	/*****************************************************
	 *   머터리얼의 환경 블랜드 모드를 나타내는 열거형입니다..
	 ********/
	enum class EnvironmentBlendModeType
	{
		Disabled		= 0,
		Multiply		= 1,
		Additive		= 2,
		Additional_Vec4 = 3
	};


	/**************************************************
	 *  머터리얼의 툰 참조값을 나타나내는 열거형입니다...
	 *******/
	enum class ToonReferenceType
	{
		Texture_Reference  = 0,
		Internal_Reference = 1 
	};


	/*******************************************
	 *  Pmx 파일 헤더의 글로벌값들의 목록이 정의된
	 *  구조체입니다...
	 *******/
	struct Globals
	{
		TextEncodingType Text_Encoding;
		uint8_t     	 Additional_Vec4_Count;
		VertexIndexType  Vertex_Index_Size;
		DefaultIndexType Texture_Index_Size;
		DefaultIndexType Material_Index_Size;
		DefaultIndexType Bone_Index_Size;
		DefaultIndexType Morph_Index_Size;
		DefaultIndexType Rigidbody_Index_Size;
	};



	/*******************************************
	 *   Pmx 파일의 헤더를 나타내는 구조체입니다..
	 ********/
	struct Header
	{
		uint32_t Signature;
		float	 Version;
		uint8_t	 Globals_Count;
		Globals	 Globals;
	};



	/*************************************
	 *   Pmx 파일의 임포트 결과를 보관하는
	 *   구조체입니다..
	 ********/
	struct ImportResult
	{
		bool Success		        : 1;
		bool Failed_OpenFile        : 1;
		bool IsNotPmxFile	        : 1;
		bool LoadDataIsNothing      : 1;
		bool Failed_TextureStorage  : 1;
		bool Failed_MaterialStorage : 1;
		bool Require_TextureStorage : 1;

		PngImporter::ImportResult TextureLoadResult;
	};



	/*************************************
	 *   Pmx 파일로부터 얻어올 데이터들을
	 *   서술하는 구조체입니다...
	 ********/
	struct StorageDescription
	{
		Mesh*					OutMesh		 = nullptr;
		std::vector<Texture2D>* OutTextures	 = nullptr;
		std::vector<Material>*  OutMaterials = nullptr;
	};



	//=============================================================================================
	/////////////////					   Public methods..						  /////////////////
	//=============================================================================================
public:
	static ImportResult Import(const StorageDescription& loadDesc, const wchar_t* path);


};




