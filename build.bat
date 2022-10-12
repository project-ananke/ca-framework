@echo off

set compile_flags= /nologo /Zi /FC /W4 /WX /wd4100 /wd4189 /wd4201 /wd4115 /wd4211 /wd4311 /I ..\source\engine
set link_flags= /opt:ref /incremental:no

set platform_link_flags= /subsystem:windows shlwapi.lib user32.lib gdi32.lib dxguid.lib d3d11.lib dxgi.lib d3dcompiler.lib %link_flags% 

REM What if we only use DLLs to minimize the exe size?
if not exist build mkdir build

pushd source/engine/stygatore
go build
popd

move source/engine/stygatore/stygatore build/

pushd build

echo -- Compiling Styx platform layer. 
cl %compile_flags% ..\source\platform.c /link %platform_link_flags% /out:styx_platform.exe

echo -- Cleaning up miscellaneous files.
del /s /q *.obj *.lib *.exp vc140.pdb

popd
