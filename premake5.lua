include "Vendor/premake/customization/solutionitems.lua"
include "Examples"
include "Tests"

OUT_DIR = "%{wks.location}/bin/build/%{cfg.system}/%{cfg.architecture}/%{cfg.buildcfg}"
INT_DIR = "%{wks.location}/bin/intermediate/%{cfg.system}/%{cfg.architecture}/%{cfg.buildcfg}/%{prj.name}"

-- This path is relative to the premake scripts for each Example/Test, not relative to this premake script
INTRICATE_POINTERS_HPP_INCLUDE = "../../IntricatePointers/src/include"
