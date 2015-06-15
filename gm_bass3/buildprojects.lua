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


-- The Lua state of the compiled dll.
local function SetupState()
	CLIENT = false
	SERVER = false

	newoption {
		trigger     = "realm",
		description = "Choose the realm to compile for.",
		allowed = {
			{ "server", "Serverside" },
			{ "client", "Clientside" }
		}
	}

	if not _OPTIONS["realm"] or _OPTIONS["realm"] == "server" then
	   _OPTIONS["realm"] = "server"
	   CLIENT = false
	else
	   CLIENT = true
	end

	SERVER = not CLIENT
end


SetupState()
SetupModule()

local OSname = ""
local OS = os.get()

local CD = os.getcwd()

local RMname = ""

if OS == "windows" then OSname = "win32" end
if OS == "linux" then OSname = "linux" end
if OS == "macosx" then OSname = "osx" end

if CLIENT then RMname = "cl" end
if SERVER then RMname = "sv" end

local projectname = "gm"..RMname.."_"..module.."_"..OSname
local solutionname = projectname
local locationname = "_project_"..OSname.."_"..RMname

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

local CopyToFrom={
	["gmsv_"..module.."_linux"] = {"gmsv_"..module.."_linux.dll", "../../../lib/gmsv_"..module.."_linux.dll"},
	["gmcl_"..module.."_linux"] = {"gmcl_"..module.."_linux.dll", "../../../lib/gmcl_"..module.."_linux.dll"},
	["gmsv_"..module.."_osx"] = {"gmsv_"..module.."_osx.dll", "../../../lib/gmsv_"..module.."_osx.dll"},
	["gmcl_"..module.."_osx"] = {"gmcl_"..module.."_osx.dll", "../../../lib/gmcl_"..module.."_osx.dll"},
	["gmsv_"..module.."_win32"] = {"gmsv_"..module.."_win32.dll", "../../../lib/gmsv_"..module.."_win32.dll"},
	["gmcl_"..module.."_win32"] = {"gmcl_"..module.."_win32.dll", "../../../lib/gmcl_"..module.."_win32.dll"}
}

local function Q(str)
	return "\"" .. (str or "") .. "\""
end

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

		links {"bass"}
		links {"tier0","tier1","tier2","tier3","mathlib","steam_api","vstdlib"}
	end

	if OS == "linux" then 
		targetextension (".dll")

		includedirs(SOURCE_SDK_INCLUDES)
		libdirs { "../sourcesdk2013/mp/src/lib/public/linux32/", "../sourcesdk2013/mp/src/lib/public/linux32/**" }

		linkoptions {
			SOURCE_SDK_LINKPATH.."lib/public/linux32/tier1.a",
			SOURCE_SDK_LINKPATH.."lib/public/linux32/tier2.a",
			SOURCE_SDK_LINKPATH.."lib/public/linux32/tier3.a",
			SOURCE_SDK_LINKPATH.."lib/public/linux32/mathlib.a"
		}
		
		links {"bass"}
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
		
		links {"bass"}
		links {"steam_api"}
		links {"tier0","vstdlib"}
		--links {"tier0_srv","vstdlib_srv"}
	end
	
	configurations { "release" }

	
	configuration "release"
		defines { "NDEBUG" }
		flags{ "Optimize" }
		if OS == "windows" then
			postbuildcommands {
				"@echo off",
				"@cd /D $(SolutionDir)",
				"@cd ..",
				"@cmd /C @copy_dll_win32.bat "..Q("$(TARGETDIR)/"..CopyToFrom[projectname][1]).." "..Q("$(TARGETDIR)/"..CopyToFrom[projectname][2]),
				"@cmd /C @compile_win32.bat "..module.." "..locationname
			}
		else
			postbuildcommands {
				"../copy_dll_unix.sh "..Q("$(TARGETDIR)/"..CopyToFrom[projectname][1]).." "..Q("$(TARGETDIR)/"..CopyToFrom[projectname][2]),
				"../compile_unix.sh "..Q(locationname)
			}
		end
	
	project(projectname)
		defines {"GMMODULE", "_RETAIL" }
	
		if OS == "windows" then 
			defines { "_WIN32", "WIN32", "WINDOWS", CLIENT and "CLIENT" or "SERVER", "WIN32_LEAN_AND_MEAN" }
		end

		if OS == "linux" then 
			defines { "_LINUX", "LINUX", "GNUC", CLIENT and "CLIENT" or "SERVER" }
			buildoptions { "-fpermissive", "-std=gnu++11" }
		end

		if OS == "macosx" then 
			defines { "OSX", "GNUC", CLIENT and "CLIENT" or "SERVER" }
			buildoptions { "-fpermissive", "-std=gnu++11" }
		end

		files { "src/**.*", "../include/**.*", "inc/**.*" }
		kind "SharedLib"
		