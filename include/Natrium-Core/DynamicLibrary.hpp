#if !defined(NA_DYNAMIC_LIBRARY_HPP)
#define NA_DYNAMIC_LIBRARY_HPP

#include "./Core.hpp"
#include "./Logger.hpp"

namespace Na {
	class DynamicLibrary {
	public:
		DynamicLibrary(void) = default;
		inline DynamicLibrary(const std::string_view& name) { this->load(name); }
		inline ~DynamicLibrary(void) { this->unload(); }

		DynamicLibrary(const DynamicLibrary& other) = delete;
		DynamicLibrary& operator=(const DynamicLibrary& other) = delete;

		DynamicLibrary(DynamicLibrary&& other);
		DynamicLibrary& operator=(DynamicLibrary&& other);

		void load(const std::string_view& name);
		void unload(void);

		void* fn(const char* name) const;
		template<typename FnT>
		inline FnT fn(const char* name) const { return (FnT)this->fn(name); }

		inline operator bool(void) const { return m_Handle; }
		inline const std::string_view& name(void) const { return m_Name; }
		inline void* handle(void) { return m_Handle; }
	private:
		void* m_Handle = nullptr;
		std::string_view m_Name;
	};
} // namespace Na

#endif // NA_DYNAMIC_LIBRARY_HPP