-- Copied from: https://github.com/TheCherno/Hazel/blob/master/vendor/premake/premake_customization/solution_items.lua

-- Implement the solutionitems command for solution-scope files
require('vstudio')

premake.api.register 
{
	name = "solutionitems",
	scope = "workspace",
	kind = "list:string",
}

premake.override(premake.vstudio.sln2005, "projects", function(base, wks)
	if wks.solutionitems and #wks.solutionitems > 0 then
		local solution_folder_GUID = "{2150E333-8FDC-42A3-9474-1A3956D46DE8}" -- See https://www.codeproject.com/Reference/720512/List-of-Visual-Studio-Project-Type-GUIDs
		premake.push("Project(\"" .. solution_folder_GUID .. "\") = \"Solution Items\", \"Solution Items\", \"{" .. os.uuid("Solution Items:" .. wks.name) .. "}\"")
		premake.push("ProjectSection(SolutionItems) = preProject")

		for _, path in ipairs(wks.solutionitems) do
			premake.w(path .. " = " .. path)
		end

		premake.pop("EndProjectSection")
		premake.pop("EndProject")
	end
	base(wks)
end)