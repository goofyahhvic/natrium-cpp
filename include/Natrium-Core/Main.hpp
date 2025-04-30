#if !defined(NA_MAIN_HPP)
#define NA_MAIN_HPP

#if !defined(NA_MAIN_HANDLED)

#include "./Core.hpp"

#define main Na::Main

namespace Na {
	int Main(int argc, char* argv[]);
} // namespace Na

#endif // NA_MAIN_HANDLED

#endif // NA_MAIN_HPP