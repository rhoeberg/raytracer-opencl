SET VC_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community
IF NOT DEFINED LIB (IF EXIST "%VC_PATH%" (call "%VC_PATH%\VC\Auxiliary\Build\vcvars32.bat" %1))

set includeFlags=-I ..\3rdparty\glfw\include -I ..\3rdparty\glew\include -I ..\3rdparty
set libraryFlags=glfw3dll.lib opengl32.lib glew32.lib OpenCL.lib /link /LIBPATH:"..\3rdparty\glfw\lib" /LIBPATH:"..\3rdparty\glew\lib" /LIBPATH:"..\3rdparty"

IF NOT EXIST build (
   mkdir build
   xcopy 3rdparty\glfw\lib\glfw3.dll build\
   xcopy 3rdparty\glew\bin\glew32.dll build\
)
pushd build
cl -MD -FC -Zi ..\main.cpp /Feraytracer-opencl.exe %includeFlags% %libraryFlags%
popd
