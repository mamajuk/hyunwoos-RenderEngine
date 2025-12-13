#pragma once
#include <string>
#include <vector>
#include <fstream>
#include "../MathModule/Vector.h"
#include "../RenderModule/Material.h"
#include "../RenderModule/Mesh.h"
#include "../AnimationModule/IK/CCD_IKSolver.h"
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
	struct MaterialFlags
	{
		uint8_t No_Cull		   : 1 = 0;
		uint8_t Ground_Shadow  : 1 = 0;
		uint8_t Draw_Shadow    : 1 = 0;
		uint8_t Receive_Shadow : 1 = 0;
		uint8_t Has_Edge	   : 1 = 0;
		uint8_t Vertex_Colour  : 1 = 0;
		uint8_t Point_Drawing  : 1 = 0;
		uint8_t Line_Drawing   : 1 = 0;
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


	/**************************************************
	 *  본이 가지고 있는 정보를 나타내는 플래그들을 
	 *  나타내는 열거형입니다...
	 *******/
	struct BoneFlags
	{
		uint16_t Indexed_tail_position  : 1 = 0;
		uint16_t Rotatable			    : 1 = 0;
		uint16_t Translatable		    : 1 = 0;
		uint16_t Is_visible			    : 1 = 0;
		uint16_t Enabled			    : 1 = 0;
		uint16_t IK					    : 1 = 0;
		uint16_t EmptyFlag1			    : 1 = 0;
		uint16_t EmptyFlag2			    : 1 = 0;
		uint16_t Inherit_rotation	    : 1 = 0;
		uint16_t Inherit_translation    : 1 = 0;
		uint16_t Fixed_axis			    : 1 = 0;
		uint16_t Local_coordinate	    : 1 = 0;
		uint16_t Physics_after_deform   : 1 = 0;
		uint16_t External_parent_deform : 1 = 0;
	};


	/**********************************************
	 *  Pmx 파일 헤더의 글로벌값들의 목록이 정의된
	 *  구조체입니다. 각 글로벌값들은 특정 데이터를
	 *  불러오는데 필요한 데이터 타입의 크기를 
	 *  나타냅니다..
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
	 *   Pmx 파일로부터 얻어올 데이터를
	 *   담을 곳을 서술하는 구조체입니다...
	 ********/
	struct StorageDescription
	{
		Mesh*					OutMesh		   = nullptr;
		CCD_IKSolver*			OutCCDIKSolver = nullptr;
		std::vector<Texture2D>* OutTextures	   = nullptr;
		std::vector<Material>*  OutMaterials   = nullptr;
	};





	//=============================================================================================
	/////////////////					   Public methods..						  /////////////////
	//=============================================================================================
public:
	static ImportResult Import(const StorageDescription& loadDesc, const wchar_t* path);





	//=================================================================================================
	/////////////////					   Private methods..						  /////////////////
	//=================================================================================================
private:
	/*****************************
	 *   유틸리티 메소드...
	 *****/
	static void    ReadText(std::ifstream& in, const Header& header, std::wstring* out_u16_text = nullptr);
	static int32_t ReadDefaultIndexType(std::ifstream& in, const DefaultIndexType defaultIdxType);


	/********************************
	 *   버텍스 데이터 관련 메소드....
  	 *****/
	static void Import_StoreVertexData(std::ifstream& in, const Header& header, const StorageDescription& storageDesc);
	static void Import_IgnoreVertexData(std::ifstream& in, const Header& header);


	/*********************************
	 *  인덱스 데이터 관련 메소드...
	 *******/
	static void Import_StoreTriangleData(std::ifstream& in, const Header& header, const StorageDescription& storageDesc);
	static void Import_IgnoreTriangleData(std::ifstream& in, const Header& header);


	/********************************
	 *  텍스쳐 데이터 관련 메소드...
	 *******/
	static void Import_StoreTextureData(std::ifstream& in, const Header& header, const wchar_t* path, ImportResult& outRet, const StorageDescription& storageDesc);
	static void Import_IgnoreTextureData(std::ifstream& in, const Header& header);


	/*******************************
	 *  머터리얼 데이터 관련 메소드...
	 *******/
	static void Import_StoreMaterialData(std::ifstream& in, const Header& header, ImportResult& outRet, const StorageDescription& storageDesc);
	static void Import_IgnoreMaterialData(std::ifstream& in, const Header& header);


	/*******************************
	 *  본 데이터 관련 메소드...
	 *******/
	static void Import_StoreBoneData(std::ifstream& in, const Header& header, const StorageDescription& storageDesc);
};




