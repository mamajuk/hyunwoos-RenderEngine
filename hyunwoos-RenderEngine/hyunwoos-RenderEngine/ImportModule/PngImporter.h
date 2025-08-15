#pragma once
#include <cstdInt>

namespace hyunwoo {
	class PngImporter;
}



/*==============================================================================
 *    Png파일을 로드하는 기능이 구현된 클래스입니다....
 *=========*/
class hyunwoo::PngImporter final
{
	//=====================================================
	///////					Defines...			    ///////
	//=====================================================

	/************************************
	 *   Png 파일의 매직해더를 나타내는 
	     정수값입니다...
	 ******/
	constexpr static const uint64_t Signature = 0x0a1a0a0d474e5089; //"덽NG\r\n\x1a\n"


	/*************************************
	 *   Png파일의 임포트 결과를 보관하는 
	 *   구조체입니다..
	 ********/
	struct ImportResult
	{
		bool Success		   : 1;
		bool Failed_OpenFile   : 1;
		bool Invalid_Signature : 1;
		bool Invalid_File	   : 1;
	};


	/************************************
	 *   Png파일의 청크 종류를 나타내는
	 *   열거형입니다...
	 *******/
	enum class ChunkType : int32_t
	{
		IHDR = 'RDHI',
		IDAT = 'TADI',
		IEND = 'DNEI',
		sRGB = 'BGRs',
		gAMA = 'AMAg',
		pHYs = 'sYHp',
		iCCP = 'PCCi',
		fdEC = 'CEdf',
		tIME = 'EMIt',
		PLTE = 'ETLP'
	};


	/***************************************
	 *   해당 Png파일이 사용하는 컬러 종류를
	 *   나타내는 열거형입니다....
	 ******/
	enum class ColorType : int8_t
	{
		GrayScale		   = 0,
		TrueColor		   = 2,
		Indexed_Color	   = 3,
		GrayScaleWithAlpha = 4,
		TrueColorWithAlpha = 6
	};

	enum class InterlaceMethod : int8_t
	{
		No_Interlace   = 0,
		Adam7Interlace = 1
	};

	enum class CompressionMethod : int8_t
	{
		Deflate = 0
	};

	enum class FilterMethod : int8_t
	{
		Adative = 0
	};

	enum class FilterType : int8_t
	{
		None    = 0,
		Sub     = 1,
		Up		= 2,
		Average = 3,
		Paeth   = 4
	};

	enum class BlockType : int32_t
	{
		IsLast = 0b1,
	};

	struct ChunkHeader
	{
		uint32_t   Length;
		ChunkType  Type;
		uint32_t   crc32;
	};

	struct Chunk_IHDR
	{
		uint32_t	      width;
		uint32_t		  height;
		uint8_t		      bitDepth;
		ColorType         colorType;
		CompressionMethod compressionMethod;
		FilterMethod      filterMethod;
		InterlaceMethod   InterlaceMethod;
	};

	struct RGB
	{
		int8_t r, g, b;
	};

};
