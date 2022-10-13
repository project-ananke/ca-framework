@echo off

if not exist build mkdir build
pushd build

cl /nologo /I..\ext\SDL2\include ..\source\main.c /link /LIBPATH:..\ext\SDL2\lib\x64 SDL2.lib SDL2main.lib Shell32.lib /SUBSYSTEM:CONSOLE /out:ananke.exe
copy ..\ext\SDL2\lib\SDL2.dll .

popd
