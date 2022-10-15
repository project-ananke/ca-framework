@echo off

pushd source\engine\stygatore
go build
popd

if not exist build mkdir build
if not exist bin mkdir bin

copy source\engine\stygatore\stygatore.exe bin\
bin\stygatore source\engine\generated

pushd build
cl /nologo /I..\ext\sdl2\include /I..\ext\lua544 ..\source\*.c ..\ext\lua544\*.c ..\source\engine\*.c ..\source\engine\base\*.c ..\source\engine\core\styxlua\*.c ..\source\engine\os\*.c /link /LIBPATH:..\ext\sdl2\lib\x64 SDL2.lib SDL2main.lib Shell32.lib /SUBSYSTEM:CONSOLE /out:ananke.exe
popd

copy build\ananke.exe bin\
copy ext\sdl2\lib\x64\SDL2.dll bin\

