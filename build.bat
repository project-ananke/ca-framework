@echo off

if not exist build mkdir build

if "%1"=="" (
	goto compile
) else (
	goto run
)

:compile
echo -- Compiling Ananke
odin build source -collection:ext=./engine/ext -show-timings -collection:engine=./engine -debug -out:build/ananke.exe
goto end

:run
echo -- Compiling Ananke
odin build source -collection:ext=./engine/ext -show-timings -collection:engine=./engine -debug -out:build/ananke.exe
echo -- Running Ananke
pushd build
ananke.exe
popd
goto end

:end