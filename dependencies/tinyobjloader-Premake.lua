IncludeDirectories["tiny_obj_loader"] = "dependencies/tinyobjloader/include/"
Libraries["tiny_obj_loader"] = "tiny_obj_loader"

project "tiny_obj_loader"
    location "./tinyobjloader"
    targetname "tiny_obj_loader-bin"
    kind "StaticLib"
    staticruntime "Off"

    language "C++"
    cppdialect "C++20"
    systemversion "latest"

    files {
        "tinyobjloader/include/tiny_obj_loader/tiny_obj_loader.h",
        "tinyobjloader/src/tiny_obj_loader.cpp"
    }

    includedirs "tinyobjloader/include/"

    filter "configurations:dbg"
        symbols "On"
        runtime "Debug"

    filter "configurations:rel"
        optimize "speed"
        symbols "Off"

    filter "configurations:dist"
        optimize "speed"
        symbols "Off"
