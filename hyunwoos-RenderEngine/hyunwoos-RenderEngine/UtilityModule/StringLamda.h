#pragma once
#include <string>
#include <type_traits>

namespace hyunwoo {
	class StringLamda;
	class WStringLamda;

	using $  = class StringLamda;
	using w$ = class WStringLamda;
}


/*=======================================================================
 *    C++에서 멀티바이트 String 조합을 만드는 유틸리티 클래스입니다...
 *=============*/
class hyunwoo::StringLamda final
{
private:
	//===============================================================
	//////////					Defines..					/////////
	//===============================================================
	template <typename T>
	struct has_add_string_method : std::false_type {};

	template<typename T> requires std::is_same_v<decltype(&std::decay_t<T>::AddString), void(std::decay_t<T>::*)(std::string&) const>
	struct has_add_string_method<T> : std::true_type {};

	template<typename T>
	static constexpr bool has_add_string_method_v = has_add_string_method<T>::Symbol;





	//================================================================
	/////////					Fields...					//////////
	//================================================================
	static std::string m_sharedStr;




	//==================================================================
	////////////			Public methods..				////////////
	//==================================================================
public:
	StringLamda() = delete;
	StringLamda(const StringLamda&) = delete;

	template<typename... Args>
	constexpr explicit StringLamda(Args&&... args) 
	{
		m_sharedStr.clear();

		/*************************************
		 *  템플릿 용으로 사용될 내부 함수...
		 *****/
		constexpr auto lamda = [](auto&& arg)
		{
			using arg_t = std::decay_t<decltype(arg)>;

			/**원시 자료형인가?**/
			if constexpr (std::is_fundamental_v<arg_t>) {
				m_sharedStr += std::to_string(arg);
			}

			/***C-Style 문자열인가..?**/
			else if constexpr (std::is_same_v<arg_t, const char*>) {
				m_sharedStr += std::string(arg);
			}

			/**이외의 자료형일 경우, 내부적으로 전용 메소드가 정의되어있는가?**/
			else if constexpr(has_add_string_method_v<arg_t>) {
				arg.AddString(m_sharedStr);
			}

			/**이외의 경우, 컴파일 에러를 발생시킨다...**/
			else {
				static_assert(std::_Always_false<arg_t>, "Define an instance method of type void AddString(std::string&) for the given type.");
			}
		};

		(lamda(args), ...);
	}



	//=======================================================================
	//////////				  Operator methods..				  ///////////
	//=======================================================================
	constexpr operator const char* () {
		return m_sharedStr.c_str();
	}

	constexpr operator const std::string& () {
		return m_sharedStr;
	}

};





/*=======================================================================
 *    C++에서 유니코드 String 조합을 만드는 유틸리티 클래스입니다...
 *=============*/
class hyunwoo::WStringLamda final
{
private:
	//===============================================================
	//////////					Defines..					/////////
	//===============================================================
	template <typename T>
	struct has_add_wstring_method : std::false_type {};

	template<typename T> requires std::is_same_v<decltype(&std::decay_t<T>::AddWString), void(std::decay_t<T>::*)(std::wstring&) const>
	struct has_add_wstring_method<T> : std::true_type {};

	template<typename T>
	static constexpr bool has_add_string_method_v = has_add_wstring_method<T>::value;





	//================================================================
	/////////					Fields...					//////////
	//================================================================
	static std::wstring m_sharedStr;




	//==================================================================
	////////////			Public methods..				////////////
	//==================================================================
public:
	WStringLamda()                    = delete;
	WStringLamda(const WStringLamda&) = delete;

	template<typename... Args>
	constexpr explicit WStringLamda(Args&&... args)
	{
		m_sharedStr.clear();

		/*************************************
		 *  템플릿 용으로 사용될 내부 함수...
		 *****/
		constexpr auto lamda = [](auto&& arg)
		{
			using arg_t = std::decay_t<decltype(arg)>;

			/**원시 자료형인가?**/
			if constexpr (std::is_fundamental_v<arg_t>) {
				m_sharedStr += std::to_wstring(arg);
			}

			/***C-Style 문자열인가..?**/
			else if constexpr (std::is_same_v<arg_t, const wchar_t*>) {
				m_sharedStr += std::wstring(arg);
			}

			/**이외의 자료형일 경우, 내부적으로 전용 메소드가 정의되어있는가?**/
			else if constexpr (has_add_string_method_v<arg_t>) {
				arg.AddWString(m_sharedStr);
			}

			/**이외의 경우, 컴파일 에러를 발생시킨다...**/
			else {
				static_assert(std::_Always_false<arg_t>, "Define an instance method of type 'void AddWString(std::wstring&) const' for the given type.");
			}
		};

		(lamda(args), ...);
	}



	//=======================================================================
	//////////				  Operator methods..				  ///////////
	//=======================================================================
	constexpr operator const wchar_t* () {
		return m_sharedStr.c_str();
	}

	constexpr operator const std::wstring& () {
		return m_sharedStr;
	}

};