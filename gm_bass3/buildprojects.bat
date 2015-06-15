@echo off
rmdir /s /q _project_linux_cl 2> nul > nul
rmdir /s /q _project_linux_sv 2> nul > nul
rmdir /s /q _project_osx_cl 2> nul > nul
rmdir /s /q _project_osx_sv 2> nul > nul
rmdir /s /q _project_win32_cl 2> nul > nul
rmdir /s /q _project_win32_sv 2> nul > nul
rmdir /s /q bin 2> nul > nul
del working 2> nul > nul

mkdir bin 2> nul > nul
mkdir src 2> nul > nul
mkdir inc 2> nul > nul

set modulenamefile=name.txt
set modulenamepath=%homedir%%modulenamefile%

set script=%~f0
set homedir=
set execname=
set module=


for %%a in (%script%) do set homedir=%%~dpa
for %%a in (%script%) do set execname=%%~nxa

echo GMod cross plattform C^+^+ module project file generator.
echo For developing binary modules for Windows, Linux and MacOS.
echo.
echo Version 1.0, by Grocel.
echo.
echo.
echo The modulename is saved in %modulenamefile%.
echo To change it you edit this file and run the project generator.
echo You can also change it by running: "%execname% <name>"
echo.
echo.

if not "%1"=="" (
	set module=%1

	echo %module%>"%modulenamepath%"
)

if not exist "%modulenamepath%" (
	if "%1"=="" (
		echo Name file not found.
		echo.
		
		echo Please enter the name of your module you want to create.
		echo Do not add any prefixes or suffixes.
		echo.
		echo To avoid problems only enter lowercase alphanumeric
		echo chars without whitespaces. (a-z0-9)
		echo.
		echo Examples:
		echo     "gmsv_magic_win32" ^= "magic"
		echo     "gmsv_bass_linux"  ^= "bass" 
		echo     "gmcl_nav_osx"     ^= "nav" 
		echo.
		goto ASKNAME
	)
)

:START

if "%module%"=="" (
	set /P module=<"%modulenamepath%"
)

if not "%module%"=="" (
	echo %module%>"%modulenamepath%"
)

echo VS2012 serverside:
echo.

premake4 --os=windows --platform=x32 --file=buildprojects.lua --realm=server --module=%module% vs2012
if errorlevel 1 (
	echo Coudln't create VS2012 serverside project!
	pause
	goto END
)

echo.
echo.
echo VS2012 clientside:
echo.

premake4 --os=windows --platform=x32 --file=buildprojects.lua --realm=client --module=%module% vs2012
if errorlevel 1 (
	echo Coudln't create VS2012 clientside project!
	pause
	goto END
)

echo.
echo.
echo MacOS-gmake serverside:
echo.

premake4 --os=macosx --platform=universal32 --file=buildprojects.lua --realm=server --module=%module% gmake
if errorlevel 1 (
	echo Coudln't create MacOS-gmake serverside project!
	pause
	goto END
)

echo.
echo.
echo MacOS-gmake clientside:
echo.

premake4 --os=macosx --platform=universal32 --file=buildprojects.lua --realm=client --module=%module% gmake
if errorlevel 1 (
	echo Coudln't create MacOS-gmake clientside project!
	pause
	goto END
)

echo.
echo.
echo Linux-gmake serverside:
echo.

premake4 --os=linux --platform=x32 --file=buildprojects.lua --realm=server --module=%module% gmake
if errorlevel 1 (
	echo Coudln't create Linux-gmake serverside project!
	pause
	goto END
)

echo.
echo.
echo Linux-gmake clientside:
echo.

premake4 --os=linux --platform=x32 --file=buildprojects.lua --realm=client --module=%module% gmake
if errorlevel 1 (
	echo Coudln't create Linux-gmake clientside project!
	pause
	goto END
)

echo.
echo.
pause

:END
	@exit /B 0

:ASKNAME
	set /P module=Modulename: 
	
	if "%module%"=="" (
		set module=
		del "%modulenamepath%" 2> nul > nul

		echo Please enter a valid name.
		echo.

		goto ASKNAME
	)

	goto START