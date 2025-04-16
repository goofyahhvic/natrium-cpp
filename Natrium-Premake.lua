IncludeDirectories = {}
LibraryDirectories = {}
Libraries = {}

include "dependencies/fmt-Premake.lua"

project "NatriumCore"
    location "./"
    targetname "%{prj.name}-bin"
    kind "StaticLib"
    staticruntime "Off"

    language "C++"
    cppdialect "C++20"
    systemversion "latest"

    pchheader "Pch.hpp"
    pchsource "src/Natrium-Core/Pch.cpp"

    files {
        "include/Natrium-Core/**.hpp",
        "src/Natrium-Core/**.hpp",
        "src/Natrium-Core/**.cpp"
    }

    includedirs {
        "%{IncludeDirectories.fmt}",
        "include/",
        "src/",
        "src/Natrium-Core/"
    }

    links {
        "%{Libraries.fmt}"
    }

    filter "system:linux"
        defines { "NA_PLATFORM_LINUX" }

    filter "system:windows"
        defines {
            "NA_PLATFORM_WINDOWS",
            "_CRT_SECURE_NO_WARNINGS"
        }

        buildoptions {
            "/utf-8"
        }

    filter "configurations:dbg"
        symbols "On"
        runtime "Debug"
        defines { "NA_CONFIG_DEBUG" }

    filter "configurations:rel"
        optimize "speed"
        symbols "Off"
        defines { "NA_CONFIG_RELEASE" }

    filter "configurations:dist"
        optimize "speed"
        symbols "Off"
        defines { "NA_CONFIG_DIST" }
