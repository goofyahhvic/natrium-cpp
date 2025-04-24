#if !defined(NA_ARRAY_VECTOR_HPP)
#define NA_ARRAY_VECTOR_HPP

#include "./ArrayIterator.hpp"

namespace Na {
	template<typename T>
	class ArrayVector {
	public:
		using iterator = Array_Iterator<ArrayVector>;
		using reverse_iterator = Array_ReverseIterator<ArrayVector>;
		using const_iterator = Array_ConstIterator<ArrayVector>;
		using const_reverse_iterator = Array_ConstReverseIterator<ArrayVector>;
		using T_t = T;
	public:
		inline ArrayVector(void)
		: m_Size(0), m_Buffer(nullptr)
		{}

		inline ArrayVector(u64 size)
		: m_Size(0), m_Buffer(tmalloc<T>(size))
		{
			for (; m_Size < size; m_Size++)
				new (m_Buffer + m_Size) T();
		}

		inline ArrayVector(const T* buffer, u64 size)
		: m_Size(0), m_Buffer(tmalloc<T>(size))
		{
			for (; m_Size < size; m_Size++)
				new (m_Buffer + m_Size) T(buffer[m_Size]);
		}

		inline ArrayVector(T* buffer, u64 size)
		: m_Size(0), m_Buffer(tmalloc<T>(size))
		{
			for (; m_Size < size; m_Size++)
				new (m_Buffer + m_Size) T(std::move(buffer[m_Size]));
		}

		inline ArrayVector(const std::initializer_list<T>& list)
		: ArrayVector(list.begin(), list.size())
		{}

		inline ~ArrayVector(void) { this->clear(); }

		inline ArrayVector(const ArrayVector& other)
		: ArrayVector(other.m_Buffer, other.m_Size) {}

		ArrayVector& operator=(const ArrayVector& other)
		{
			if (m_Size != other.m_Size)
			{
				for (u64 i = 0; i < m_Size; i++)
					m_Buffer[i].~T();
				free(m_Buffer);
				m_Buffer = tmalloc<T>(other.m_Size);
			} else
			{
				for (u64 i = 0; i < m_Size; i++)
					m_Buffer[i].~T();
			}

			for (m_Size = 0; m_Size < other.m_Size; m_Size++)
				new (m_Buffer + m_Size) T(other.m_Buffer[m_Size]);

			return *this;
		}

		ArrayVector(ArrayVector&& other)
		{
			memcpy(this, &other, sizeof(ArrayVector));
			memset(&other, 0, sizeof(ArrayVector));
		}

		ArrayVector& operator=(ArrayVector&& other)
		{
			for (u64 i = 0; i < m_Size; i++)
				m_Buffer[i].~T();
			free(m_Buffer);
			memcpy(this, &other, sizeof(ArrayVector));
			memset(&other, 0, sizeof(ArrayVector));
			return *this;
		}

		inline void clear(void)
		{
			for (u64 i = 0; i < m_Size; i++)
				m_Buffer[i].~T();
			free(m_Buffer);
			m_Buffer = nullptr;
			m_Size = 0;
		}

		inline void resize(u64 new_size)
		{
			if (m_Size == new_size)
				return;

			if (m_Size < new_size)
			{
				m_Buffer = trealloc<T>(m_Buffer, new_size);
				for (; m_Size < new_size; m_Size++)
					new (m_Buffer + m_Size) T();
				return;
			}

			for (u64 i = m_Size; i < new_size; i++)
				m_Buffer[i].~T();
			m_Buffer = trealloc<T>(m_Buffer, new_size);
		}

		template<typename... t_Args>
		u64 emplace(t_Args&&... __args)
		{
			m_Buffer = trealloc<T>(m_Buffer, ++m_Size);
			new (m_Buffer + m_Size - 1) T(std::forward<t_Args>(__args)...);
			return m_Size;
		}

		bool pop(void)
		{
			if (m_Size)
			{
				m_Buffer[--m_Size].~T();
				m_Buffer = trealloc<T>(m_Buffer, m_Size);
				return true;
			}
			return false;
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

		[[nodiscard]] inline u64 size(void) const { return m_Size; }
		[[nodiscard]] inline bool empty(void) const { return !m_Size; }
	private:
		u64 m_Size;
		T* m_Buffer;
	};
} // namespace Na

#endif // NA_ARRAY_VECTOR_HPP