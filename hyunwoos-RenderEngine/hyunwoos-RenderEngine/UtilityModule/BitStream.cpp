#include "BitStream.h"

/*=========================================================================================================================================
 *    BitStream의 생성자입니다...
 *========*/
hyunwoo::BitStream::BitStream(uint8_t* byteStreamPtr, uint32_t byteTotal)
:ByteStreamPtr(byteStreamPtr), ByteLeft(byteTotal), BitLeft(0), BitStreamValue(0)
{
}






/*======================================================================================================================================
 *    비트 스트림 포인터로부터 지정한 수만큼의 비트를 읽고, 포인터를 이동시킵니다..
 *=======*/
uint32_t hyunwoo::BitStream::ReadBits(uint32_t readBitCount)
{
	uint32_t ret = 0;

	/***************************************************************
	 *   읽을 비트 수가 없다면, 결과를 갱신하고
	 *   함수를 종료한다....
	 ******/
	if (readBitCount<=0 || readBitCount>32 || ByteLeft==0 && BitLeft==0) {
		return 0;
	}


	/***************************************************************
	 *    비트 스트림에 남은 비트 수보다 많은 비트 수를 
	 *    요구하는가?
	 ******/
	uint32_t bit_filter = (0xFFFFFFFF >> (32 - readBitCount));

	if (BitLeft < readBitCount && ByteLeft>0) {

		uint32_t nxt_bits      = 0;
		uint32_t need_bytes    = (ByteLeft>=4? 4:ByteLeft);
		uint32_t nxt_read_bits;

		/*------------------------------------------------
		 *  바이트스트림으로부터 4bytes를 읽어들인다..
		 *  ( 남은 바이트가 4보다 작을 경우엔, 남은 바이트
		 *    만큼만 읽어들인다...)
		 ******/
		for (uint32_t i = 0; i < need_bytes; i++) {
			uint32_t Symbol = (*ByteStreamPtr++);
			nxt_bits |= Symbol<<(i*8);
		}


		/*-------------------------------------------------
		 *   새로 읽어들인 값에서 필요한 만큼의 비트를
		 *   기존 비트 스트림에 붙여서 결과에 담고,
		 *   남은 값으로 기존 비트 스트림을 대체한다..
		 ******/
		ret			   = (BitStreamValue | nxt_bits << BitLeft) & bit_filter;
		nxt_read_bits  = (readBitCount - BitLeft);
		BitStreamValue = (nxt_bits >> nxt_read_bits);
		BitLeft		   = (need_bytes * 8) - nxt_read_bits;
		ByteLeft	  -= need_bytes;
		
		return ret;
	}



	/***********************************************
	 *   비트 스트림의 남은 비트 수가 충분하다면,
	 *   요구한 만큼의 비트를 읽고, 결과에 담는다.
	 ********/
	ret				 = (BitStreamValue & bit_filter);
	BitStreamValue >>= readBitCount;
	BitLeft			-= readBitCount;

	return ret;
}





/*=======================================================================
 *    현재 비트스트림의 오프셋이, 바이트 경계에 맞아 떨어지도록 이동시킵니다...
 *==========*/
void hyunwoo::BitStream::MoveOffsetToByteBoundary()
{
	const uint32_t ShiftCount = (BitLeft % 8);
	BitLeft         -= ShiftCount;
	BitStreamValue >>= ShiftCount;
}
