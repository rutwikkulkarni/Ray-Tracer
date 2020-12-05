@echo off
set compile_flags= /O2 -nologo -DBUILD_WIN32=1 /Zi -I ../code/
set linker_flags= user32.lib 

if not exist build mkdir build
pushd build
cl %compile_flags% ../code/ray_main.cpp /Fe:ray.exe /link %linker_flags%
popd