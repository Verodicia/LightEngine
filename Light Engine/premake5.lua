project "Light Engine"

    kind "StaticLib"
    staticruntime "on"

    cppdialect "C++17"
    language   "C++"

    targetdir (TargetDir)
    objdir    (ObjectDir)

    defines "_CRT_SECURE_NO_WARNINGS"

    files "%{prj.location}/**.**"
    excludes "%{prj.location}/**.vcxproj**"

	pchheader "ltpch.h"
	pchsource "src/Engine/ltpch.cpp"

    links
    {
		"glfw"         ,
		"glad"         ,
		"ImGui"        ,
		"freetype"     ,
        "spdlog"       ,
        "opengl32.lib" ,
		"irrKlang.lib" ,
    }

    includedirs
    {
		"%{prj.location}/src/Engine/"                   ,
		"%{prj.location}/src/"                          ,
		"%{wks.location}/glfw/include"                  ,
		"%{wks.location}/glad/"                         ,
		"%{wks.location}/ImGui/"                        ,
		"%{wks.location}/freetype/"                     ,
		"%{wks.location}/spdlog/"                       ,
		"%{wks.location}/Dependencies/glm/"             ,
		"%{wks.location}/Dependencies/irrKlang/include" ,
		"%{wks.location}/Dependencies/stb_image"        ,
    }

	libdirs
	{
		"%{wks.location}/Dependencies/irrKlang/lib" ,
	}


	-- Operating System
	filter "system:not windows"
		excludes "%{prj.location}/src/Platform/DirectX**"

	filter "system:windows"
		links
		{
			"d3d11.lib"       ,
			"dxguid.lib"      ,
			"D3DCompiler.lib" ,
		}


    -- Configurations
    filter "configurations:debug"
		defines  "LIGHT_DEBUG"
		optimize "debug"
		runtime  "debug"
		symbols  "on"

    filter "configurations:release"
		defines "LIGHT_RELEASE"
		optimize "on"
		runtime  "release"

    filter "configurations:distribution"
		defines "LIGHT_DIST"
		optimize "on"
		runtime  "release"