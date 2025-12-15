#include "ByteStream.h"
#include "../MathModule/Math.h"


/***************************************************************************
 *   ConsecutiveByteStream 관련 메소드들의 정의....
 *******/

/*=======================================================================================================================================================
 *  주어진 인자 readBytesCount만큼, 현재 오프셋에서 바이트를 읽어들이고 오프셋을 옮깁니다. 읽어들인 결과는 주어진 인자 outPtr에 담깁니다....
 **********/
void hyunwoo::ConsecutiveByteStream::ReadBytes(char* outPtr, uint32_t readBytesCount)
{
    readBytesCount = Math::Clamp(readBytesCount, 0, GetBytesLeft());
    memcpy(outPtr, (m_startPtr + m_curOffset), readBytesCount);

    m_curOffset += readBytesCount;
}


/*=======================================================================================================================================================
 *  주어진 인자 offset으로 현재 오프셋을 이동시킵니다...
 **********/
void hyunwoo::ConsecutiveByteStream::MoveOffset(uint32_t offset)
{
    m_curOffset = Math::Clamp(offset, 0, m_length);
}


/*=======================================================================================================================================================
 *  현재 오프셋에서 바이트 스트림의 끝까지 오프셋이 얼마나 남았는지를 나타내는 정수값을 얻습니다....
 **********/
uint32_t hyunwoo::ConsecutiveByteStream::GetBytesLeft() const
{
    return (m_length - m_curOffset);
}


/*=======================================================================================================================================================
 *  현재 오프셋을 얻습니다....
 **********/
uint32_t hyunwoo::ConsecutiveByteStream::GetCurrOffset() const
{
    return m_curOffset;
}


/*=======================================================================================================================================================
 *  해당 바이트 스트림의 전체 바이트 수를 얻습니다....
 **********/
uint32_t hyunwoo::ConsecutiveByteStream::GetTotalByteCount() const
{
    return m_length;
}
















/***************************************************************************
 *   FileSpanByteStream 관련 메소드들의 정의....
 *******/

 /*=======================================================================================================================================================
  *  주어진 인자 readBytesCount만큼, 현재 오프셋에서 바이트를 읽어들이고 오프셋을 옮깁니다. 읽어들인 결과는 주어진 인자 outPtr에 담깁니다....
  **********/
void hyunwoo::FileSpanByteStream::ReadBytes(char* outPtr, uint32_t readBytesCount)
{
    /***************************************************************
     *  모든 바이트를 읽어들일 때까지, spanList를 순차적으로 순회한다..
     *******/
    while(readBytesCount>0 && m_curSpanIdx < m_spanList.size() && m_curOffset < m_length) 
    {
        const FileSpan&      span       = m_spanList[m_curSpanIdx];
        const std::streampos cur_pos    = span.StartStreamPos + std::streamoff(m_curOffset - span.StartIdx);
        const uint32_t       spanEndIdx = (span.StartIdx + span.Length);
        const uint32_t       read_bytes = Math::Clamp(readBytesCount, 0, (spanEndIdx - m_curOffset));
        

        /*-------------------------------------------
         *  해당 span의 영역까지의 데이터를 읽어들이고,
         *  오프셋을 이동시킨다...
         ******/
        if (m_inIfstream.tellg() != cur_pos) {
            m_inIfstream.seekg(cur_pos);
        }

        m_inIfstream.read(outPtr, read_bytes);
        m_curOffset    += read_bytes;
        readBytesCount -= read_bytes;
        outPtr         += read_bytes;

        if (m_curOffset>=spanEndIdx) {
            m_curSpanIdx++;
        }
    }
}


/*=======================================================================================================================================================
 *  주어진 인자 offset으로 현재 오프셋을 이동시킵니다...
 **********/
void hyunwoo::FileSpanByteStream::MoveOffset(uint32_t offset)
{
    
}


/*=======================================================================================================================================================
 *  현재 오프셋에서 바이트 스트림의 끝까지 오프셋이 얼마나 남았는지를 나타내는 정수값을 얻습니다....
 **********/
uint32_t hyunwoo::FileSpanByteStream::GetBytesLeft() const
{
    return (m_length - m_curOffset);
}


/*=======================================================================================================================================================
 *  현재 오프셋을 얻습니다....
 **********/
uint32_t hyunwoo::FileSpanByteStream::GetCurrOffset() const
{
    return m_curOffset;
}


/*=======================================================================================================================================================
 *  해당 바이트 스트림의 전체 바이트 수를 얻습니다....
 **********/
uint32_t hyunwoo::FileSpanByteStream::GetTotalByteCount() const
{
    return m_length;
}


/*=======================================================================================================================================================
 *  설정된 FileSpan 리스트를 초기화합니다....
 **********/
void hyunwoo::FileSpanByteStream::ClearFileSpans()
{
    m_spanList.clear();
    m_length = 0;
}


/*=======================================================================================================================================================
 *  새로운 FileSpan을 추가합니다....
 **********/
void hyunwoo::FileSpanByteStream::AddFileSpan(std::streampos pos, uint32_t length)
{
    m_spanList.push_back(FileSpan(pos, m_length, length));
    m_length += length;
}


/*=======================================================================================================================================================
 *  주어진 인덱스에 위치한 FileSpan 객체의 포인터를 얻습니다.....
 **********/
const hyunwoo::FileSpanByteStream::ReadOnlyFileSpan hyunwoo::FileSpanByteStream::GetFileSpanAt(uint32_t index) const
{
   if (index < 0 || index >= m_spanList.size()) {
       return ReadOnlyFileSpan();
   }

   const FileSpan& span = m_spanList[index];
   return ReadOnlyFileSpan{ span.StartStreamPos, span.Length };
}
