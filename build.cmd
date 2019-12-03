@echo off

set VCVARSALL="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"

call %VCVARSALL%

cl /O2 /GS- /c /TC /MD muchfloat_simple.c
link /dynamicbase:no /release muchfloat_simple.obj

cl /O2 /GS- /c /TC /MD muchfloat.c
link /dynamicbase:no /release muchfloat.obj

cl /O2 /GS- /c /TC /MD muchgmp.c
link /dynamicbase:no /release gmp/libgmp.a gmp/libgcc.a gmp/libmingwex.a muchgmp.obj