IncludeDirectories = {}
LibraryDirectories = {}
Libraries = {}

include "dependencies/fmt-Premake.lua"
include "dependencies/stb-Premake.lua"
include "dependencies/GLFW-Premake.lua"
IncludeDirectories["glm"] = "dependencies/glm/"

project "Natrium-Core"
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
        "%{IncludeDirectories.glm}",
        "%{IncludeDirectories.glfw}",
        "include/",
        "src/Natrium-Core/"
    }

    links {
        "%{Libraries.fmt}",
        "%{Libraries.glfw}",
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

project "Natrium-Renderer"
    location "./"
    targetname "%{prj.name}-bin"
    kind "StaticLib"
    staticruntime "Off"

    language "C++"
    cppdialect "C++20"
    systemversion "latest"

    pchheader "Pch.hpp"
    pchsource "src/Natrium-Renderer/Pch.cpp"

    files {
        "include/Natrium-Renderer/**.hpp",
        "src/Natrium-Renderer/**.hpp",
        "src/Natrium-Renderer/**.cpp"
    }

    includedirs {
        "%{IncludeDirectories.stb}",
        "%{IncludeDirectories.fmt}",
        "%{IncludeDirectories.glm}",
        "%{IncludeDirectories.glfw}",
        "include/",
        "src/Natrium-Renderer/"
    }

    links {
        "%{Libraries.stb}",
        "%{Libraries.glfw}",
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