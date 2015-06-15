@echo off

if "%1"=="" (
	echo Give parameter 1!
	exit /B 1
)

if "%2"=="" (
	echo Give parameter 2!
	exit /B 1
)

if "%1"=="%2" (
	echo Parameter 1 and parameter 2 can not be the same!
	exit /B 2
)

if not exist "%1" (
	echo File %1 doesn't exist!
	exit /B 3
)

:RETRY
	copy %1 %2
	
	if errorlevel 1 (
		echo Couldn't copy %1 to %2, retrying...
		pause
		goto RETRY
	)


:END
	@exit /B 0