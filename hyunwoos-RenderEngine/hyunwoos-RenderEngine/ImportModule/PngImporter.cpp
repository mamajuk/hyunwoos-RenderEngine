#include "PngImporter.h"
#include "../UtilityModule/Zlib.h"
#include <fstream>

/*================================================================================================================================================
 *   지정한 주소의 Png파일로부터, 텍스쳐 데이터를 읽어들입니다....
 *============*/
hyunwoo::PngImporter::ImportResult hyunwoo::PngImporter::Import(Texture2D& outTexture, const std::wstring& path)
{
	ImportResult outRet = { 0, };

	/**************************************************************************************
	 *   주어진 주소의 파일이 유효한가?
	 *******/
	std::ifstream in(path, std::ios::binary);

	/*----------------------------------
	 *   유효하지 않다면, 결과를 갱신하고
	 *   함수를 종료한다...
	 *****/
	if (in.is_open() == false) {
		outRet.Failed_OpenFile = true;
		return outRet;
	}



	/**************************************************************************************
	 *   올바른 PNG파일의 시그니처를 구성하고 있는가?
	 ********/
	uint64_t signature;
	in.read((char*)&signature, 8);

	/*---------------------------------
	 *  유효하지 않다면, 결과를 갱신하고
	 *  함수를 종료한다...
	 *-----*/
	if (signature != PngImporter::Signature) {
		outRet.Invalid_Signature = true;
		return outRet;
	}



	/***************************************************************************************
	 *   시그니처 이후, 등장하는 청크들을 하나씩 읽어들이고, 청크 타입에 따라 적절히 처리한다.
	 *   Png에서 등장하는 정수값들은 모두 빅엔디안을 사용한다. 이는 Png가 네트워크 전송을 위해 사용
	 *   되는 확장명이기 때문. 따라서 1bytes보다 큰 정수값을 정상적으로 읽기 위해선 리틀 엔디안으로 
	 *   순서를 재배치해야 한다.
	 *********/
	Chunk				 chunk;
	IndexedRGB			 plteData[256];
	IHDR_Data			 ihdrData;
	std::vector<uint8_t> deflate_img_stream;

	while (in)
	{
	  /*+-------------------------------------------------------+
		|				The Structure of Png Chunk				|
		+-------------+-----------------------------------------+
		|  Length(4)  | 해당 청크의 Data 영역의 바이트 수를 나타내는 |
		|			  | 부호가 없는 정수값이다. 값이 0이여도 올바른  |
		|			  | 값이다. ( 길이가 0인 청크도 존재하기 때문. ) |
		+-------------+-----------------------------------------+
		|  Type(4)    | 해당 청크의 유형을 아타내는 4bytes의 문자열  |
		|			  | 이다. e.g: IHDR 청크라면 ['I','H','D','R']|
		|			  | 순으로 데이터 스트림에 저장되어 있을 것이다.	|
		+-------------+-----------------------------------------+
		| Data(Length)| 해당 청크가 가진 데이터들이 실제로 들어있는   |
		|			  | 필드로, Length 필드값의 바이트 수만큼 존재  |
		|			  | 한다. Length 필드가 0이라면 무시하면 된다.  |
		+-------------+-----------------------------------------+
		| CRC(4)	  | 해당 청크의 Data가 손상되었는지 확인하는데   |
		|			  | 필요한 4bytes 값이다.						|
		+-------------------------------------------------------+*/
		in.read((char*)&chunk, 8);
		chunk.Length = SwapBytesOrder(chunk.Length);



		/*+-------------------------------------------------------+
		  |						 IHDR Chunk..				      |
		  +-------------------------------------------------------+
		  |  IHDR 청크는, 청크들 중에서 가장 먼저 등장하는 Critical Chu|
		  |  nk( 필수로 처리해야하는 청크 )로, Png 이미지가 어떻게 구성 |
		  |  되어 있는지의 정보들이 들어있다.					      |
		  +----------------+--------------------------------------+
		  |  Width(4)	   | 이미지의 너비값을 나타내는 4bytes 부호   |
		  |				   | 없는 정수값.							  |
		  +----------------+--------------------------------------+
		  |  Height(4)	   | 이미지의 높이값을 나타내는 4bytes 부호 없|
		  |				   | 는 정수값.							  |
		  +----------------+--------------------------------------+
		  |  Bit Depth(1)  | 이미지에서 사용되는 색상을 구성하는 채널이|
		  |				   | 몇 비트를 사용하는지를 나타내는 1bytes   |
		  |				   | 부호 없는 정수값. e.g: Bit Depth = 8이 |
		  |				   | 라면, RGB 색상의 각 채널이 8bits를 사용 |
		  |				   | 한다는 것. Color Type에 따라서 정해진 값|
		  |				   | 을 가진다.							  |
		  +----------------+--------------------------------------+
		  |  Color Type(1) | Png 이미지 유횽을 정의하는 1bytes 정수값 |
		  |				   | . 이 값을 통해서 해당 이미지 픽셀들의    |
		  |				   | 색상 유횽을 결정한다.					  |
		  +----------------+--------------------------------------+
		  |  Compression   | 이미지 데이터를 압축하는데 사용된 방법을  |
		  |  Method (1)	   | 나타내는 1bytes 정수값. 항상 0~32KB의   |
		  |				   | 슬라이딩 윈도우를 갖는 압축 방식인 Deflat|
		  |				   | e를 나타내는 값 0을 가진다. (이 값이 0이 |
		  |				   | 아니라면 잘못된 값인 것이다.			  |
		  +----------------+--------------------------------------+
		  | FilterMethod(1)| 이미지 데이터의 Zlib(Deflate) 압축을 더 |
		  |				   | 효율적으로 하기 위해서, Zlib 압축전에 적용|
		  |				   | 되는 필터링 방식을 나타내는 1bytes 정수값|
		  |				   | . 항상 5개의 필터링 방식을 사용하는 Adati|
		  |				   | ve를 나타내는 0을 가진다.				  |
		  +----------------+--------------------------------------+
		  |  Interlace     | 인터레이스 여부를 나타내는 1bytes 정수값.|
		  |  Method(1)	   | 오직 인터레이스 없음(0) 또는 Adam7 인터  |
		  |				   | 레이스(1) 두 개의 값만 정의되어 있다.    |
		  |				   | 이외의 값이면 오류인 것.				  |
		  +-------------------------------------------------------+*/
		if (chunk.Type == ChunkType::IHDR)
		{
			in.read((char*)&ihdrData, chunk.Length);

			ihdrData.Width  = SwapBytesOrder(ihdrData.Width);
			ihdrData.Height = SwapBytesOrder(ihdrData.Height);

			//압축방법이 유효하지 않다면, 결과를 갱신하고 함수를 종료한다....
			if (ihdrData.CompressionMethod != CompressionMethod::Deflate) {
				outRet.CompressionMethodIsNotDeflate = true;
				return outRet;
			}

			//필터 방법이 유효하지 않다면, 결과를 갱신하고 함수를 종료한다...
			if (ihdrData.FilterMethod != FilterMethod::Adative) {
				outRet.FilterMethodIsNotAdative = true;
				return outRet;
			}

			//인터레이스 값이 잘못되어 있다면, 경과를 갱신하고 함수를 종료한다..
			const bool isNoInterlace      = (ihdrData.InterlaceMethod != InterlaceMethod::No_Interlace);
			const bool isNoAdam7Interlace = (ihdrData.InterlaceMethod != InterlaceMethod::Adam7Interlace);

			if (isNoInterlace && isNoAdam7Interlace) {
				outRet.Invalid_Interlace = true;
				return outRet;
			}
		}



		/*+-------------------------------------------------------+
		  |						 IDAT Chunk..				      |
		  +-------------------------------------------------------+
		  |  IDAT 청크는 Critical Chunk로, Adative 방식의 '필터링'과 |
		  |  Deflate 방식의 '압축'을 차례대로 거친 실제 이미지 데이터가 |
		  |  저장되어 있는 청크다.									  |
		  |													      |
		  |  IDAT 청크는 연속적으로 여러개가 등장할 수도 있는데, 이 경우 |
		  |  Deflate로 압축된 필터링 데이터가, 여러개의 IDAT 청크들의   |
		  |  데이터에 쪼개져서 들어가 있음을 의미한다.				  |
		  |														  |
		  |  따라서 먼저 모든 IDAT 청크들의 데이터를 하나의 스트림에 모아|
		  |  서, 앞서 설명한 Deflate '압축'과 Adative '필터링'을 순서 |
		  |  대로 해제하면, 원본 이미지 데이터를 얻을 수 있다.	      |
		  +-------------------------------------------------------+*/
		else if (chunk.Type == ChunkType::IDAT)
		{
			const uint32_t prev_size = deflate_img_stream.size();

			deflate_img_stream.resize(prev_size + chunk.Length);
			in.read((char*)&deflate_img_stream[prev_size], chunk.Length);
		}



		/*+-------------------------------------------------------+
		  |						 IEND Chunk..				      |
		  +-------------------------------------------------------+
		  |  청크의 끝을 알리기 위한 Critical Chunk. 해당 청크가 등장  |
		  |  했다면, 모든 청크들을 읽은 것이니, 반복문을 종료시키면 된다.|
		  |  단순한 용도이기 때문에, Data의 바이트 수는 0이다.		  |
		  +-------------------------------------------------------+*/
		else if (chunk.Type == ChunkType::IEND)
		{
			break;
		}



		/*+-------------------------------------------------------+
		  |						 PLTE Chunk..				      |
		  +-------------------------------------------------------+
		  |  Png ColorType이 Indexed Color(3)일 때 존재하는 Ancillary|
		  |  Chunk( 필수로 처리할 필요없이 무시해도 되는 청크 )다.      |
		  |  이미지 데이터의 각 픽셀값들은 PLTE 테이블의 인덱스값이며,   |
		  |  PLTE 청크는 이 테이블의 데이터가 담겨 있다..			  |
		  +-------------------------------------------------------+*/
		else if (chunk.Type == ChunkType::PLTE)
		{
			in.read((char*)plteData, chunk.Length);
		}



		/*+-------------------------------------------------------+
		  |						Other Chunk..				      |
		  +-------------------------------------------------------+
		  |  Critical Chunk들과 일부 청크들을 제외한 나머지 청크들은   |
		  |  처리하지 않고 넘어간다..								  |
		  +-------------------------------------------------------+*/
		else
		{
			in.seekg(chunk.Length, std::ios::cur);
		}



		/*------------------------------------------------------
		 *   청크 데이터의 손상 여부를 확인하는데 필요한 CRC값을
		 *   읽어들인다...
		 *------*/
		in.seekg(4, std::ios::cur);
	}




	/*******************************************************************************************
	 *   온전한 압축 이미지 데이터의 Deflate 압축을 해제한다....
	 *******/
	std::vector<uint8_t> filtered_stream;
	if ((outRet.ZlibInflateRet = Zlib::Inflate(deflate_img_stream, filtered_stream)).Success==false) {
		outRet.Failed_Deflate = true;
		return outRet;
	}


	/*----------------------------------------------
	 *   Deflate 압축을 해제한 데이터가 올바른가?
	 *------*/
	const uint32_t bitsPerPixel		  = (ihdrData.BitDepth * GetColorDimension(ihdrData.ColorType));
	const uint32_t bytesPerPixel	  = (bitsPerPixel+7) / 8;
	const uint32_t correct_data_size  = (ihdrData.Height * (8 + ihdrData.Width * bitsPerPixel))/8;

	//올바르지 않다면, 결과를 갱신하고 함수를 종료한다..
	if (filtered_stream.size() != correct_data_size) {
		outRet.Failed_Deflate = true;
		return outRet;
	}


	if (ihdrData.BitDepth<8) {
		int i = 0;
	}


	/****************************************************************************************
	 *   필터링을 해제한다. 필터링은 스캔라인 단위로 적용되며, 이미지 데이터의 각 스캔라인(이미지 픽셀
	 *   의 한 행을 의미)마다 적용된 필터링 방법이 모두 다르다. 그렇기에 각 스캔라인이 시작하기 전에
	 *   해당 스캔라인에 적용된 필터링 방법을 나타내는 1bytes가 먼저 등장한다..
	 *********/
	const int32_t		 scanLineStride = (ihdrData.Width * bitsPerPixel) / 8;
	std::vector<uint8_t> defiltered_stream;

	defiltered_stream.reserve(ihdrData.Width * ihdrData.Height);


	/*-----------------------------------------------
	 *   필터링을 해제하기 위한 람다 함수들을 정의한다..
	 *-----*/
	const auto Recon_a = [&](int32_t row, int32_t col) -> int32_t {
		return (col >= bytesPerPixel ? defiltered_stream[(row*scanLineStride) + col - bytesPerPixel] : 0);
	};

	const auto Recon_b = [&](int32_t row, int32_t col)->int32_t {
		return (row > 0 ? defiltered_stream[(row-1) * scanLineStride + col] : 0);
	};

	const auto Recon_c = [&](int32_t row, int32_t col)->int32_t {
		return (row > 0 && col>=bytesPerPixel? defiltered_stream[(row-1) * scanLineStride + col - bytesPerPixel]:0);
	};

	const auto PaethPredictor = [&](int32_t a, int32_t b, int32_t c) -> int32_t {
		int32_t p  = (a + b - c);
		int32_t pa = Math::Abs(p - a);
		int32_t pb = Math::Abs(p - b);
		int32_t pc = Math::Abs(p - c);

		if (pa <= pb && pa <= pc) {
			return a;
		}

		if (pb <= pc) {
			return b;
		}

		return c;
	};


	/*-----------------------------------------------------------
	 *   각 스캔라인 앞에 등장하는 필터링 방식(1bytes)를 읽어들이고,
	 *   이에 따른 적절한 방법으로 스캔라인을 구성하는 값들의 필터링을
	 *   해제하여 필터링이 해제된 데이터 스트림에 넣는다...
	 *-------*/
	uint32_t idx = 0;
	for (uint32_t scanLine = 0; scanLine < ihdrData.Height; scanLine++){
		FilterType filter_type = (FilterType)filtered_stream[idx++];


		for (uint32_t x = 0; x < scanLineStride; x++) {

			uint32_t filt_x  = filtered_stream[idx++];
			uint32_t recon_x = 0;
			
			switch (filter_type)
			{
				//필터링 없음. 원본 바이트 그대로인 바이트...
				case(FilterType::None): {
					recon_x = filt_x;
					break;
				}

				//현재 바이트에서 같은 행의 이전 픽셀(byte) 값을 빼서 저장된 필터링 방식.
				case(FilterType::Sub): {
					recon_x = filt_x + Recon_a(scanLine, x);
					break;
				}

				//현재 바이트에서 바로 이전 행의 픽셀(byte) 값을 빼서 저장된 필터링 방식.
				case(FilterType::Up): {
					recon_x = filt_x + Recon_b(scanLine, x);
					break;
				}

				//현재 바이트에서 왼쪽 픽셀과, 위쪽 픽셀의 평균값을 기준으로 차이가 저장된 필터링 방식.
				case(FilterType::Average): {
					recon_x = filt_x + Math::Floor((Recon_a(scanLine,x) + Recon_b(scanLine,x)) / 2);
					break;
				}

				/*현재 바이트에서 왼쪽, 위쪽, 왼쪽-위, 대각선 값 중에서 실제 픽셀 값과 가장 가까운
				  예측값을 골라 차이값이 저장된 필터링 방식.*/
				case(FilterType::Paeth): {
					recon_x = filt_x + PaethPredictor(Recon_a(scanLine, x), Recon_b(scanLine, x), Recon_c(scanLine, x));
					break;
				}

				/**잘못된 필터링 방법일 경우, 결과를 갱신하고 함수를 종료한다...**/
				default: {
					outRet.Invalid_FilterType = true;
					return outRet;
				}
			}

			defiltered_stream.push_back(uint8_t(recon_x));
		}
	}


	/********************************************************************************************
	 *    필터링이 해제된 스트림은, 하나의 픽셀을 구성하는 색상 채널들의 스트림이라고 생각하면 된다.
	 *    해당 이미지에서 사용된 ColorType값을 기반으로, 하나의 픽셀을 구성하는 색상 채널 개수만큼 
	 *    필터링이 해제된 스트림으로부터 색상 채널들을 읽어들이고 하나의 LinearColor로 묶어 출력 텍스쳐의
	 *    픽셀 스트림에 넣는다...
	 **********/
	idx			      = 0;
	outTexture.Width  = ihdrData.Width;
	outTexture.Height = ihdrData.Height;

	outTexture.Pixels.clear();
	outTexture.Pixels.reserve(ihdrData.Width * ihdrData.Height);


	while (idx < defiltered_stream.size()) 
	{
		switch (ihdrData.ColorType) {

		  /*+------------------------------------------------------+
			|				   Gray Scale(1bytes)				   |
			+-----------------+------------------------------------+
			|   ColorType: 0  |        BitDepth: 1,2,4,8,16		   |
			+-----------------+------------------------------------+
			|  각 픽셀을 구성하는 색상 채널은 그레이스케일( 흰색 ~ 검정 ) |
			|  값 하나로 구성된다..     							   |
			+------------------------------------------------------+*/
			case(ColorType::GrayScale): 
			{
				const DWORD colorChannel_G = (defiltered_stream[idx++]);

				outTexture.Pixels.push_back(Color(
					colorChannel_G,
					colorChannel_G,
					colorChannel_G,
					255
				));
				break;
			}



			/*+------------------------------------------------------+
			  |				  Gray Scale With Alphh(2bytes)			 |
			  +-----------------+------------------------------------+
			  |   ColorType: 4  |        BitDepth: 8, 16		     |
			  +-----------------+------------------------------------+
			  |  각 픽셀을 구성하는 색상 채널은, 그레이스케일값과 알파값으로 |
			  |  구성된다.										     |
			  +------------------------------------------------------+*/
			case(ColorType::GrayScaleWithAlpha):
			{
				const DWORD colorChannel_G = (defiltered_stream[idx++]);
				const DWORD colorChannel_A = (defiltered_stream[idx++]);

				outTexture.Pixels.push_back(Color(
					colorChannel_G,
					colorChannel_G,
					colorChannel_G,
					colorChannel_A
				));
				break;
			}



			/*+------------------------------------------------------+
			  |				    Indexed Color(3bytes)	    		 |
			  +-----------------+------------------------------------+
			  |   ColorType: 3  |        BitDepth: 1,2,4,8		     |
			  +-----------------+------------------------------------+
			  |  각 픽셀을 구성하는 색상 채널이 하나만 존재한다. 이 색상    |
			  |  값은 PLTE 청크로부터 읽어들인 RGB 색상 룩업테이블의 인덱스 |
			  |  값이다.												 |
			  +------------------------------------------------------+*/
			case(ColorType::Indexed_Color):
			{
				const uint32_t plteIdx	   = defiltered_stream[idx++];
				const DWORD colorChannel_R = (plteData[plteIdx].r);
				const DWORD colorChannel_G = (plteData[plteIdx].g);
				const DWORD colorChannel_B = (plteData[plteIdx].b);

				outTexture.Pixels.push_back(Color(
					colorChannel_R,
					colorChannel_G,
					colorChannel_B,
					255
				));
				break;
			}



			/*+------------------------------------------------------+
			  |				     True Color(3bytes)	    		     |
			  +-----------------+------------------------------------+
			  |   ColorType: 2  |        BitDepth: 8,16 		     |
			  +-----------------+------------------------------------+
			  |  각 픽셀을 구성하는 색상 채널은 R,G,B 3개로 구성된다.		 |
			  +------------------------------------------------------+*/
			case(ColorType::TrueColor):
			{
				const DWORD colorChannel_R = (defiltered_stream[idx++]);
				const DWORD colorChannel_G = (defiltered_stream[idx++]);
				const DWORD colorChannel_B = (defiltered_stream[idx++]);

				outTexture.Pixels.push_back(Color(
					colorChannel_R,
					colorChannel_G,
					colorChannel_B,
					255
				));
				break;
			}



			/*+------------------------------------------------------+
			  |				 True Color With Alpha(4bytes)	    	 |
			  +-----------------+------------------------------------+
			  |   ColorType: 2  |        BitDepth: 8,16 		     |
			  +-----------------+------------------------------------+
			  |  각 픽셀을 구성하는 색상 채널은 R,G,B,A 4개로 구성된다.    |
			  +------------------------------------------------------+*/
			case(ColorType::TrueColorWithAlpha):
			{
				const DWORD colorChannel_R = (defiltered_stream[idx++]);
				const DWORD colorChannel_G = (defiltered_stream[idx++]);
				const DWORD colorChannel_B = (defiltered_stream[idx++]);
				const DWORD colorChannel_A = (defiltered_stream[idx++]);

				outTexture.Pixels.push_back(Color(
					colorChannel_R,
					colorChannel_G,
					colorChannel_B,
					colorChannel_A
				));
				break;
			}
		}
	}

	outRet.Success = true;
	outRet.ImportCount++;
	return outRet;
}












/*=======================================================================================================================
 *   주어진 모든 경로들로부터 텍스쳐를 읽어들입니다....
 *==============*/
hyunwoo::PngImporter::ImportResult hyunwoo::PngImporter::Imports(std::vector<Texture2D>& outTextures, std::initializer_list<const wchar_t*> paths)
{
	ImportResult outRet = { 0, };

	outTextures.clear();
	outTextures.resize(paths.size());

	uint32_t tex_idx = 0;
	for (const auto path : paths)
	{
		uint32_t prev_importCount = outRet.ImportCount;

		outRet = Import(outTextures[tex_idx++], path);
		outRet.ImportCount += prev_importCount;

		if (outRet.Success == false) {
			return outRet;
		}
	}
	return outRet;
}












/*===========================================================================================================
 *    주어진 인자로 받은 정수값의 바이트 순서를 바꿉니다.....
 *=========*/
uint32_t hyunwoo::PngImporter::SwapBytesOrder(uint32_t value)
{
	value = ((value << 8) & 0xFF00FF00) | ((value >> 8) & 0xFF00FF);
	value = ((value << 16) | (value >> 16));
	return value;
}









/*===============================================================================================================
 *   주어진 컬러값과 대응되는 색
 *==========*/
uint32_t hyunwoo::PngImporter::GetColorDimension(ColorType colorType)
{
	switch (colorType) 
	{
		case(ColorType::GrayScale): {
			return 1;
		}

		case(ColorType::GrayScaleWithAlpha): {
			return 2;
		}

		case(ColorType::Indexed_Color): {
			return 1;
		}

		case(ColorType::TrueColor): {
			return 3;
		}

		case(ColorType::TrueColorWithAlpha): {
			return 4;
		}
	}

	return 0;
}
