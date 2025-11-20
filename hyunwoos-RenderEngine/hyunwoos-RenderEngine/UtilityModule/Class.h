#pragma once
#include <cstdint>

namespace hyunwoo {
	class Class;
}


/*=============================================================================================================================================================
 *    각 클래스들의 연산자들을 간접 호출해주는 매니저 메소드를 제공하는 클래스입니다....
 ***********/
class hyunwoo::Class final
{
	//=================================================================================
	/////////////						Defines...						 //////////////
	//=================================================================================
public:
	/************************************************
	 *   Manager 함수의 명령을 나타내는 열거형입니다...
	 *******/
	enum class Operator : uint8_t
	{
		Construct,
		Destruct,
		Copy,
		Move,
		Get_Size,
	};


	/************************************************
	 *   특정 타입을 관리할 수 있는 함수의 별칭입니다...
	 *******/
	using ManagerFunc = int(Operator op, void* from, void* to);




	//=========================================================================================
	/////////////						Public methods...						 //////////////
	//=========================================================================================
public:
	template<typename T>
	static int Manager(Operator op, void* from, void* to)
	{
		T& from_ref = *static_cast<T*>(from);
		T& to_ref   = *static_cast<T*>(to);

		/*****************************************
		 *   주어진 명령에 따라 적절히 처리한다...
		 ********/
		switch (op) 
		{
			/**생성자를 호출한다.....**/
			case(Operator::Construct): {
				new(from) T();
				return 0;
			}

			/**소멸자를 호출한다.....**/
			case(Operator::Destruct): {
				from_ref.~T();
				return 0;
			}

			/**이동 생성자를 호출한다.....**/
			case(Operator::Move): {
				new(to) T(std::move(from_ref));
				return 0;
			}

			/**복사 생성자를 호출한다.....**/
			case(Operator::Copy): {
				new(to) T(from_ref);
				return 0;
			}

			/**관리 타입의 크기를 반환한다...**/
			case(Operator::Get_Size): {
				return sizeof(T);
			}
		}

		return 0;
	}
};