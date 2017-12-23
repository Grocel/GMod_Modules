@echo off

set module=%1

set compiler="C:\Windows\Microsoft.NET\Framework\v4.0.30319\MSBuild.exe"
set dir=_project_win32_
set realm=sv

set script=%~f0
set homedir=

for %%a in (%script%) do set homedir=%%~dpa

if "%module%"=="" (
	set /P module=<"%homedir%name.txt"
)


if "%compiling%"=="X" (
	echo Skipping...
	echo.

	exit /B 0
)
set compiling=X

:START
	if "%2"=="%dir%sv" (
		echo Skipping project_win32_sv...
		echo.

		set realm=cl
		goto COMPILE
	)

	if "%2"=="%dir%cl" (
		echo Skipping project_win32_cl...
		echo.

		set realm=sv
		goto COMPILE
	)

	if "%2"=="" (
		set realm=sv
		goto COMPILE
	)

:COMPILE
	set realmdir=%dir%%realm%
	set projectfile=gm%realm%_%module%_win32.sln

	if not exist "%homedir%%realmdir%" (
		echo Skipping %realmdir%...
		echo.

		goto NEXT
	)

	if not exist "%homedir%%realmdir%\%projectfile%" (
		echo The file "%realmdir%\%projectfile%" is missing!
		echo Skipping %realmdir%...
		echo.

		goto NEXT
	)

	echo Compiling %realmdir%...
	echo.

	%compiler% "%homedir%%realmdir%\%projectfile%" /t:rebuild

	if errorlevel 1 (
		echo Coudln't compile %realmdir%!
		echo.
		echo.
		echo.

		goto NEXT
	)

	echo Compiling %realmdir% done!...
	echo.
	echo.
	echo.

:NEXT
	if "%2"=="" (
		if "%realmdir%"=="_project_win32_sv" (
			set realm=cl
			goto COMPILE
		)
	)

:END
	set compiling=""
	pause > nul
	@exit /B 0