#pragma once
#include <cstdint>
#include <vector>
#include "../UtilityModule/BitStream.h"
#include "../UtilityModule/ByteStream.h"

namespace hyunwoo {
	class Zlib;
}


/*======================================================================================================================================================
 *    Zlib 알고리즘의 압축 해제 과정이 구현된 유틸리티 클래스입니다...
 *==============*/
class hyunwoo::Zlib final
{
	//===================================================================================================
	////////									 Defines..										/////////
	//===================================================================================================
public:
	/*******************************************************
	 *    Zlib 알고리즘 전용 허프만 트리가 구현된 클래스입니다..
	 *******/
	class HuffmanTree;



	/*********************************************
	 *   Zlib로 압축된 대상이, 어떤 압축
	 *   방식을 사용했는지 나타내는 열거형입니다..
	 ******/
	enum class BlockCompressionType : uint8_t
	{
		None		    = 0,
		Fixed_Huffman   = 1,
		Dynamic_Huffman = 2
	};



	/******************************************
	 *    어떤 압축 알고리즘을 사용했는지를 
	 *    나타내는 1bytes 유니온입니다...
	 ********/
	union CompressionMethod 
	{
		enum class CMType : uint8_t {
			Deflate  = 8, //Deflate 알고리즘이 사용됨을 의미.
			Reserved = 15 //예약됨..
		};

		enum class CINFOType : uint8_t{
			SlidingWindow_Size32KB = 7 //사용할 슬라이딩 윈도우의 크기가 32kb임을 나타내는 값.
		};

		struct {
			CMType    cm    : 4; //일반적으로 8==deflate다. 다른값은 에러가 발생해야 한다..
			CINFOType cinfo : 4; //사용한 슬라이딩 윈도우의 크기를 나타냄. cm이 8이면 7( 32k )
		};

		uint8_t data;
	};



	/************************************************
	 *    Zlib의 엑스트라 플래그들을 확인하기 위한
	 *    1 bytes 크기의 유니온입니다....
	 ******/
	union ExtraFlag 
	{
		struct{
			uint8_t fCheck : 5; //CMF&FLG가 부호가 없는 16bit 인트로 31의 배수임을 확인하기 위해 있는 값...
			uint8_t fdict  : 1; //사전 설정된 Dictionary가 있는지 여부. 설정된 경우 FLG 직후 32bits의 사전 데이터를 읽어들여야 한다. (왠만해서는 없음.)
			uint8_t flevel : 2; //압축 강도( 정수값 0~3. 각각 가장빠른/빠른/기본값/최대 압축 수준을 나타낸다. )
		};

		uint8_t data;
	};



	/****************************************************
	 *   Zlib로 압축된 파일의 구조를 나타내는 구조체입니다...
	 *******/
	struct Data 
	{
		CompressionMethod    CompressionMethod;
		ExtraFlag		     Extra_flags;
		uint32_t		     fdict;
		std::vector<uint8_t> DeflateBlockData;
		uint32_t             checkSum_alder32;
	};



	/*************************************************
	 *   동적 허프만 코딩으로 압축된 블럭의 코드 길이
	 *   테이블을 압축한 허프만 코딩의 압축을 해제하는데
	 *   사용되는 코드 길이들의 값을, 발생 빈도별로
	 *   정리해놓은 룩업 테이블입니다....
	 *******/
	constexpr static inline uint32_t CodeLengthCodesOrder[] = {
		16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
	};



	/******************************************************
	 *    LZ77 과 관련된 요소들을 제공하는 구조체입니다.
	 *******/
	struct LZ77 
	{
		struct LengthCodePair {
			uint32_t extra_bits;   
			uint32_t length_start;
		};

		struct DistCodePair {
			uint32_t extra_bits;
			uint32_t length_start;
		};

		constexpr static inline LengthCodePair LengthCodeTable[] = {
			{0,3},{0,4},{0,5},{0,6},{0,7},{0,8},{0,9},{0,10},{1,11},{1,13},		   /** 257 ~ 266 */
			{1,15},{1,17},{2,19},{2,23},{2,27},{2,31},{3,35},{3,43},{3,51},{3,59}, /** 267 ~ 276 */
			{4,67},{4,83},{4,99},{4,115},{5,131},{5,163},{5,195},{5,227},{0,258}   /** 277 ~ 285 */
		};

		constexpr static inline DistCodePair DistCodeTable[] = {
			{0,1},{0,2},{0,3},{0,4},{1,5},{1,7},{2,9},{2,13},{3,17},{3,25},										 /** 0 ~ 9 */
			{4,33},{4,49},{5,65},{5,97},{6,129},{6,193},{7,257},{7,385},{8,513},{8,769},						 /**10 ~ 19 */
			{9,1025},{9,1537},{10,2049},{10,3073},{11,4097},{11,6145},{12,8193},{12,12289},{13,16385},{13,24577} /** 20 ~ 29 */
		};
	};



	/*****************************************
	 *   Zlib 압축 해제 결과를 담는 구조체입니다..
	 ******/
	struct InflateResult
	{
		bool Success					 : 1;
		bool CMIsNotDeflate				 : 1;
		bool CMF_Or_FLG_ValueIsCorrupted : 1;
		bool UsePresetDictionary		 : 1;
		bool Invalid_NoCompresionBlock   : 1;
	};




	//============================================================================================================
	///////////										Public methods..									//////////
	//============================================================================================================
public:
	static InflateResult Inflate(ByteStream& inZlibDeflateStream, std::vector<uint8_t>& outInflateStream);




	//==========================================================================================================
	///////////								     Private methods..									////////////
	//==========================================================================================================
private:
	static void Inflate_LZ77(BitStream& bitStream, std::vector<uint8_t>& outInflateStream, const Zlib::HuffmanTree& literal_length_code_tree, const Zlib::HuffmanTree& distance_code_tree);

};












/*====================================================================================================
 *     Zlib 알고리즘 전용 허프만 트리가 구현된 클래스입니다.....
  *=========*/
class hyunwoo::Zlib::HuffmanTree final
{
	//==============================================================
	//////////					Defines..				 ///////////
	//==============================================================
public:
	/****************************************
	 *   허프만 트리의 노드를 나타내는 구조체..
	 *****/
	struct Node
	{
		int32_t  Left;
		int32_t  Right;
		uint32_t Symbol;
	};


	/*******************************************
	 *  허프만 트리의 심볼 종류를 나타내는 열거형..
	 *****/
	enum class SymbolType
	{
		Literal_Length,
		Distance
	};



	//================================================================
	//////////					 Fields...					//////////
	//================================================================
private:
	std::vector<Node> m_nodeList;




	//=================================================================
	//////////				   Public methods..				///////////
	//=================================================================
public:
	HuffmanTree() {};
	HuffmanTree(const HuffmanTree&) = delete;
	~HuffmanTree() = default;

	void Build_Dynamic(const uint32_t* code_length_table_ptr, uint32_t code_length_table_len);
	void Build_Fixed(const SymbolType symbolType);

	uint32_t GetSymbol(BitStream& bitStream) const;
	uint32_t GetNodeCount() const;



	//=====================================================================
	/////////					Private methods..				///////////
	//====================================================================
private:
	void InsertNode(uint32_t symbol, uint32_t code, uint32_t code_length);
};