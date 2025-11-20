workspace "Examples"
    architecture "x86_64"
    cppdialect (CPP_VER)

    configurations
    {
        "Debug",
        "Release"
    }

    solutionitems
    {
        "../.editorconfig"
    }

    flags
    {
        "MultiProcessorCompile"
    }

    defines
    {
        "_CRT_SECURE_NO_DEPRECATE",
        "_CRT_SECURE_NO_WARNINGS",
        "_CRT_NONSTDC_NO_WARNINGS",
        "_SILENCE_ALL_CXX20_DEPRECATION_WARNINGS"
    }

    filter "system:windows"
        systemversion "latest"
        staticruntime "Off"

        defines
        {
            "_PLATFORM_WINDOWS"
        }

    filter "system:linux"
        systemversion "latest"
        pic "On"
        staticruntime "Off"

        defines
        {
            "_PLATFORM_LINUX"
        }

    filter "system:macosx"
        systemversion "latest"
        pic "On"
        staticruntime "Off"

        defines
        {
            "_PLATFORM_OSX"
        }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "Full"

        defines
        {
            "_DEBUG"
        }

    filter "configurations:Release"
        runtime "Release"
        symbols "Off"
        optimize "Full"
        linktimeoptimization "on"

        defines
        {
            "NDEBUG"
        }

        flags
        {
            "NoBufferSecurityCheck",
            "NoRuntimeChecks",
            "NoIncrementalLink"
        }

include "Example-Ref"
include "Example-Scope"
include "Example-WeakRef"
