#if !defined(NA_ARRAY_LIST_HPP)
#define NA_ARRAY_LIST_HPP

#include "./ArrayIterator.hpp"

namespace Na {
	template<typename T>
	class ArrayList {
	public:
		using iterator = Array_Iterator<ArrayList>;
		using reverse_iterator = Array_ReverseIterator<ArrayList>;
		using const_iterator = Array_ConstIterator<ArrayList>;
		using const_reverse_iterator = Array_ConstReverseIterator<ArrayList>;
		using T_t = T;
	public:
		inline ArrayList(void)
		: m_Capacity(0), m_Size(0), m_Buffer(nullptr)
		{}

		inline ArrayList(u64 capacity, u64 size = 0)
		: m_Capacity(capacity), m_Size(size), m_Buffer(tmalloc<T>(capacity))
		{}

		inline ArrayList(const T* buffer, u64 size)
		: m_Capacity(size), m_Size(0), m_Buffer(tmalloc<T>(size))
		{
			while (m_Size < size)
				this->emplace_d(buffer[m_Size]);
		}

		inline ArrayList(T* buffer, u64 size)
		: m_Capacity(size), m_Size(size), m_Buffer(tmalloc<T>(size))
		{
			while (m_Size < size)
				this->emplace_d(std::move(buffer[m_Size]));
		}

		inline ArrayList(const std::initializer_list<T>& list)
		: ArrayList(list.begin(), list.size())
		{}

		inline ~ArrayList(void)
		{
			if (!m_Capacity)
				return;

			while (m_Size)
				this->pop();
			free(m_Buffer);
			memset(this, 0, sizeof(ArrayList));
		}

		inline ArrayList(const ArrayList& other)
		: ArrayList(other.m_Buffer, other.m_Size) {}

		inline ArrayList& operator=(const ArrayList& other)
		{
			if (m_Capacity < other.m_Capacity)
			{
				free(m_Buffer);
				m_Buffer = tmalloc<T>(other.m_Capacity);
				m_Capacity = other.m_Capacity;
			}

			while (m_Size < other.m_Size)
				this->emplace_d(other[m_Size]);

			return *this;
		}

		inline ArrayList(ArrayList&& other)
		{
			memcpy(this, &other, sizeof(ArrayList));
			memset(&other, 0, sizeof(ArrayList));
		}

		inline ArrayList& operator=(ArrayList&& other)
		{
			this->clear();
			free(m_Buffer);
			memcpy(this, &other, sizeof(ArrayList));
			memset(&other, 0, sizeof(ArrayList));
			return *this;
		}

		// capacity is not changed
		bool clear(void)
		{
			bool cleared = m_Size;
			while (m_Size)
				this->pop();
			return cleared;
		}

		inline void resize(u64 new_size) { m_Size = new_size; }

		inline void reallocate(u64 new_capacity)
		{
			if (new_capacity == m_Capacity)
				return;
			m_Buffer = trealloc<T>(m_Buffer, new_capacity);
			m_Capacity = new_capacity;
		}

		inline void reallocate(u64 new_capacity, u64 new_size)
		{
			m_Size = new_size;
			this->reallocate(new_capacity);
		}

		template<typename... ArgsT>
		inline u64 emplace(ArgsT&&... __args)
		{
			if (m_Size == m_Capacity)
				this->reallocate(m_Capacity * 2 + 1);
			new (m_Buffer + m_Size) T(std::forward<ArgsT>(__args)...);
			return m_Size++;
		}

		template<typename... ArgsT>
		inline u64 emplace_d(ArgsT&&... __args)
		{
			new (m_Buffer + m_Size) T(std::forward<ArgsT>(__args)...);
			return m_Size++;
		}

		inline bool pop(void)
		{
			if (m_Size)
				m_Buffer[--m_Size].~T();
			return m_Size;
		}

		[[nodiscard]] inline iterator begin(void) { return m_Buffer; }
		[[nodiscard]] inline const_iterator begin(void) const { return m_Buffer; }
		[[nodiscard]] inline const_iterator cbegin(void) const { return m_Buffer; }

		[[nodiscard]] inline iterator end(void) { return m_Buffer + m_Size; }
		[[nodiscard]] inline const_iterator end(void) const { return m_Buffer + m_Size; }
		[[nodiscard]] inline const_iterator cend(void) const { return m_Buffer + m_Size; }

		[[nodiscard]] inline reverse_iterator rbegin(void) { return m_Buffer + m_Size - 1; }
		[[nodiscard]] inline const_reverse_iterator rbegin(void) const { return m_Buffer + m_Size - 1; }
		[[nodiscard]] inline const_reverse_iterator crbegin(void) const { return m_Buffer + m_Size - 1; }

		[[nodiscard]] inline reverse_iterator rend(void) { return m_Buffer - 1; }
		[[nodiscard]] inline const_reverse_iterator rend(void) const { return m_Buffer - 1; }
		[[nodiscard]] inline const_reverse_iterator crend(void) const { return m_Buffer - 1; }

		[[nodiscard]] inline iterator at(u64 index) { return m_Buffer + index; }
		[[nodiscard]] inline const_iterator at(u64 index) const { return m_Buffer + index; }

		[[nodiscard]] inline T& operator[](u64 index) { return m_Buffer[index]; }
		[[nodiscard]] inline const T& operator[](u64 index) const { return m_Buffer[index]; }

		[[nodiscard]] inline T& operator*(void) { return *m_Buffer; }
		[[nodiscard]] inline const T& operator*(void) const { return *m_Buffer; }

		[[nodiscard]] inline T* operator->(void) { return m_Buffer; }
		[[nodiscard]] inline const T* operator->(void) const { return m_Buffer; }

		[[nodiscard]] inline T* ptr(void) { return m_Buffer; }
		[[nodiscard]] inline const T* ptr(void) const { return m_Buffer; }

		[[nodiscard]] inline T& head(void) { return *m_Buffer; }
		[[nodiscard]] inline const T& head(void) const { return *m_Buffer; }

		[[nodiscard]] inline T& tail(void) { return *(m_Buffer + m_Size - 1); }
		[[nodiscard]] inline const T& tail(void) const { return *(m_Buffer + m_Size - 1); }

		[[nodiscard]] inline u64 capacity(void) const { return m_Capacity; }
		[[nodiscard]] inline u64 size(void) const { return m_Size; }
		[[nodiscard]] inline u64 free_space(void) const { return m_Capacity - m_Size; }
		[[nodiscard]] inline bool empty(void) const { return !m_Size; }
		[[nodiscard]] inline bool full(void) const { return m_Size == m_Capacity; }
	private:
		u64 m_Capacity, m_Size;
		T* m_Buffer;
	};
} // namespace Na

#endif // NA_ARRAY_LIST_HPP