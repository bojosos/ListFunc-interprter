
-- Could just build into lib and link
workspace "ListFunc"
    architecture "x86_64"
    startproject "ListFunc"

    configurations
    {
        "Relase"
    }

    flags
    {
        "MultiProcessorCompile"
    }

project "ListFunc"
    kind "ConsoleApp"
    language "c++"
    cppdialect "c++17"
    staticruntime "on"
    targetdir ("%{wks.location}/bin/" .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. "/%{prj.name}")

    files
    {
        "src/interpreter.h",
        "src/interpreter.cpp",
        "src/parser.h",
        "src/parser.cpp",
        "src/utils.cpp",
        "src/utils.h",
        "src/main.cpp",
        "src/test_runner.h",
        "src/test_runner.cpp",
        "src/nodes.h",
        "src/nodes.cpp",
        "src/library.cpp"
    }

project "ListFunc-tests"
    kind "ConsoleApp"
    language "c++"
    cppdialect "c++17"
    staticruntime "on"
    targetdir ("%{wks.location}/bin/" .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. "/%{prj.name}")

    defines { "TEST_LIB" }

    files
    {
        "src/interpreter.h",
        "src/interpreter.cpp",
        "src/parser.h",
        "src/parser.cpp",
        "src/utils.cpp",
        "src/utils.h",
        "src/test_runner.h",
        "src/test_runner.cpp",
        "src/nodes.h",
        "src/nodes.cpp",
        "src/library.cpp"
    }