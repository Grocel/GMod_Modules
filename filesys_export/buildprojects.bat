@echo off
rmdir /s /q _project_linux 2> nul > nul
rmdir /s /q _project_win32 2> nul > nul
rmdir /s /q bin 2> nul > nul
del working 2> nul > nul

mkdir bin 2> nul > nul
mkdir src 2> nul > nul
mkdir inc 2> nul > nul

set module=filesys_export

:START

echo VS2012:
echo.

premake4 --os=windows --platform=x32 --file=buildprojects.lua --module=%module% vs2012
if errorlevel 1 (
	echo Coudln't create VS2012 project!
	pause
	goto END
)

echo.
echo.
echo Linux-gmake:
echo.

premake4 --os=linux --platform=x32 --file=buildprojects.lua --module=%module% gmake
if errorlevel 1 (
	echo Coudln't create Linux-gmake project!
	pause
	goto END
)

echo.
echo.
pause

:END
	@exit /B 0