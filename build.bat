@echo off

pushd engine\stygatore
go build
popd

if not exist build mkdir build
if not exist bin mkdir bin

copy engine\stygatore\stygatore.exe bin\
bin\stygatore engine\generated

pushd build
cl /nologo /I..\ext\sdl2\include /I..\ext\lua544 ..\engine\*.c ..\ext\lua544\*.c ..\engine\base\*.c ..\engine\core\styxlua\*.c ..\engine\sys\*.c /link /LIBPATH:..\ext\sdl2\lib\x64 SDL2.lib SDL2main.lib Shell32.lib /SUBSYSTEM:CONSOLE /out:ananke.exe
popd

copy build\ananke.exe bin\
copy ext\sdl2\lib\x64\SDL2.dll bin\

