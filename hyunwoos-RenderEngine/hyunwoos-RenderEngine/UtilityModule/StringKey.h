#pragma once
#include <string>
#include <unordered_map>
#include "StringLamda.h"
#include "../EngineModule/UniqueableObject.h"

namespace hyunwoo {
	class WStringKey;
}


/*=======================================================================================================================================================
 *    동일한 문자열을 고유한 키 값으로 다루는 클래스입니다....
 ***********/
class hyunwoo::WStringKey final
{
	friend class std::hash<WStringKey>;

	//=================================================================================================
	//////////////////							Defines..							///////////////////
	//=================================================================================================
private:
	/***********************************************************
	 *   참조 카운트를 가지는 std::wstirng의 warrper 구조체...
	 ********/
	struct RefWString final
	{
		uint32_t     RefCount = 0;
		size_t		 HashKey  = 0;
		std::wstring Str;

		RefWString() = default;
		RefWString(uint32_t refCount, std::wstring_view view) { Set(refCount, view); }

		void Set(uint32_t refCount, std::wstring_view view);
	};


	/***********************************************************
	 *   RefString의 인덱스를 가지는 구조체입니다...
	 ********/
	struct RefWStringView final
	{
		struct Hasher
		{
			std::size_t operator()(const RefWStringView& t) const noexcept;
		};

		struct Equaler
		{
			bool operator()(RefWStringView const& a, RefWStringView const& b) const noexcept;
		};

		uint32_t PoolIdx;
	};


	/***********************************************************
	 *   RefWStringView를 통해, pool에 있는 RefWString의
	 *   인덱스값을 얻어낼 수 있는 map의 별칭..
	 ********/
	using RefWStringMap = std::unordered_map<
		RefWStringView, uint32_t,
		RefWStringView::Hasher,
		RefWStringView::Equaler
	>;





	//=================================================================================================
	//////////////////							Fields..							///////////////////
	//=================================================================================================
private:
	static std::vector<RefWString> m_stringPools;
	static std::vector<uint32_t>   m_freeStrList;
	static RefWStringMap		   m_viewMap;

	uint32_t m_poolIdx = 0;




	//=====================================================================================================
	//////////////////					  Public methods..								///////////////////
	//=====================================================================================================
public:
	WStringKey() = default;
	WStringKey(const WStringKey& prev)	{ operator=(prev); }
	WStringKey(const wchar_t* rhs_view) { SetKey(std::wstring_view(rhs_view)); }
	~WStringKey()						{ Clear(); }


	void					Clear();
	const std::wstring_view GetView() const;

	void operator=(const wchar_t* rhs_view) { SetKey(std::wstring_view(rhs_view)); };
	void operator=(const WStringKey& prev);
	bool operator==(const WStringKey& rhs) const;
	bool operator!=(const WStringKey& rhs) const;




	//=====================================================================================================
	//////////////////					  Private methods..								///////////////////
	//=====================================================================================================
private:
	void SetKey(const std::wstring_view view);
};















/*=======================================================================================================================================================
 *    WStringKey에 대한 std::hash의 템플릿 특수화 버전입니다....
 ***********/
namespace std {

	template<>
	class hash<hyunwoo::WStringKey>
	{
	public:
		std::size_t operator()(const hyunwoo::WStringKey& key) const
		{
			return std::hash<uint32_t>()(key.m_poolIdx);
		}
	};
}