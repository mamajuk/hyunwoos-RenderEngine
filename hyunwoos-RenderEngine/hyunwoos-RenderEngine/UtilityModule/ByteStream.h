#pragma once
#include <vector>
#include <fstream>
#include <cstdint>

namespace hyunwoo {
	class ByteStream;
	class ConsecutiveByteStream;
	class FileSpanByteStream;
}



/*=======================================================================================================================================================================================
 *   모든 바이트 스트림들의 기반 클래스입니다....
 *===============*/
class hyunwoo::ByteStream
{
	//=======================================================================================================
	////////									 Virtual methods...									/////////
	//======================================================================================================
public:
	virtual void ReadBytes(char* outPtr, uint32_t readBytesCount) = 0;
	virtual void MoveOffset(uint32_t offset)					  = 0;

	virtual uint32_t GetBytesLeft() const      = 0;
	virtual uint32_t GetCurrOffset() const     = 0;
	virtual uint32_t GetTotalByteCount() const = 0;
};








/*=======================================================================================================================================================================================
 *   일반 배열처럼 작동하는 바이트 스트림 클래스입니다...
 *===============*/
class hyunwoo::ConsecutiveByteStream final : public hyunwoo::ByteStream
{
	//=======================================================================================
	////////							   Fields..									/////////
	//=======================================================================================
private:
	char*    m_startPtr;
	uint32_t m_curOffset;
	uint32_t m_length;


	//===============================================================================================
	////////							Override methods...									/////////
	//===============================================================================================
public:
	virtual void ReadBytes(char* outPtr, uint32_t readBytesCount) override final;
	virtual void MoveOffset(uint32_t offset) override final;

	virtual uint32_t GetBytesLeft() const override final;
	virtual uint32_t GetCurrOffset() const override final;
	virtual uint32_t GetTotalByteCount() const override final;



	//===============================================================================================
	////////							   Public methods..									/////////
	//===============================================================================================
	ConsecutiveByteStream(char* startPtr, uint32_t length) :m_startPtr(startPtr), m_curOffset(0), m_length(length) {};
	ConsecutiveByteStream(const ConsecutiveByteStream&) = default;
	~ConsecutiveByteStream()							= default;
};









/*=======================================================================================================================================================================================
 *   파일 스트림의 여러 영역들을 하나의 연속적인 메모리처럼 다루는 바이트 스트림 클래스입니다....
 *===============*/
class hyunwoo::FileSpanByteStream final : public hyunwoo::ByteStream
{
	//=======================================================================================
	////////							   Define..									/////////
	//=======================================================================================
private:
	/*********************************************
	 *  파일 스트림의 특정 영역을 나타내는 구조체...
	 ******/
	struct FileSpan final
	{
		std::streampos StartStreamPos;
		uint32_t	   StartIdx;
		uint32_t	   Length;

		FileSpan(std::streampos& startStreamPos, uint32_t startIdx, uint32_t length):StartStreamPos(startStreamPos), StartIdx(startIdx), Length(length){}
	};

public:
	struct ReadOnlyFileSpan final
	{
		std::streampos StartPos;
		uint32_t	   Length;
	};



	//===========================================================================================
	////////							   Fields...									/////////
	//===========================================================================================
private:
	uint32_t			  m_curSpanIdx;
	uint32_t			  m_curOffset;
	uint32_t			  m_length;	
	std::ifstream&		  m_inIfstream;
	std::vector<FileSpan> m_spanList;



	//===============================================================================================
	////////							Override methods...									/////////
	//===============================================================================================
public:
	virtual void ReadBytes(char* outPtr, uint32_t readBytesCount) override final;
	virtual void MoveOffset(uint32_t offset) override final;

	virtual uint32_t GetBytesLeft() const override final;
	virtual uint32_t GetCurrOffset() const override final;
	virtual uint32_t GetTotalByteCount() const override final;



	//===============================================================================================
	////////							   Public methods..									/////////
	//===============================================================================================
	FileSpanByteStream(std::ifstream& in) :m_inIfstream(in), m_length(0), m_curOffset(0), m_curSpanIdx(0) {};
	FileSpanByteStream(const FileSpanByteStream&)     = delete;
	FileSpanByteStream(FileSpanByteStream&&) noexcept = delete;
	~FileSpanByteStream()							  = default;


	/******************************************
	 *  FileSpan 관련 메소드들...
	 ******/
	void ClearFileSpans();

	void				   AddFileSpan(std::streampos pos, uint32_t length);
	const ReadOnlyFileSpan GetFileSpanAt(uint32_t idx) const;
};