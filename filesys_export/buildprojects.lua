-- The name of your module will have.
local function SetupModule()
	newoption {
		trigger     = "module",
		description = "Give the name of the module."
	}
	if not _OPTIONS["module"] then
	   _OPTIONS["module"] = "test"
	end

	module = _OPTIONS["module"]
end

SetupModule()

local OSname = ""
local OS = os.get()

local CD = os.getcwd()

if OS == "windows" then OSname = "win32" end
if OS == "linux" then OSname = "linux" end
if OS == "macosx" then OSname = "osx" end

local projectname = module
local solutionname = projectname
local locationname = "_project_"..OSname

local SOURCE_SDK_PATH = "../sourcesdk2013/mp/src/"
local SOURCE_SDK_LINKPATH = "../sourcesdk2013/mp/src/"

if OS ~= "windows" then
	SOURCE_SDK_LINKPATH = "$(homedir)/"..SOURCE_SDK_PATH
end

local SOURCE_SDK_INCLUDES={
	SOURCE_SDK_PATH,
	SOURCE_SDK_PATH.."public/",
	SOURCE_SDK_PATH.."public/engine/",
	SOURCE_SDK_PATH.."common/",
	SOURCE_SDK_PATH.."public/vstdlib/",
	SOURCE_SDK_PATH.."public/steam/",
	SOURCE_SDK_PATH.."public/tier0/",
	SOURCE_SDK_PATH.."public/tier1/",
	SOURCE_SDK_PATH.."tier1/",
	SOURCE_SDK_PATH.."public/game/server/",
	SOURCE_SDK_PATH.."game/shared/",
	SOURCE_SDK_PATH.."game/server/",
	SOURCE_SDK_PATH.."game/client/"
}

solution(solutionname)

	language "C++"
	location ( locationname )
	flags { "Symbols", "NoPCH", "NoEditAndContinue", "StaticRuntime", "EnableSSE2", "OptimizeSpeed" }
	targetdir ( "bin/"..OSname.."/" )
	targetname ( projectname )

	targetprefix ("")
	targetsuffix ("")

	includedirs { "../include/", "inc/" }
	libdirs { "inc/", "inc/**" }

	if OS == "windows" then
		includedirs(SOURCE_SDK_INCLUDES)
		libdirs { "../sourcesdk2013/mp/src/lib/public/", "../sourcesdk2013/mp/src/lib/public/**" }

		links {"tier0","tier1","tier2","tier3","mathlib","steam_api","vstdlib"}
	end

	if OS == "linux" then 
		includedirs(SOURCE_SDK_INCLUDES)
		libdirs { "../sourcesdk2013/mp/src/lib/public/linux32/", "../sourcesdk2013/mp/src/lib/public/linux32/**" }

		linkoptions {
			SOURCE_SDK_LINKPATH.."lib/public/linux32/tier1.a",
			SOURCE_SDK_LINKPATH.."lib/public/linux32/tier2.a",
			SOURCE_SDK_LINKPATH.."lib/public/linux32/tier3.a",
			SOURCE_SDK_LINKPATH.."lib/public/linux32/mathlib.a"
		}
		
		links {"steam_api"}
		links {"tier0","vstdlib"}
		--links {"tier0_srv","vstdlib_srv"}
	end

	if OS == "macosx" then 
		targetextension (".dll")

		includedirs(SOURCE_SDK_INCLUDES)
		libdirs { "../sourcesdk2013/mp/src/lib/public/osx32", "../sourcesdk2013/mp/src/lib/public/osx32/**" }

		linkoptions {
			SOURCE_SDK_LINKPATH.."lib/public/osx32/tier1.a",
			SOURCE_SDK_LINKPATH.."lib/public/osx32/tier2.a",
			SOURCE_SDK_LINKPATH.."lib/public/osx32/tier3.a",
			SOURCE_SDK_LINKPATH.."lib/public/osx32/mathlib.a"
		}
		
		links {"steam_api"}
		links {"tier0","vstdlib"}
		--links {"tier0_srv","vstdlib_srv"}
	end
	
	configurations { "release" }

	
	configuration "release"
		defines { "NDEBUG" }
		flags{ "Optimize" }
		if OS == "windows" then
			postbuildcommands {}
		else
			postbuildcommands {}
		end
	
	project(projectname)
		defines {"GMMODULE", "_RETAIL" }
	
		if OS == "windows" then 
			defines { "_WIN32", "WIN32", "WINDOWS", CLIENT and "CLIENT" or "SERVER", "WIN32_LEAN_AND_MEAN" }
		end

		if OS == "linux" then 
			defines { "_LINUX", "LINUX", "GNUC", CLIENT and "CLIENT" or "SERVER" }
			buildoptions { "-shared", "-fpermissive", "-std=gnu++11" }
		end

		if OS == "macosx" then 
			defines { "OSX", "GNUC", CLIENT and "CLIENT" or "SERVER" }
			buildoptions { "-shared", "-fpermissive", "-std=gnu++11" }
		end

		files { "src/**.*", "../include/**.*", "inc/**.*" }
		kind "SharedLib"
		