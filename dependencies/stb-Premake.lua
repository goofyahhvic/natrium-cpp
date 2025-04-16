IncludeDirectories["stb"] = "dependencies/stb/include/"
Libraries["stb"] = "stb"

project "stb"
    location "./"
    targetname "%{prj.name}-bin"
    kind "StaticLib"
    staticruntime "Off"

    language "C"
    cdialect "C17"
    systemversion "latest"

    files {
        "stb/include/stb/**.h",
        "stb/src/stb_impl.c"
    }

    includedirs "stb/include/"

    filter "configurations:dbg"
        symbols "On"
        runtime "Debug"

    filter "configurations:rel"
        optimize "speed"
        symbols "Off"

    filter "configurations:dist"
        optimize "speed"
        symbols "Off"
