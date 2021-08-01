workspace "Template"
	location ".\\build\\"
	startproject "Manual Map"

	targetdir "%{wks.location}\\bin\\%{cfg.buildcfg}\\"
	objdir "%{wks.location}\\obj\\%{cfg.buildcfg}\\%{prj.name}\\"
	buildlog "%{wks.location}\\obj\\%{cfg.buildcfg}\\%{prj.name}.log"

	largeaddressaware "on"
	editandcontinue "off"
	staticruntime "on"

	systemversion "latest"
	characterset "mbcs" --unicode for wide string support
	architecture "x86"
	warnings "extra"

	buildoptions {
		"/Zm200",
		"/utf-8",
		"/std:c++latest",
		"/bigobj",
	}

	flags {
		"noincrementallink",
		"no64bitchecks",
		"shadowedvariables",
		"undefinedidentifiers",
		"multiprocessorcompile",
	}

	defines {
		"NOMINMAX",
		"WIN32_LEAN_AND_MEAN",
		"_CRT_SECURE_NO_WARNINGS",
		"_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS",
	}

	platforms {
		"x86",
	}
	
	configurations {
		"Release",
		"Debug",
	}
	
	--Same as in visual studio
	--postbuildcommands {
		--"if \"%COMPUTERNAME%\" == \"DESKTOP-BA0K95Q\" ( copy /y \"$(TargetPath)\" \"E:\\Path\\To\\Final\\Place\\\" )",
--	}
	
	configuration "Release"
		defines "NDEBUG"
		optimize "full"
		runtime "release"
		symbols "off"

	configuration "Debug"
		defines "DEBUG"
		optimize "debug"
		runtime "debug"
		symbols "on"

	--Would recommend changing project name and targetname to suit your needs
	project "Manual Map"
		targetname "Output_Name"

		language "c++"
		kind "consoleapp"
		warnings "off"
		
		--REQUIRED FOR THE MANUAL MAP | COMPILER MAGIC
		linkoptions "/NXCOMPAT:NO /IGNORE:4254 /DYNAMICBASE:NO /SAFESEH:NO /LARGEADDRESSAWARE /LAST:.main"
		
		--Only if you want to change the dll directory of discord_game_sdk.dll or others you may need
		--//!\\MANDATORY COPY INTO PROPERTIES//!\\--
		
			--LINKER > INPUT > DELAY LOADED DLLS
			
		--[[
		
			discord_game_sdk.dll
		
		]]--