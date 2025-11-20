#include "StringKey.h"

/******************************************************
 *  WStringKey에 대한 메소드들....
 ********/


 /*============================================================================================================================================================
  *   정적 메소드들의 초기화....
  **********/
std::vector<hyunwoo::WStringKey::RefWString> hyunwoo::WStringKey::m_stringPools = { RefWString() };
std::vector<uint32_t>						 hyunwoo::WStringKey::m_freeStrList;
hyunwoo::WStringKey::RefWStringMap			 hyunwoo::WStringKey::m_viewMap;









/*============================================================================================================================================================
 *   해당 WStringKey 객체가 참조하고 있던 문자열의 참조 카운트를 1 낮추고, 0이 되었다면 해시맵에서 제거한다....
 **********/
void hyunwoo::WStringKey::Clear()
{
	/********************************************************************
	 *   가지고 있는 키가 유효한가?
	 *******/
	RefWStringView view = { m_poolIdx };
	if (m_poolIdx!=0 && m_viewMap.contains(view)) 
	{
		RefWString& str = m_stringPools[m_poolIdx];

		/*------------------------------------------------
		 *   해당 문자열의 참조 카운트를 낮춘 후 0이 되었다면
		 *   freeList에 삽입하고, 해시맵에서 제거한다...
		 ********/
		if (--str.RefCount==0) {
			m_freeStrList.push_back(m_poolIdx);
			m_viewMap.erase(view);
			m_poolIdx = 0;
		}
	}
}












/*============================================================================================================================================================
 *  주어진 문자열에 대한 StringPool을 조회한 다음, 키를 얻어 저장합니다....
 **********/
void hyunwoo::WStringKey::SetKey(const std::wstring_view rhs_view)
{
	/******************************************************
	 *   기존에 가리키고 있던 문자열의 참조 카운트를 낮춘다...
	 ********/
	Clear();



	/*********************************************************
	 *   인자로 받은 문자열이 이미 해시맵에 등록되었을 경우,
	 *   해시맵에 등록된 키를 가져와 저장한다....
	 *******/
	RefWStringView view = { 0 };
	m_stringPools[0].Set(0, rhs_view);

	if (m_viewMap.contains(view)) {
		m_poolIdx = m_viewMap[view];
		m_stringPools[m_poolIdx].RefCount++;
	}



	/*********************************************************
	 *  등록되어있지 않을 경우, 해시맵에 해당 문자열을 등록한다...
	 *********/
	
	/*-----------------------------------------
	 *   여유있는 RefString이 존재하는가?
	 ******/
	else if (m_freeStrList.size() > 0) {
		m_poolIdx = m_freeStrList.back();
		m_freeStrList.pop_back();

		m_stringPools[m_poolIdx].Set(1, rhs_view);
		m_viewMap.insert(std::pair<RefWStringView, uint32_t>(RefWStringView{m_poolIdx}, m_poolIdx));
	}

	/*----------------------------------------
	 *   여유있는 RefString이 없다면, 풀에
	 *   새로운 RefString을 생성하여 삽입한다..
	 *******/
	else {
		m_poolIdx = m_stringPools.size();
		m_stringPools.push_back(RefWString(1, rhs_view));
		m_viewMap.insert(std::pair<RefWStringView, uint32_t>(RefWStringView{ m_poolIdx }, m_poolIdx));
	}

	m_stringPools[0].Str = L" ";
}














/*============================================================================================================================================================
 *   해당 WStringKey 객체가 복사되는 경우, 복사될 키가 유효하면 해당 문자열의 참조 카운트를 올린다...
 **********/
void hyunwoo::WStringKey::operator=(const WStringKey& prev)
{
	Clear();


	/**********************************************
	 *   복사된 키의 문자열이 유효하면, 참조 카운트를
	 *   1 올린다...
	 ******/
	RefWStringView view = { prev.m_poolIdx };

	if (m_viewMap.contains(view)) {
		m_poolIdx = m_viewMap[view];
		m_stringPools[m_poolIdx].RefCount++;
	}
}










/*============================================================================================================================================================
 *   주어진 WStringKey가 동일한 문자열을 가리키는지 확인합니다....
 **********/
bool hyunwoo::WStringKey::operator==(const WStringKey& rhs) const
{
	return (m_poolIdx==rhs.m_poolIdx);
}

bool hyunwoo::WStringKey::operator!=(const WStringKey& rhs) const
{
	return (m_poolIdx != rhs.m_poolIdx);
}
















/*============================================================================================================================================================
 *   주어진 WStringKey와 대응되는 std::wstring의 참조를 반환합니다....
 **********/
const std::wstring_view hyunwoo::WStringKey::GetView() const
{
	/****************************************************
	 *   해당 StringKey가 유효하면, 이에 대응되는 문자열의
	 *   view를 반환한다....
	 *******/
	return std::wstring_view(m_stringPools[m_poolIdx].Str);
}












/*============================================================================================================================================================
 *   주어진 RefStringView가 가리키는 문자열을 통해 해시키를 생성합니다....
 **********/
std::size_t hyunwoo::WStringKey::RefWStringView::Hasher::operator()(const RefWStringView& t) const noexcept
{
	return m_stringPools[t.PoolIdx].HashKey;
}


/*============================================================================================================================================================
 *   두 RefStringView가 동일한 문자열을 가리키고 있는지를 확인합니다....
 **********/
bool hyunwoo::WStringKey::RefWStringView::Equaler::operator()(RefWStringView const& a, RefWStringView const& b)  const noexcept
{
	return (m_stringPools[a.PoolIdx].Str == m_stringPools[b.PoolIdx].Str);
}













/*============================================================================================================================================================
 *   해당 RefWString을 초기화합니다....
 **********/
void hyunwoo::WStringKey::RefWString::Set(uint32_t refCount, std::wstring_view view)
{
	Str		 = view;
	HashKey  = std::hash<std::wstring_view>()(view);
	RefCount = refCount;
}
