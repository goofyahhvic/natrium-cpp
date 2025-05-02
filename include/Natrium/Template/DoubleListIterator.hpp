#if !defined(NA_DOUBLE_ITERATOR_HPP)
#define NA_DOUBLE_ITERATOR_HPP

#include "../Core.hpp"

namespace Na {
	enum forwards_t { forwards };
	enum backwards_t { backwards };

	template<typename t_List>
	class DoubleList_Iterator {
	public:
		using T = t_List::T_t;
		using Node = t_List::Node;
		using This = DoubleList_Iterator;
	public:
		inline This& operator++(void) { node = node->next; return *this; }
		inline This  operator++(int) { Node* temp = node; node = node->next; return temp; }

		inline This& operator--(void) { node = node->previous; return *this; }
		inline This  operator--(int) { Node* temp = node; node = node->previous; return temp; }

		inline This& operator+=(u64 amount)
		{
			for (u64 i = 0; i < amount; i++)
				++(*this);
			return *this;
		}

		inline This& operator-=(u64 amount)
		{
			for (u64 i = 0; i < amount; i++)
				--(*this);
			return *this;
		}

		[[nodiscard]] inline This operator+(u64 amount) const { This it(node); return it += amount; }
		[[nodiscard]] inline This operator-(u64 amount) const { This it(node); return it -= amount; }

		[[nodiscard]] inline T* operator->(void) const { return &(node->data); }
		[[nodiscard]] inline T& operator*(void) const { return node->data; }
		[[nodiscard]] inline T* get(void) { return &(node->data); }

		[[nodiscard]] inline bool operator==(const This& other) const { return node == other.node; }

		[[nodiscard]] inline t_List::reverse_iterator reverse(void) { return node; }
		inline DoubleList_Iterator(Node* node = nullptr)
		: node(node) {}

		inline DoubleList_Iterator(Node* node, u64 amount, forwards_t)
		: node(node) { *this += amount; }

		inline DoubleList_Iterator(Node* node, u64 amount, backwards_t)
		: node(node) { *this -= amount; }

		Node* node;
	};

	template<typename t_List>
	class DoubleList_ConstIterator {
	public:
		using T = t_List::T_t;
		using Node = t_List::Node;
		using This = DoubleList_ConstIterator;
	public:
		inline This& operator++(void) { node = node->next; return *this; }
		inline This  operator++(int) { const Node* temp = node; node = node->next; return temp; }

		inline This& operator--(void) { node = node->previous; return *this; }
		inline This  operator--(int) { const Node* temp = node; node = node->previous; return temp; }

		inline This& operator+=(u64 amount)
		{
			for (u64 i = 0; i < amount; i++)
				++(*this);
			return *this;
		}

		inline This& operator-=(u64 amount)
		{
			for (u64 i = 0; i < amount; i++)
				--(*this);
			return *this;
		}

		[[nodiscard]] inline This operator+(u64 amount) const { This it(node); return it += amount; }
		[[nodiscard]] inline This operator-(u64 amount) const { This it(node); return it -= amount; }

		[[nodiscard]] inline const T* operator->(void) const { return &(node->data); }
		[[nodiscard]] inline const T& operator*(void) const { return node->data; }
		[[nodiscard]] inline const T* get(void) { return &(node->data); }

		[[nodiscard]] inline bool operator==(const This& other) const { return node == other.node; }

		[[nodiscard]] inline t_List::const_reverse_iterator reverse(void) { return node; }

		inline DoubleList_ConstIterator(const Node* node = nullptr)
		: node(node) {}

		inline DoubleList_ConstIterator(const Node* node, u64 amount, forwards_t)
		: node(node) { *this += amount; }

		inline DoubleList_ConstIterator(const Node* node, u64 amount, backwards_t)
		: node(node) { *this -= amount; }

		const Node* node;
	};

	template<typename t_List>
	class DoubleList_ReverseIterator {
	public:
		using T = t_List::T_t;
		using Node = t_List::Node;
		using This = DoubleList_ReverseIterator;
	public:
		inline This& operator++(void) { node = node->previous; return *this; }
		inline This  operator++(int) { Node* temp = node; node = node->previous; return temp; }

		inline This& operator--(void) { node = node->next; return *this; }
		inline This  operator--(int) { Node* temp = node; node = node->next; return temp; }

		inline This& operator+=(u64 amount)
		{
			for (u64 i = 0; i < amount; i++)
				--(*this);
			return *this;
		}

		inline This& operator-=(u64 amount)
		{
			for (u64 i = 0; i < amount; i++)
				++(*this);
			return *this;
		}

		[[nodiscard]] inline This operator+(u64 amount) const { This it(node); return it += amount; }
		[[nodiscard]] inline This operator-(u64 amount) const { This it(node); return it -= amount; }

		[[nodiscard]] inline T* operator->(void) const { return &(node->data); }
		[[nodiscard]] inline T& operator*(void) const { return node->data; }
		[[nodiscard]] inline T* get(void) { return &(node->data); }

		[[nodiscard]] inline bool operator==(const This& other) const { return node == other.node; }

		[[nodiscard]] inline t_List::iterator reverse(void) { return node; }

		inline DoubleList_ReverseIterator(Node* node = nullptr)
		: node(node) {}

		inline DoubleList_ReverseIterator(Node* node, u64 amount, forwards_t)
		: node(node) { *this += amount; }

		inline DoubleList_ReverseIterator(Node* node, u64 amount, backwards_t)
		: node(node) { *this -= amount; }

		Node* node;
	};

	template<typename t_List>
	class DoubleList_ConstReverseIterator {
	public:
		using T = t_List::T_t;
		using Node = t_List::Node;
		using This = DoubleList_ConstReverseIterator;
	public:
		inline This& operator++(void) { node = node->previous; return *this; }
		inline This  operator++(int) { const Node* temp = node; node = node->previous; return temp; }

		inline This& operator--(void) { node = node->next; return *this; }
		inline This  operator--(int) { const Node* temp = node; node = node->next; return temp; }

		inline This& operator+=(u64 amount)
		{
			for (u64 i = 0; i < amount; i++)
				--(*this);
			return *this;
		}

		inline This& operator-=(u64 amount)
		{
			for (u64 i = 0; i < amount; i++)
				++(*this);
			return *this;
		}

		[[nodiscard]] inline This operator+(u64 amount) const { This it(node); return it += amount; }
		[[nodiscard]] inline This operator-(u64 amount) const { This it(node); return it -= amount; }

		[[nodiscard]] inline const T* operator->(void) const { return &(node->data); }
		[[nodiscard]] inline const T& operator*(void) const { return node->data; }
		[[nodiscard]] inline const T* get(void) { return &(node->data); }

		[[nodiscard]] inline bool operator==(const This& other) const { return node == other.node; }

		[[nodiscard]] inline t_List::const_iterator reverse(void) { return node; }

		inline DoubleList_ConstReverseIterator(const Node* node = nullptr)
		: node(node) {}

		inline DoubleList_ConstReverseIterator(const Node* node, u64 amount, forwards_t)
		: node(node) { *this += amount; }

		inline DoubleList_ConstReverseIterator(const Node* node, u64 amount, backwards_t)
		: node(node) { *this -= amount; }

		const Node* node;
	};
} // namespace Na

#endif // NA_DOUBLE_ITERATOR_HPP