include "Vendor/premake/customization/solutionitems.lua"

OUT_DIR = "%{wks.location}/bin/build/%{cfg.system}/%{cfg.architecture}/%{cfg.buildcfg}"
INT_DIR = "%{wks.location}/bin/intermediate/%{cfg.system}/%{cfg.architecture}/%{cfg.buildcfg}/%{prj.name}"

workspace "Examples"
    architecture "x86_64"

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
        cppdialect "C++20"

        defines
        {
            "_PLATFORM_WINDOWS"
        }

    filter "system:linux"
        systemversion "latest"
        pic "On"
        staticruntime "Off"
        cppdialect "gnu++20"

        defines
        {
            "_PLATFORM_LINUX"
        }

    filter "system:macosx"
        systemversion "latest"
        pic "On"
        staticruntime "Off"
        cppdialect "C++latest"

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

        defines
        {
            "NDEBUG"
        }

        flags
        {
            "NoBufferSecurityCheck",
            "NoRuntimeChecks",
            "LinkTimeOptimization",
            "NoIncrementalLink"
        }

-- This path is relative to the premake scripts for each Example, not relative to this premake script
INTRICATE_POINTERS_HPP_INCLUDE = "../../IntricatePointers/src/include"

include "Example-Ref"
include "Example-Scope"
include "Example-WeakRef"
