@echo off

set homedir=

set script=%~f0
for %%a in (%script%) do set homedir=%%~dpa

cd %homedir%
bash -c ./compile_unix.sh %1

if errorlevel 1 (
	echo.
	echo Bash is missing, you need to install the Cygwin Terminal
	echo with "make", "g++" and "gcc"!
	echo This allows you to compile for unix systems on windows.
	echo.
	echo Make sure you add the "<cygwinpath>/bin" directory to your PATH environment variable too.
	echo.

	exit /B 1
)

exit /B 0
