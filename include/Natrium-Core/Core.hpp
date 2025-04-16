#if !defined(NA_CORE_HPP)
#define NA_CORE_HPP

#include <stdint.h>

#define NA_BIT(x) (1u << x)
#define NA_GET_FROM_OFFSET(t, p) (t*)((Na::Byte*)p - offsetof(t, p))

#define NA_FORMAT fmt::format

#if !defined(NA_CONFIG_DEBUG) && !defined(NA_CONFIG_RELEASE) && !defined(NA_CONFIG_DIST)
    #error "Has not defined a build configuration macro!"
    #error "Define NA_CONFIG_DEBUG, NA_CONFIG_RELEASE or NA_CONFIG_DIST!"
#endif // NA_CONFIG

#if !defined(NA_PLATFORM_WINDOWS) && !defined(NA_PLATFORM_LINUX)
    #error "Has not defined a platform macro!"
    #error "Define NA_PLATFORM_WINDOWS or NA_PLATFORM_LINUX!"
#endif // NA_PLATFORM

#if defined(NA_CONFIG_DIST)
    #define NA_ASSERT(x, ...)
#else
    #define NA_ASSERT(x, ...) \
        if(!(x)) {\
            throw std::runtime_error(NA_FORMAT(__VA_ARGS__));\
        }
#endif // NA_CONFIG

#if defined(NA_PLATFORM_WINDOWS)

#elif defined(NA_PLATFORM_LINUX)

#endif // NA_PLATFORM

using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

constexpr u64 NA_INVALID_HANDLE = UINT64_MAX;

namespace Na {
    using Byte = unsigned char;

    enum class BuildConfig : u8 {
        None = 0, Debug, Release, Distribution
    };

#if defined(NA_CONFIG_DEBUG)
    constexpr BuildConfig k_BuildConfig = BuildConfig::Debug;
#elif defined(NA_CONFIG_RELEASE)
    constexpr BuildConfig k_BuildConfig = BuildConfig::Release;
#elif defined(NA_CONFIG_DIST)
    constexpr BuildConfig k_BuildConfig = BuildConfig::Distribution;
#else
    constexpr BuildConfig k_BuildConfig = BuildConfig::None;
#endif // NA_CONFIG

    enum class Platform : u8 {
        None = 0, Windows, Linux
    };

#if defined(NA_PLATFORM_WINDOWS)
    constexpr Platform k_Platform = Platform::Windows;
#elif defined(NA_PLATFORM_LINUX)
    constexpr Platform k_Platform = Platform::Linux;
#else
    constexpr Platform k_Platform = Platform::None;
#endif // NA_PLATFORM

    void HelloWorld(void);

} // namespace Na

#endif // NA_CORE_HPP