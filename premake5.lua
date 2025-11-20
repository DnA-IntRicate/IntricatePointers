include "Vendor/premake/customization/solutionitems.lua"

local function pickCppVersion(targetVersion)
    if os.target() == "windows" then
        return "C++" .. targetVersion
    elseif os.target() == "linux" then
        return "gnu++" .. targetVersion
    end

    return "Default"
end

local cppTargetVer = "23"
CPP_VER = pickCppVersion(cppTargetVer)
print("C++ version: ISO " .. CPP_VER .. ".\n")

OUT_DIR = "%{wks.location}/bin/build/%{cfg.system}/%{cfg.architecture}/%{cfg.buildcfg}"
INT_DIR = "%{wks.location}/bin/intermediate/%{cfg.system}/%{cfg.architecture}/%{cfg.buildcfg}/%{prj.name}"

-- This path is relative to the premake scripts for each Example/Test, not relative to this premake script
INTRICATE_POINTERS_HPP_INCLUDE = "../../IntricatePointers/src/include"

include "Examples"
include "Tests"
