@echo off

if not exist build mkdir build

if "%1"=="" (
	goto compile
) else (
	goto run
)


:run
echo -- Running Ananke
pushd build
ananke.exe
popd
goto end

:compile
echo -- Compiling Ananke
odin build source -collection:ext=engine/ext -collection:engine=engine -debug -out:build/ananke.exe

echo -- Copying binary files
copy /y engine\ext\lib\* build\*
goto end

:end