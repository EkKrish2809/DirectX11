cls

md .\src\bin

del D3D.exe
del D3D.obj

cl.exe  /c /EHsc /DEBUG /I "C:\Assimp\include" /I "D:\DirectX\ModelLoading\FromLearnOpenGL\05-D3DModel_MyFormat\src\Math" D3D.cpp ./src/Common/DXShaders.cpp ./src/Common/Noise.cpp ./src/Effect/Model/TextureLoader.cpp 
@REM cl.exe  /c /EHsc /I "C:\Assimp\include" D3D.cpp ./src/Common/DXShaders.cpp ./src/Effect/Model/TextureLoader.cpp 

move *.obj .\src\bin\

rc.exe D3D.rc
move *.res .\src\bin\

link.exe .\src\bin\*.obj .\src\bin\D3D.res /out:D3D.exe /LIBPATH:"C:\Assimp\lib\x64" /LIBPATH:"D:\DirectX\ModelLoading\FromLearnOpenGL\05-D3DModel_MyFormat\src\Lib" /NODEFAULTLIB:MSVCRT.LIB  user32.lib gdi32.lib assimp-vc143-mt.lib ole32.lib /SUBSYSTEM:WINDOWS

D3D.exe
