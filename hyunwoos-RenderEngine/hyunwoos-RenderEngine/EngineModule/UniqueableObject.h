#pragma once
#include <cstdint>
#include <type_traits>
#include <vector>

namespace hyunwoo {
	class UniqueableObject;

	template<typename T> struct WeakPtr;
	template<typename T> struct CachedWeakPtr;
}




/*===========================================================================================================================================================================
 *   필요시 고유의 값이 배정되어 유효성 여부를 판별할 수 있는 기반 클래스입니다. 
 *   ( ※자식 클래스들은 복사/이동 생성자 및 가상 소멸자 구현시, 반드시 UniqueableObject의 복사/이동 생성자 및 소멸자가 호출되어야 합니다. 그렇지 않으면 Undefined Behavior입니다... ※ )
 **********/
class hyunwoo::UniqueableObject
{
	template<typename T> friend struct WeakPtr;
	template<typename T> friend struct CachedWeakPtr;

	//===================================================================================================
	////////////////						     Defines...						     ////////////////////
	//===================================================================================================
public:

	/********************************************
	 *   식별할 수 있는 고유 전역 식별자를 나타내는 
	 *   구조체입니다..
	 ******/
	struct UUID final
	{
		friend class UniqueableObject;
		template<typename T> friend struct WeakPtr;
		template<typename T> friend struct CachedWeakPtr;

	private:
		union 
		{
			uint64_t Value = 0; //해당 값이 0이면 유효하지않은 UUID...

			struct {
				uint32_t Table_idx;
				uint32_t Generation;
			};
		};

	public:
		bool operator==(const UUID& rhs) const {
			return (Value == rhs.Value);
		}

		bool operator!=(const UUID& rhs) const {
			return (Value != rhs.Value);
		}

		bool IsValid() const {
			return (Value != 0);
		}
	};


private:

	/**********************************************
	 *   외부에서 포인터 유효성 검사하는데 필요한
	 *   메타 데이터용 구조체입니다...
	 *******/
	struct MemoryUsageDescriptor final
	{
		UniqueableObject* Raw_ptr	 = nullptr;
		uint32_t		  Generation = 0;
	};

	


	//===================================================================================================
	////////////////						     Fields...						     ////////////////////
	//===================================================================================================
private:
	static std::vector<MemoryUsageDescriptor> m_descs;
	static std::vector<uint32_t>			  m_free_list;
	static uint64_t							  m_dirtyUpdateID;

	UUID m_uuid;


	//==========================================================================================================
	////////////////						   Protected methods..							////////////////////
	//==========================================================================================================
protected:
	UniqueableObject() :m_uuid(){}

	UniqueableObject(const UniqueableObject& prev) 
	{ 
		operator=(prev);
	}

	UniqueableObject(UniqueableObject&& prev) noexcept
	{
		operator=(std::move(prev));
	}

	virtual ~UniqueableObject()
	{
		Make_UnUnique();
	}


	//======================================================================================================
	////////////////						   Public methods..							////////////////////
	//======================================================================================================
public:
	static uint64_t GetDirtyUpdateID() {
		return m_dirtyUpdateID;
	}

	bool IsUniqued()
	{
		return (m_uuid.Value != 0);
	}

	UUID GetUUID()
	{
		return m_uuid;
	}

	void Make_Unique()
	{
		/****************************************************
		 *   이미 고유한 상태라면 함수를 종료한다....
		 *******/
		if (m_uuid.Value != 0) {
			return;
		}


		/**********************************************
		 *  여유있는 MemoryUsageDescriptor가 없다면,
		 *  새로운 MemoryUsageDescriptor를 배정한다..
		 *******/
		if (m_free_list.empty()) {
			m_uuid.Table_idx = m_descs.size();
			m_uuid.Generation = 1;

			UniqueableObject::m_descs.push_back(MemoryUsageDescriptor{ this, 1 });
		}


		/*************************************************
		 *  여유있는 MemoryUsageDescriptor를 배정한다..
		 *******/
		else {
			uint32_t			   desc_idx = m_free_list.back();
			MemoryUsageDescriptor& desc = m_descs[desc_idx];

			m_uuid.Table_idx     = m_free_list.back();
			m_uuid.Generation    = desc.Generation;
			desc.Raw_ptr         = this;
			m_free_list.pop_back();
		}
	}




	//==========================================================================================================
	////////////////						   Private methods..							////////////////////
	//==========================================================================================================
private:
	void Make_UnUnique()
	{
		/********************************************************************************
		 *   고유한 상태라면, 배정된 MemoryUsageDescriptor를 회수한다....
		 ******/
		if (m_uuid.Value != 0)
		{
			/*------------------------------------------------
			 *   배정된 MemoryUsageDescriptor의 세대값을 올려
			 *   해당 UUID를 무효화시키고, 재사용을 위해서 freeList
			 *   에 담는다...
			 *******/
			MemoryUsageDescriptor& my_desc = m_descs[m_uuid.Table_idx];

			if (my_desc.Generation==m_uuid.Generation) {
				my_desc.Raw_ptr = nullptr;
				my_desc.Generation++;
				m_free_list.push_back(m_uuid.Table_idx);
				m_dirtyUpdateID++;
			}

			m_uuid.Value = 0;
		}
	}

	


	//==========================================================================================================
	////////////////						   Operator methods..							////////////////////
	//==========================================================================================================
public:
	void operator=(const UniqueableObject& rhs)
	{
		Make_UnUnique();
	}

	void operator=(UniqueableObject&& prev) noexcept
	{
		/***********************************************************
		 *   같은 객체간 이동일 경우, 함수를 종료한다...
		 ********/
		if (&prev==this) {
			return;
		}



		/*************************************************************
		 *   소유권을 해당 객체로 이전시키기 전, 해당 객체에 배정된
		 *   MemoryUsageDescriptor가 있는지 확인하고, 있다면 정리한다..
		 *******/
		Make_UnUnique();

		m_uuid = prev.m_uuid;
		prev.m_uuid.Value = 0;



		/**********************************************************
		 *  이전된 소유권자에게 배정된 MemoryUsageDescriptor
		 *  의 raw_ptr을, 최신화한다....
		 ******/
		if (m_uuid.Value != 0)
		{
			MemoryUsageDescriptor& desc = m_descs[m_uuid.Table_idx];

			if (desc.Generation == m_uuid.Generation) {
				desc.Raw_ptr = this;
			}
		}

		m_dirtyUpdateID++;
	}

};













/*=======================================================================================================================================================================
 *   포인터 유효성 검증용 스마트 포인터입니다. 템플릿 타입 T는 반드시 UniqueableObject를 상속한 타입이여야 합니다.
 *   nullptr이 아닌 raw_ptr로 스마트 포인터에 대입 연산자를 시도할 경우, Undefine Behavior입니다....
 **********/
template<typename T>
class hyunwoo::WeakPtr final
{
	//===================================================================================================
	////////////////						     Fields...						     ////////////////////
	//===================================================================================================
private:
	UniqueableObject::UUID m_uuid;



	//=======================================================================================================
	////////////////						   Public methods..						     ////////////////////
	//=======================================================================================================
public:
	WeakPtr() :m_uuid()
	{
		static_assert(std::is_convertible_v<T*, UniqueableObject*>, "WeakPtr<T>'s T is not base of UniqueableObject!!");
	}

	WeakPtr(T* const raw_ptr)
	:m_uuid()
	{
		static_assert(std::is_convertible_v<T*, UniqueableObject*>, "WeakPtr<T>'s T is not base of UniqueableObject!!");
		operator=(raw_ptr);
	}

	WeakPtr(const WeakPtr<T>& prev)		{ operator=(prev); }
	WeakPtr(WeakPtr<T>&& prev) noexcept	{ operator=(std::move(prev)); }


	void Reset()
	{
		m_uuid.Value = 0;
	}

	UniqueableObject::UUID GetUUID() const
	{
		return m_uuid;
	}

	T* Get() const
	{
		/****************************************************************
		 *   해당 객체가 유효하면 raw_ptr을, 아니라면 nullptr을 반환한다...
		 ******/
		UniqueableObject::MemoryUsageDescriptor& desc = UniqueableObject::m_descs[m_uuid.Table_idx];

		if (desc.Generation == m_uuid.Generation) {
			return static_cast<T*>(desc.Raw_ptr);
		}

		return nullptr;
	}




	//===========================================================================================================
	////////////////						   Operator methods..						     ////////////////////
	//===========================================================================================================
	void operator=(const WeakPtr<T>& prev)
	{
		m_uuid = prev.m_uuid;
	}

	void operator=(WeakPtr<T>&& prev) noexcept
	{
		m_uuid = prev.m_uuid;
	}

	void operator=(T* const raw_ptr)
	{
		/**************************************************************
		 *   nullptr이 대입되었을 경우, 유효하지 않은 UUID값으로 갱신한다..
		 *******/
		if (raw_ptr == nullptr) {
			m_uuid.Value = 0;
			return;
		}


		/**************************************************************
		 *   해당 객체가 고유하지 않으면, 고유한 객체로 만들고 고유해진
		 *   객체의 UUID를 복사한다...
		 ******/
		UniqueableObject::UUID& uuid  = raw_ptr->m_uuid;
		raw_ptr->Make_Unique();
		m_uuid.Value = uuid.Value;
	}

	T* operator*() const
	{
		return Get();
	}

	T* operator->() const
	{
		return Get();
	}
};






















/*=======================================================================================================================================================================
 *   마지막으로 접근한 주소값을 내부적으로 캐싱하는, 포인터 유효성 검증용 스마트 포인터입니다. 템플릿 타입 T는 반드시 UniqueableObject를 상속한 타입이여야 합니다. 
 *   nullptr이 아닌 raw_ptr로 스마트 포인터에 대입 연산자를 시도할 경우, Undefine Behavior입니다....
 **********/
template<typename T>
class hyunwoo::CachedWeakPtr final
{
	//===================================================================================================
	////////////////						     Fields...						     ////////////////////
	//===================================================================================================
private:
	T*					   m_cached_raw_ptr;
	UniqueableObject::UUID m_uuid;



	//=======================================================================================================
	////////////////						   Public methods..						     ////////////////////
	//=======================================================================================================
public:
	CachedWeakPtr() :m_uuid(), m_cached_raw_ptr(nullptr)
	{
		static_assert(std::is_convertible_v<T*, UniqueableObject*>, "CachedWeakPtr<T>'s T is not base of UniqueableObject!!");
	}

	CachedWeakPtr(T* const raw_ptr)
	:m_uuid(), m_cached_raw_ptr(nullptr)
	{
		static_assert(std::is_convertible_v<T*, UniqueableObject*>, "CachedWeakPtr<T>'s T is not base of UniqueableObject!!");
		operator=(raw_ptr);
	}

	CachedWeakPtr(const CachedWeakPtr<T>& prev)		{ operator=(prev); }
	CachedWeakPtr(CachedWeakPtr<T>&& prev) noexcept { operator=(std::move(prev)); }


	void Reset()
	{
		m_uuid.Value = 0;
	}

	UniqueableObject::UUID GetUUID()
	{
		return m_uuid;
	}

	T* Get()
	{
		/****************************************************************
		 *   해당 객체가 유효하면 raw_ptr을, 아니라면 nullptr을 반환한다...
		 ******/
		UniqueableObject::MemoryUsageDescriptor& desc = UniqueableObject::m_descs[m_uuid.Table_idx];

		if (desc.Generation == m_uuid.Generation) {
			return (m_cached_raw_ptr = static_cast<T*>(desc.Raw_ptr));
		}

		return (m_cached_raw_ptr = nullptr);
	}

	T* GetCache() const {
		return m_cached_raw_ptr;
	}



	//===========================================================================================================
	////////////////						   Operator methods..						     ////////////////////
	//===========================================================================================================
	void operator=(const CachedWeakPtr<T>& prev)
	{
		m_uuid			 = prev.m_uuid;
		m_cached_raw_ptr = prev.m_cached_raw_ptr;
	}

	void operator=(CachedWeakPtr<T>&& prev) noexcept
	{
		m_uuid = prev.m_uuid;
	}

	void operator=(T* const raw_ptr)
	{
		/**************************************************************
		 *   nullptr이 대입되었을 경우, 유효하지 않은 UUID값으로 갱신한다..
		 *******/
		if (raw_ptr == nullptr) {
			m_uuid.Value = 0;
			return;
		}


		/**************************************************************
		 *   해당 객체가 고유하지 않으면, 고유한 객체로 만들고 고유해진
		 *   객체의 UUID를 복사한다...
		 ******/
		UniqueableObject::UUID& uuid = raw_ptr->m_uuid;
		raw_ptr->Make_Unique();
		m_cached_raw_ptr = raw_ptr;
		m_uuid.Value     = uuid.Value;
	}

	T* operator*() const
	{
		return Get();
	}

	T* operator->() const
	{
		return Get();
	}
};
















