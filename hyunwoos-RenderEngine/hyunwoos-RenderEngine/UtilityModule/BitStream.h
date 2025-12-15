#pragma once
#include <cstdint>
#include "ByteStream.h"

namespace hyunwoo {
	class BitStream;
}



/*=======================================================================================================================================================================================
 *   바이트 스트림에서 리틀엔디안(LE)순의 비트 단위로 읽을 수 있도록 해주는 유틸리티 클래스입니다...
 *===============*/
class hyunwoo::BitStream final
{
	//========================================================================================================
	////////										Properties..									 /////////
	//========================================================================================================
public:
	uint32_t    BitStreamValue  = 0;
	uint32_t    BitLeft		    = 0;
	ByteStream& ByteStreamRef;



	//=======================================================================================================
	////////									  Public methods...									/////////
	//======================================================================================================
public:
	BitStream(ByteStream& byteStreamRef, uint32_t byteTotal);
	BitStream(const BitStream&) = default;
	~BitStream()				= default;

	uint32_t ReadBits(uint32_t readBitCount);
	void     MoveOffsetToByteBoundary();
};