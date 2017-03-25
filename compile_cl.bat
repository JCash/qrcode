
echo off

if NOT DEFINED VCINSTALLDIR (
    if exist "C:\Program Files (x86)\Microsoft Visual Studio 15.0\VC\vcvarsall.bat" (
        call "C:\Program Files (x86)\Microsoft Visual Studio 15.0\VC\vcvarsall.bat" amd64
        echo "USING VISUAL STUDIO 15"
    )
)

if NOT DEFINED VCINSTALLDIR (
    if exist "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" (
        call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64
        echo "USING VISUAL STUDIO 14"
    )
)

if NOT DEFINED VCINSTALLDIR (
    if exist "C:\Program Files (x86)\Microsoft Visual Studio 13.0\VC\vcvarsall.bat" (
        call "C:\Program Files (x86)\Microsoft Visual Studio 13.0\VC\vcvarsall.bat" amd64
        echo "USING VISUAL STUDIO 13"
    )
)

if NOT DEFINED VCINSTALLDIR (
    if exist "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" (
        call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" amd64
        echo "USING VISUAL STUDIO 12"
    )
)

if NOT DEFINED VCINSTALLDIR (
    echo "No compatible visual studio found! run vcvarsall.bat first!"
)

mkdir build

cl.exe /nologo /O2 /c /Isrc src/stb_wrapper.c -Fobuild/stb_wrapper.obj
cl.exe /nologo /O2 /EHsc /D_CRT_SECURE_NO_WARNINGS /W4 /Isrc build/stb_wrapper.obj src/encode.c /link /out:.\build\qrencode.exe

del *.obj

