@echo off

if not exist build mkdir build
pushd build
cl /nologo /I..\ext\SDL2\include ..\source\main.c /link /LIBPATH:..\ext\lib\x64 SDL2.lib SDL2main.lib /out:ananke.exe
popd
