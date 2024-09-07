cls

md .\src\bin

del water.exe
@REM del D3D.obj

@REM cl.exe  /c /EHsc /DEBUG /I "C:\Assimp\include" /I "D:\DirectX\DirectX11\DirectX11\src\Math" D3D.cpp ./src/Common/DXShaders.cpp ./src/Common/Noise.cpp ./src/Effect/Model/TextureLoader.cpp 
@REM cl.exe  /c /EHsc /I "C:\Assimp\include"  Main.cpp ./src/Common/*.cpp ./src/Effects/Water/*.cpp
cl.exe  /c /EHsc /DEBUG Main.cpp ./src/Common/*.cpp ./src/Effects/Water/*.cpp ./src/Effects/Terrain/*.cpp ./src/Effects/Cube/*.cpp

move *.obj .\src\bin\

rc.exe D3D.rc
move *.res .\src\bin\

@REM link.exe .\src\bin\*.obj .\src\bin\D3D.res /out:D3D.exe /LIBPATH:"C:\Assimp\lib\x64" /LIBPATH:"D:\DirectX\DirectX11\DirectX11\src\Lib" /NODEFAULTLIB:MSVCRT.LIB  user32.lib gdi32.lib assimp-vc143-mt.lib ole32.lib /SUBSYSTEM:WINDOWS
@REM link.exe .\src\bin\*.obj .\src\bin\D3D.res  /out:D3D.exe /LIBPATH:"C:\Assimp\lib\x64" /LIBPATH:"src\Lib" /NODEFAULTLIB:MSVCRT.LIB  user32.lib gdi32.lib assimp-vc143-mt.lib ole32.lib /SUBSYSTEM:WINDOWS
link.exe  .\src\bin\*.obj .\src\bin\D3D.res /NODEFAULTLIB:MSVCRT.LIB /out:water.exe  /LIBPATH:"src\Lib"   user32.lib gdi32.lib  ole32.lib /SUBSYSTEM:WINDOWS

water.exe
