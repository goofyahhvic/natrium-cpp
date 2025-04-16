#if !defined(NA_LIST_NODE_HPP)
#define NA_LIST_NODE_HPP

#include "../Core.hpp"

namespace Na {
	template<typename t_List>
	struct DoubleList_Node {
		using T = t_List::T_t;

		DoubleList_Node* next, * previous;
		T data;

		template<typename... t_Args>
		DoubleList_Node(DoubleList_Node* next, DoubleList_Node* previous, t_Args&&... __args)
		: next(next), previous(previous), data(std::forward<t_Args>(__args)...) {}

		inline bool is_head(void) const { return !previous; }
		inline bool is_tail(void) const { return !next; }

		inline T& operator*(void) { return data; }
		inline const T& operator*(void) const { return data; }
	};
} // namespace Na

#endif // NA_LIST_NODE_HPP